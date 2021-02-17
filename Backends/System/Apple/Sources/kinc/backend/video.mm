#include "pch.h"

#include <kinc/video.h>

#import <AVFoundation/AVFoundation.h>
#include <kinc/audio1/audio.h>
#include <kinc/graphics4/texture.h>
#include <kinc/log.h>
#include <kinc/system.h>
#include <kinc/backend/VideoSoundStream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const char* iphonegetresourcepath();
extern const char* macgetresourcepath();

Kore::VideoSoundStream::VideoSoundStream(int nChannels, int freq) : bufferSize(1024 * 100), bufferReadPosition(0), bufferWritePosition(0), read(0), written(0) {
	buffer = new float[bufferSize];
}

void Kore::VideoSoundStream::insertData(float* data, int nSamples) {
	for (int i = 0; i < nSamples; ++i) {
		float value = data[i]; // / 32767.0;
		buffer[bufferWritePosition++] = value;
		++written;
		if (bufferWritePosition >= bufferSize) {
			bufferWritePosition = 0;
		}
	}
}

float Kore::VideoSoundStream::nextSample() {
	++read;
	if (written <= read) {
		printf("Out of audio\n");
		return 0;
	}
	if (bufferReadPosition >= bufferSize) {
		bufferReadPosition = 0;
		printf("buffer read back - %i\n", (int)(written - read));
	}
	return buffer[bufferReadPosition++];
}

bool Kore::VideoSoundStream::ended() {
	return false;
}

static void load(kinc_video_t *video, double startTime) {
    video->impl.videoStart = startTime;
    AVURLAsset* asset = [[AVURLAsset alloc] initWithURL:video->impl.url options:nil];
    video->impl.videoAsset = asset;

    AVAssetTrack* videoTrack = [[asset tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0];
    NSDictionary* videoOutputSettings =
        [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA], kCVPixelBufferPixelFormatTypeKey, nil];
    AVAssetReaderTrackOutput* videoOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:videoTrack outputSettings:videoOutputSettings];

    bool hasAudio = [[asset tracksWithMediaType:AVMediaTypeAudio] count] > 0;
    AVAssetReaderAudioMixOutput* audioOutput = nullptr;
    if (hasAudio) {
        AVAssetTrack* audioTrack = [[asset tracksWithMediaType:AVMediaTypeAudio] objectAtIndex:0];
        NSDictionary* audioOutputSettings = [NSDictionary
            dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:kAudioFormatLinearPCM], AVFormatIDKey, [NSNumber numberWithFloat:44100.0], AVSampleRateKey,
                                         [NSNumber numberWithInt:32], AVLinearPCMBitDepthKey, [NSNumber numberWithBool:NO], AVLinearPCMIsNonInterleaved,
                                         [NSNumber numberWithBool:YES], AVLinearPCMIsFloatKey, [NSNumber numberWithBool:NO], AVLinearPCMIsBigEndianKey, nil];
        audioOutput = [AVAssetReaderAudioMixOutput assetReaderAudioMixOutputWithAudioTracks:@[ audioTrack ] audioSettings:audioOutputSettings];
    }

    AVAssetReader* reader = [AVAssetReader assetReaderWithAsset:asset error:nil];

    if (startTime > 0) {
        CMTimeRange timeRange = CMTimeRangeMake(CMTimeMake(startTime * 1000, 1000), kCMTimePositiveInfinity);
        reader.timeRange = timeRange;
    }

    [reader addOutput:videoOutput];
    if (hasAudio) {
        [reader addOutput:audioOutput];
    }

    video->impl.assetReader = reader;
    video->impl.videoTrackOutput = videoOutput;
    if (hasAudio) {
        video->impl.audioTrackOutput = audioOutput;
    }
    else {
        video->impl.audioTrackOutput = nullptr;
    }

    if (video->impl.myWidth < 0) video->impl.myWidth = [videoTrack naturalSize].width;
    if (video->impl.myHeight < 0) video->impl.myHeight = [videoTrack naturalSize].height;
    int framerate = [videoTrack nominalFrameRate];
    kinc_log(KINC_LOG_LEVEL_INFO, "Framerate: %i\n", framerate);
    video->impl.next = video->impl.videoStart;
    video->impl.audioTime = video->impl.videoStart * 44100;
}

void kinc_video_init(kinc_video_t *video, const char *filename) {
    video->impl.playing = false;
    video->impl.sound = nullptr;
    video->impl.image_initialized = false;
	char name[2048];
#ifdef KORE_IOS
	strcpy(name, iphonegetresourcepath());
#else
	strcpy(name, macgetresourcepath());
#endif
	strcat(name, "/");
	strcat(name, KORE_DEBUGDIR);
	strcat(name, "/");
	strcat(name, filename);
	video->impl.url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:name]];
	video->impl.myWidth = -1;
	video->impl.myHeight = -1;
	load(video, 0);
}

void kinc_video_destroy(kinc_video_t *video) {
	kinc_video_stop(video);
}

#ifdef KORE_IOS
void iosPlayVideoSoundStream(Kore::VideoSoundStream* video);
void iosStopVideoSoundStream();
#else
void macPlayVideoSoundStream(Kore::VideoSoundStream* video);
void macStopVideoSoundStream();
#endif

void kinc_video_play(kinc_video_t *video) {
	AVAssetReader* reader = video->impl.assetReader;
	[reader startReading];

	video->impl.sound = new Kore::VideoSoundStream(2, 44100);
// Mixer::play(sound);
#ifdef KORE_IOS
	iosPlayVideoSoundStream((Kore::VideoSoundStream*)video->impl.sound);
#else
	macPlayVideoSoundStream((Kore::VideoSoundStream*)video->impl.sound);
#endif

	video->impl.playing = true;
	video->impl.start = kinc_time() - video->impl.videoStart;
}

void kinc_video_pause(kinc_video_t *video) {
	video->impl.playing = false;
	if (video->impl.sound != nullptr) {
// Mixer::stop(sound);
#ifdef KORE_IOS
		iosStopVideoSoundStream();
#else
		macStopVideoSoundStream();
#endif
        Kore::VideoSoundStream* sound = (Kore::VideoSoundStream*)video->impl.sound;
		delete sound;
		video->impl.sound = nullptr;
	}
}

void kinc_video_stop(kinc_video_t *video) {
	kinc_video_pause(video);
}

static void updateImage(kinc_video_t *video) {
	if (!video->impl.playing) return;
	{
		AVAssetReaderTrackOutput* videoOutput = video->impl.videoTrackOutput;
		CMSampleBufferRef buffer = [videoOutput copyNextSampleBuffer];
		if (!buffer) {
			AVAssetReader* reader = video->impl.assetReader;
			if ([reader status] == AVAssetReaderStatusCompleted) {
				kinc_video_stop(video);
			}
			else {
				kinc_video_pause(video);
				load(video, video->impl.next);
				kinc_video_play(video);
			}
			return;
		}
		video->impl.next = CMTimeGetSeconds(CMSampleBufferGetOutputPresentationTimeStamp(buffer));

		CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(buffer);

		if (!video->impl.image_initialized) {
			CGSize size = CVImageBufferGetDisplaySize(pixelBuffer);
			video->impl.myWidth = size.width;
			video->impl.myHeight = size.height;
			kinc_g4_texture_init(&video->impl.image, kinc_video_width(video), kinc_video_height(video), KINC_IMAGE_FORMAT_BGRA32);
			video->impl.image_initialized = true;
		}

		if (pixelBuffer != NULL) {
			CVPixelBufferLockBaseAddress(pixelBuffer, 0);
#ifdef KORE_OPENGL
			kinc_g4_texture_upload(&video->impl.image, (uint8_t*)CVPixelBufferGetBaseAddress(pixelBuffer), static_cast<int>(CVPixelBufferGetBytesPerRow(pixelBuffer) / 4));
#endif
			CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
		}
		CFRelease(buffer);
	}

	if (video->impl.audioTrackOutput != nullptr) {
		AVAssetReaderAudioMixOutput* audioOutput = video->impl.audioTrackOutput;
		while (video->impl.audioTime / 44100.0 < video->impl.next + 0.1) {
			CMSampleBufferRef buffer = [audioOutput copyNextSampleBuffer];
			if (!buffer) return;
			CMItemCount numSamplesInBuffer = CMSampleBufferGetNumSamples(buffer);
			AudioBufferList audioBufferList;
			CMBlockBufferRef blockBufferOut = nil;
			CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(buffer, NULL, &audioBufferList, sizeof(audioBufferList), NULL, NULL,
			                                                        kCMSampleBufferFlag_AudioBufferList_Assure16ByteAlignment, &blockBufferOut);
			for (int bufferCount = 0; bufferCount < audioBufferList.mNumberBuffers; ++bufferCount) {
				float* samples = (float*)audioBufferList.mBuffers[bufferCount].mData;
                Kore::VideoSoundStream* sound = (Kore::VideoSoundStream*)video->impl.sound;
				if (video->impl.audioTime / 44100.0 > video->impl.next - 0.1) {
					sound->insertData(samples, (int)numSamplesInBuffer * 2);
				}
				else {
					// Send some data anyway because the buffers are huge
					sound->insertData(samples, (int)numSamplesInBuffer);
				}
				video->impl.audioTime += numSamplesInBuffer;
			}
			CFRelease(blockBufferOut);
			CFRelease(buffer);
		}
	}
}

void kinc_video_update(kinc_video_t *video, double time) {
	if (video->impl.playing && time >= video->impl.start + video->impl.next) {
		updateImage(video);
	}
}

int kinc_video_width(kinc_video_t *video) {
	return video->impl.myWidth;
}

int kinc_video_height(kinc_video_t *video) {
	return video->impl.myHeight;
}

kinc_g4_texture_t *kinc_video_current_image(kinc_video_t *video) {
	kinc_video_update(video, kinc_time());
	return &video->impl.image;
}

double kinc_video_duration(kinc_video_t *video) {
    return 0.0;
}

bool kinc_video_finished(kinc_video_t *video) {
    return false;
}

bool kinc_video_paused(kinc_video_t *video) {
    return !video->impl.playing;
}

double kinc_video_position(kinc_video_t *video) {
    return 0.0;
}
