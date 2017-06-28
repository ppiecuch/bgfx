/*
 * Copyright 2011-2017 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include "imgui/imgui.h"
#include "entry/entry.h"

bool showExampleDialog(entry::AppI* _app)
{
	bool restart = false;

	char temp[1024];
	bx::snprintf(temp, BX_COUNTOF(temp), "Example: %s", _app->getName() );

	ImGui::SetNextWindowPos(
		  ImVec2(10.0f, 50.0f)
		, ImGuiSetCond_FirstUseEver
		);
	ImGui::Begin(temp
		, NULL
		, ImVec2(256.0f, 200.0f)
		, ImGuiWindowFlags_AlwaysAutoResize
		);

	ImGui::TextWrapped("%s", _app->getDescription() );
	ImGui::Separator();

	{
		uint32_t num = entry::getNumApps();
		const char** items = (const char**)alloca(num*sizeof(void*) );

		uint32_t ii = 0;
		int32_t current = 0;
		for (entry::AppI* app = entry::getFirstApp(); NULL != app; app = app->getNext() )
		{
			if (app == _app)
			{
				current = ii;
			}

			items[ii++] = app->getName();
		}

		if (1 < num
		&&  ImGui::Combo("Example", &current, items, num) )
		{
			entry::setRestartArgs(items[current]);
			restart = true;
		}
	}

#if 0
	{
		bgfx::RendererType::Enum supportedRenderers[bgfx::RendererType::Count];
		uint8_t num = bgfx::getSupportedRenderers(BX_COUNTOF(supportedRenderers), supportedRenderers);

		const bgfx::Caps* caps = bgfx::getCaps();

		const char* items[bgfx::RendererType::Count];

		int32_t current = 0;
		for (uint8_t ii = 0; ii < num; ++ii)
		{
			items[ii] = bgfx::getRendererName(supportedRenderers[ii]);
			if (supportedRenderers[ii] == caps->rendererType)
			{
				current = ii;
			}
		}

		if (ImGui::Combo("Renderer", &current, items, num) )
		{
			restart = true;
		}
	}
#endif // 0

	const bgfx::Stats* stats = bgfx::getStats();
	ImGui::Text("CPU %0.3f"
		, double(stats->cpuTimeEnd-stats->cpuTimeBegin)/stats->cpuTimerFreq*1000.0
		);

	ImGui::Text("GPU %0.3f (latency: %d)"
		, double(stats->gpuTimeEnd-stats->gpuTimeBegin)/stats->gpuTimerFreq*1000.0
		, stats->maxGpuLatency
		);

	ImGui::End();

	return restart;
}
