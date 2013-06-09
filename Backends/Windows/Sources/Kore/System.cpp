#include "pch.h"
#include <Kore/System.h>
#include <Kore/Application.h>
#include <Kore/Input/KeyEvent.h>
#include <Kore/Input/Keyboard.h>
#include <Kore/Input/Mouse.h>
#include <Kore/Graphics/Graphics.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <exception>

using namespace Kore;

namespace {
	int mouseX, mouseY;
	bool keyPressed[256];
	Kore::KeyCode keyTranslated[256]; //http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx

	void initKeyTranslation() {
		for (int i = 0; i < 256; ++i) keyTranslated[i] = Kore::Key_unknown;

		keyTranslated[VK_OEM_5  ] = Kore::Key_Huetchen;
		keyTranslated[VK_OEM_102] = Kore::Key_GermanLessMore;

		keyTranslated[VK_BACK] = Kore::Key_Backspace;
		keyTranslated[VK_TAB] = Kore::Key_Tab;
		//keyTranslated[VK_CLEAR]
		keyTranslated[VK_RETURN] = Kore::Key_Return;
		keyTranslated[VK_SHIFT] = Kore::Key_Shift;
		keyTranslated[VK_CONTROL] = Kore::Key_Control;
		keyTranslated[VK_MENU] = Kore::Key_Alt;
		keyTranslated[VK_PAUSE] = Kore::Key_Pause;
		keyTranslated[VK_CAPITAL] = Kore::Key_CapsLock;
		//keyTranslated[VK_KANA]
		//keyTranslated[VK_HANGUEL]
		//keyTransltaed[VK_HANGUL]
		//keyTransltaed[VK_JUNJA]
		//keyTranslated[VK_FINAL]
		//keyTranslated[VK_HANJA]
		//keyTranslated[VK_KANJI]
		keyTranslated[VK_ESCAPE] = Kore::Key_Escape;
		//keyTranslated[VK_CONVERT]
		//keyTranslated[VK_NONCONVERT
		//keyTranslated[VK_ACCEPT
		//keyTranslated[VK_MODECHANGE
		keyTranslated[VK_SPACE] = Kore::Key_Space;
		keyTranslated[VK_PRIOR] = Kore::Key_PageUp;
		keyTranslated[VK_NEXT] = Kore::Key_PageDown;
		keyTranslated[VK_END] = Kore::Key_End;
		keyTranslated[VK_HOME] = Kore::Key_Home;
		keyTranslated[VK_LEFT] = Kore::Key_Left;
		keyTranslated[VK_UP] = Kore::Key_Up;
		keyTranslated[VK_RIGHT] = Kore::Key_Right;
		keyTranslated[VK_DOWN] = Kore::Key_Down;
		//keyTranslated[VK_SELECT
		keyTranslated[VK_PRINT] = Kore::Key_Print;
		//keyTranslated[VK_EXECUTE
		//keyTranslated[VK_SNAPSHOT
		keyTranslated[VK_INSERT] = Kore::Key_Insert;
		keyTranslated[VK_DELETE] = Kore::Key_Delete;
		keyTranslated[VK_HELP] = Kore::Key_Help;
		keyTranslated[0x30] = Kore::Key_0;
		keyTranslated[0x31] = Kore::Key_1;
		keyTranslated[0x32] = Kore::Key_2;
		keyTranslated[0x33] = Kore::Key_3;
		keyTranslated[0x34] = Kore::Key_4;
		keyTranslated[0x35] = Kore::Key_5;
		keyTranslated[0x36] = Kore::Key_6;
		keyTranslated[0x37] = Kore::Key_7;
		keyTranslated[0x38] = Kore::Key_8;
		keyTranslated[0x39] = Kore::Key_9;
		keyTranslated[0x41] = Kore::Key_A;
		keyTranslated[0x42] = Kore::Key_B;
		keyTranslated[0x43] = Kore::Key_C;
		keyTranslated[0x44] = Kore::Key_D;
		keyTranslated[0x45] = Kore::Key_E;
		keyTranslated[0x46] = Kore::Key_F;
		keyTranslated[0x47] = Kore::Key_G;
		keyTranslated[0x48] = Kore::Key_H;
		keyTranslated[0x49] = Kore::Key_I;
		keyTranslated[0x4A] = Kore::Key_J;
		keyTranslated[0x4B] = Kore::Key_K;
		keyTranslated[0x4C] = Kore::Key_L;
		keyTranslated[0x4D] = Kore::Key_M;
		keyTranslated[0x4E] = Kore::Key_N;
		keyTranslated[0x4F] = Kore::Key_O;
		keyTranslated[0x50] = Kore::Key_P;
		keyTranslated[0x51] = Kore::Key_Q;
		keyTranslated[0x52] = Kore::Key_R;
		keyTranslated[0x53] = Kore::Key_S;
		keyTranslated[0x54] = Kore::Key_T;
		keyTranslated[0x55] = Kore::Key_U;
		keyTranslated[0x56] = Kore::Key_V;
		keyTranslated[0x57] = Kore::Key_W;
		keyTranslated[0x58] = Kore::Key_X;
		keyTranslated[0x59] = Kore::Key_Y;
		keyTranslated[0x5A] = Kore::Key_Z;
		//keyTranslated[VK_LWIN
		//keyTranslated[VK_RWIN
		//keyTranslated[VK_APPS
		//keyTranslated[VK_SLEEP
		//keyTranslated[VK_NUMPAD0]
		//keyTranslated[VK_NUMPAD1
		//keyTranslated[VK_NUMPAD2
		//keyTranslated[VK_NUMPAD3
		//keyTranslated[VK_NUMPAD4
		//keyTranslated[VK_NUMPAD5
		//keyTranslated[VK_NUMPAD6
		//keyTranslated[VK_NUMPAD7
		//keyTranslated[VK_NUMPAD8
		//keyTranslated[VK_NUMPAD9
		keyTranslated[VK_MULTIPLY] = Kore::Key_multiply;
		//keyTranslated[VK_ADD]
		//keyTranslated[VK_SEPARATOR
		//keyTranslated[VK_SUBTRACT
		//keyTranslated[VK_DECIMAL
		//keyTranslated[VK_DIVIDE
		keyTranslated[VK_F1] = Kore::Key_F1;
		keyTranslated[VK_F2] = Kore::Key_F2;
		keyTranslated[VK_F3] = Kore::Key_F3;
		keyTranslated[VK_F4] = Kore::Key_F4;
		keyTranslated[VK_F5] = Kore::Key_F5;
		keyTranslated[VK_F6] = Kore::Key_F6;
		keyTranslated[VK_F7] = Kore::Key_F7;
		keyTranslated[VK_F8] = Kore::Key_F8;
		keyTranslated[VK_F9] = Kore::Key_F9;
		keyTranslated[VK_F10] = Kore::Key_F10;
		keyTranslated[VK_F11] = Kore::Key_F11;
		keyTranslated[VK_F12] = Kore::Key_F12;
		//keyTranslated[VK_F13
		//keyTranslated[VK_F14
		//keyTranslated[VK_F15
		//keyTranslated[VK_F16
		//keyTranslated[VK_F17
		//keyTranslated[VK_F18
		//keyTranslated[VK_F19
		//keyTranslated[VK_F20
		//keyTranslated[VK_F21
		//keyTranslated[VK_F22
		//keyTranslated[VK_F23
		//keyTranslated[VK_F24
		keyTranslated[VK_NUMLOCK] = Kore::Key_NumLock;
		keyTranslated[VK_SCROLL] = Kore::Key_ScrollLock;
		//0x92-96 //OEM specific
		//keyTranslated[VK_LSHIFT]
		//keyTranslated[VK_RSHIFT
		//keyTranslated[VK_LCONTROL]
		//keyTranslated[VK_RCONTROL
		//keyTranslated[VK_LMENU
		//keyTranslated[VK_RMENU
		//keyTranslated[VK_BROWSER_BACK
		//keyTranslated[VK_BROWSER_FORWARD
		//keyTranslated[VK_BROWSER_REFRESH
		//keyTranslated[VK_BROWSER_STOP
		//keyTranslated[VK_BROWSER_SEARCH
		//keyTranslated[VK_BROWSER_FAVORITES
		//keyTranslated[VK_BROWSER_HOME
		//keyTranslated[VK_VOLUME_MUTE
		//keyTranslated[VK_VOLUME_DOWN
		//keyTranslated[VK_VOLUME_UP
		//keyTranslated[VK_MEDIA_NEXT_TRACK
		//keyTranslated[VK_MEDIA_PREV_TRACK
		//keyTranslated[VK_MEDIA_STOP
		//keyTranslated[VK_MEDIA_PLAY_PAUSE
		//keyTranslated[VK_LAUNCH_MAIL
		//keyTranslated[VK_LAUNCH_MEDIA_SELECT
		//keyTranslated[VK_LAUNCH_APP1
		//keyTranslated[VK_LAUNCH_APP2
		//keyTranslated[VK_OEM_1 //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ';:' key
		keyTranslated[VK_OEM_PLUS] = Kore::Key_Plus;
		keyTranslated[VK_OEM_COMMA] = Kore::Key_Comma;
		keyTranslated[VK_OEM_MINUS] = Kore::Key_Minus;
		keyTranslated[VK_OEM_PERIOD] = Kore::Key_Period;
		//keyTranslated[VK_OEM_2 //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key
		//keyTranslated[VK_OEM_3 //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key
		//keyTranslated[VK_OEM_4 //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key
		//keyTranslated[VK_OEM_5 //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\|' key
		//keyTranslated[VK_OEM_6 //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key
		//keyTranslated[VK_OEM_7 //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key
		//keyTranslated[VK_OEM_8 //Used for miscellaneous characters; it can vary by keyboard.
		//keyTranslated[0xE1 //OEM specific
		//keyTranslated[VK_OEM_102 //Either the angle bracket key or the backslash key on the RT 102-key keyboard
		//0xE3-E4 //OEM specific
		//keyTranslated[VK_PROCESSKEY
		//0xE6 //OEM specific
		//keyTranslated[VK_PACKET //Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods.
		//0xE9-F5 //OEM specific
		//keyTranslated[VK_ATTN
		//keyTranslated[VK_CRSEL
		//keyTranslated[VK_EXSEL
		//keyTranslated[VK_EREOF
		//keyTranslated[VK_PLAY
		//keyTranslated[VK_ZOOM
		//keyTranslated[VK_NONAME
		//keyTranslated[VK_PA1
		//keyTranslated[PA1 key
		//keyTranslated[VK_OEM_CLEAR
	}

	uint r = 0;
}

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_MOVE:
	case WM_MOVING:
	case WM_SIZE:
	case WM_SIZING:
		//Scheduler::breakTime();
		break;
	case WM_DESTROY:
		Application::the()->stop();
		return 0;
	case WM_ERASEBKGND:
		return 1;
	case WM_MOUSEMOVE:
		Mouse::the()->_move(MouseEvent(LOWORD(lParam), HIWORD(lParam)));
		break;
	case WM_LBUTTONDOWN:
		Mouse::the()->_pressLeft(MouseEvent(LOWORD(lParam), HIWORD(lParam)));
		break;
	case WM_LBUTTONUP:
		Mouse::the()->_releaseLeft(MouseEvent(LOWORD(lParam), HIWORD(lParam)));
		break;
	case WM_RBUTTONDOWN:
		mouseX = LOWORD(lParam);
		mouseY = HIWORD(lParam);
		Mouse::the()->_pressRight(MouseEvent(LOWORD(lParam), HIWORD(lParam)));
		break;
	case WM_RBUTTONUP:
		mouseX = LOWORD(lParam);
		mouseY = HIWORD(lParam);
		Mouse::the()->_releaseRight(MouseEvent(LOWORD(lParam), HIWORD(lParam)));
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!keyPressed[wParam]) {
			keyPressed[wParam] = true;
			Keyboard::the()->keydown(KeyEvent(keyTranslated[wParam]));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		keyPressed[wParam] = false;
		Keyboard::the()->keyup(KeyEvent(keyTranslated[wParam]));
		break;
	case WM_SYSCOMMAND:
		//printf("WS_SYSCOMMAND %5d %5d %5d\n", msg, wParam, lParam);
		switch (wParam) {
		case SC_KEYMENU: // Ignorieren, wenn Alt f�r das WS_SYSMENU gedr�ckt wird.
			return 0;
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0; // Prevent From Happening

		// Pause game when window is minimized, continue when it's restored or maximized.
		//
		// Unfortunately, if the game would continue to run when minimized, the graphics in
		// the Windows Vista/7 taskbar would not be updated, even when Direct3DDevice::Present()
		// is called without error. I do not know why.
		case SC_MINIMIZE:
			//Scheduler::haltTime(); // haltTime()/unhaltTime() is incremental, meaning that this doesn't interfere with when the game itself calls these functions
			break;
		case SC_RESTORE:
		case SC_MAXIMIZE:
			//Scheduler::unhaltTime();
			break;
		}
		break;
	}
	return DefWindowProcA(hWnd, msg, wParam, lParam);
}

bool Kore::System::handleMessages() {
	static MSG message;
	while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) DispatchMessageA(&message);
	return true;
}

vec2i Kore::System::mousePos() {
	return vec2i(mouseX, mouseY);
}

#undef CreateWindow

namespace {
	HWND hwnd = nullptr;

	const char* windowClassName = "KoreWindow";

	void registerWindowClass(HINSTANCE hInstance) {
		WNDCLASSEXA wc = { sizeof(WNDCLASSEXA), CS_CLASSDC, MsgProc, 0L, 0L, hInstance, LoadIcon(hInstance, MAKEINTRESOURCE(107)), nullptr /*LoadCursor(0, IDC_ARROW)*/, 0, 0, windowClassName, 0 };
		RegisterClassExA(&wc);
	}
}

void* Kore::System::createWindow() {
	HINSTANCE inst = GetModuleHandleA(nullptr);
	registerWindowClass(inst);

	DWORD dwExStyle;
	DWORD dwStyle;

	RECT WindowRect;
	WindowRect.left = 0;
	WindowRect.right = Application::the()->width();
	WindowRect.top = 0;
	WindowRect.bottom = Application::the()->height();
	
	if (Application::the()->fullscreen()) {
		DEVMODEA dmScreenSettings;					// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));		// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= Application::the()->width();			// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= Application::the()->height();			// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= 32;				// Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettingsA(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			//return FALSE;
		}
		
		dwExStyle = WS_EX_APPWINDOW;					// Window Extended Style
		dwStyle = WS_POPUP;						// Windows Style
		ShowCursor(FALSE);
	}
	else {
		dwStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	}
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size
	hwnd = CreateWindowExA(dwExStyle, windowClassName, Kore::Application::the()->name(), WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle, 100, 100, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, nullptr, nullptr, inst, nullptr);
	if (Application::the()->fullscreen()) SetWindowPos(hwnd, nullptr, 0, 0, Application::the()->width(), Application::the()->height(), 0);
	GetFocus();
	::SetCursor(LoadCursor(0, IDC_ARROW));

	if (!Application::the()->fullscreen()) {
		uint yres = GetSystemMetrics(SM_CYSCREEN);

		// Fenster rechts von Textkonsole positionieren
		RECT r;
		r.left   = 8 * 80 + 44;
		r.top    = 0;
		r.right  = r.left + Application::the()->width() - 1;
		r.bottom = r.top + Application::the()->height() - 1;
		uint h = r.bottom - r.top + 1;
		AdjustWindowRect(&r, dwStyle, FALSE); // Rahmen usw. miteinberechnen
		MoveWindow(hwnd, r.left, (yres - h) >> 1, r.right - r.left + 1, r.bottom - r.top + 1, TRUE);
	}

	return hwnd;
}

void* Kore::System::windowHandle() {
	return hwnd;
}

void Kore::System::destroyWindow() {
	if (hwnd && !DestroyWindow(hwnd)) {
		//MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hwnd = nullptr;
	}
	if (!UnregisterClassA(windowClassName, GetModuleHandleA(nullptr))) {
		//MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		//hInstance=NULL;
	}
}

void Kore::System::changeResolution(int width, int height, bool fullscreen) {
#ifndef OPENGL
	Application::the()->setWidth(width);
	Application::the()->setHeight(height);
	Application::the()->setFullscreen(fullscreen);

	if (!Application::the()->fullscreen()) {
		uint yres = GetSystemMetrics(SM_CYSCREEN);

		// Fenster rechts von Textkonsole positionieren
		RECT r;
		r.left   = 8 * 80 + 44;
		r.top    = 0;
		r.right  = r.left + Application::the()->width() - 1;
		r.bottom = r.top + Application::the()->height() - 1;
		uint h = r.bottom - r.top + 1;
		DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		AdjustWindowRect(&r, dwStyle, FALSE); // Rahmen usw. miteinberechnen
		MoveWindow(hwnd, r.left, (yres - h) >> 1, r.right - r.left + 1, r.bottom - r.top + 1, TRUE);

		Graphics::changeResolution(width, height);
	}
#endif
}

namespace {
	bool keyboardshown = false;
}

void Kore::System::showKeyboard() {
	keyboardshown = true;
}

void Kore::System::hideKeyboard() {
	keyboardshown = false;
}

bool Kore::System::showsKeyboard() {
	return keyboardshown;
}

void Kore::System::setTitle(const char* title) {
	SetWindowTextA(hwnd, title);
}

void Kore::System::showWindow() {
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);
}

double Kore::System::frequency() {
	ticks rate;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&rate));
	return (double)rate;
}

Kore::System::ticks Kore::System::timestamp() {
	ticks stamp;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&stamp));
	return stamp;
}

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR lpCmdLine, int /*nCmdShow*/) {
	initKeyTranslation();
	for (int i = 0; i < 256; ++i) keyPressed[i] = false;

	int argc = 1; 
	for (unsigned i = 0; i < strlen(lpCmdLine); ++i) { 
		while (lpCmdLine[i] == ' ' && i < strlen(lpCmdLine)) ++i;
		if (lpCmdLine[i] == '\"') {
			++i;
			while (lpCmdLine[i] != '\"' && i < strlen(lpCmdLine)) ++i;
			++argc;
		} 
		else {
			while (lpCmdLine[i] != ' ' && i < strlen(lpCmdLine)) ++i;
			++argc;
		}
	}

	char** argv = (char**)malloc(sizeof(char*) * (argc + 1)); 

	argv[0] = (char*)malloc(1024);
	GetModuleFileNameA(0, argv[0], 1024);

	for (int i = 1; i < argc; ++i) argv[i] = (char*)malloc(strlen(lpCmdLine) + 10);
	argv[argc] = 0;

	argc = 1;
	int pos = 0;
	for (unsigned i = 0; i < strlen(lpCmdLine); ++i) {
		while (lpCmdLine[i] == ' ' && i < strlen(lpCmdLine)) ++i;
		if (lpCmdLine[i] == '\"') {
			++i;
			while (lpCmdLine[i] != '\"' && i < strlen(lpCmdLine)) {
				argv[argc][pos] = lpCmdLine[i];
				++i;
				++pos;
			}
			argv[argc][pos] = '\0';
			++argc;
			pos = 0;
		}
		else {
			while (lpCmdLine[i] != ' ' && i < strlen(lpCmdLine)) {
				argv[argc][pos] = lpCmdLine[i];
				++i;
				++pos;
			}
			argv[argc][pos] = '\0';
			++argc;
			pos = 0;
		}
	}
	argv[argc] = 0; 

	int ret = 0;
#ifndef _DEBUG
	try {
#endif
		for (int i = 0; i < 256; ++i) keyPressed[i] = false;
		ret = kore(argc, argv);
#ifndef _DEBUG
	}
	catch (std::exception& ex) {
		ret = 1;
		MessageBoxA(0, ex.what(), "Exception", MB_OK);
	}
	catch (...) {
		ret = 1;
		MessageBox(0, L"Unknown Exception", L"Exception", MB_OK);
	}
#endif
	
	for(int i = 0; i < argc; ++i) free(argv[i]);
	free(argv); 

	return ret;
}
