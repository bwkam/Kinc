#include "commandlist_functions.h"

#include "d3d12unit.h"

#include <kope/graphics5/commandlist.h>
#include <kope/graphics5/device.h>

void kope_d3d12_command_list_begin_render_pass(kope_g5_command_list *list, const kope_g5_render_pass_parameters *parameters) {
	parameters->color_attachments[0].texture->d3d12.resource;

	D3D12_CPU_DESCRIPTOR_HANDLE rtv = list->d3d12.device->all_rtvs->GetCPUDescriptorHandleForHeapStart();
	rtv.ptr += parameters->color_attachments[0].texture->d3d12.rtv_index * list->d3d12.device->rtv_increment;

	FLOAT color[4];
	memcpy(color, &parameters->color_attachments[0].clear_value, sizeof(color));
	list->d3d12.list->ClearRenderTargetView(rtv, color, 0, NULL);
}
