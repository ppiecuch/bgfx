/*
 * Copyright 2011-2014 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "common.h"
#include "bgfx_utils.h"

int _main_(int /*_argc*/, char** /*_argv*/)
{
	uint32_t width = 1280;
	uint32_t height = 720;
	uint32_t debug = BGFX_DEBUG_TEXT;
	uint32_t reset = BGFX_RESET_VSYNC;

	bgfx::init();
	bgfx::reset(width, height, reset);

	// Enable debug text.
	bgfx::setDebug(debug);

	// Set view 0 clear state.
	bgfx::setViewClear(0
		, BGFX_CLEAR_COLOR_BIT|BGFX_CLEAR_DEPTH_BIT
		, 0x303030ff
		, 1.0f
		, 0
		);

	bgfx::UniformHandle u_time = bgfx::createUniform("u_time", bgfx::UniformType::Uniform1f);

	// Create program from shaders.
	bgfx::ProgramHandle program = loadProgram("vs_mesh", "fs_mesh");

	Mesh* mesh = meshLoad("meshes/bunny.bin");

	int64_t timeOffset = bx::getHPCounter();

	while (!entry::processEvents(width, height, debug, reset) )
	{
		// Set view 0 default viewport.
		bgfx::setViewRect(0, 0, 0, width, height);

		// This dummy draw call is here to make sure that view 0 is cleared
		// if no other draw calls are submitted to view 0.
		bgfx::submit(0);

		int64_t now = bx::getHPCounter();
		static int64_t last = now;
		const int64_t frameTime = now - last;
		last = now;
		const double freq = double(bx::getHPFrequency() );
		const double toMs = 1000.0/freq;
		float time = (float)( (bx::getHPCounter()-timeOffset)/double(bx::getHPFrequency() ) );
		bgfx::setUniform(u_time, &time);

		// Use debug font to print information about this example.
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(0, 1, 0x4f, "bgfx/examples/04-mesh");
		bgfx::dbgTextPrintf(0, 2, 0x6f, "Description: Loading meshes.");
		bgfx::dbgTextPrintf(0, 3, 0x0f, "Frame: % 7.3f[ms]", double(frameTime)*toMs);

		float at[3] = { 0.0f, 1.0f, 0.0f };
		float eye[3] = { 0.0f, 1.0f, -2.5f };

		float view[16];
		float proj[16];
		bx::mtxLookAt(view, eye, at);
		bx::mtxProj(proj, 60.0f, float(width)/float(height), 0.1f, 100.0f);

		// Set view and projection matrix for view 0.
		bgfx::setViewTransform(0, view, proj);

		float mtx[16];
		bx::mtxRotateXY(mtx
			, 0.0f
			, time*0.37f
			); 

		meshSubmit(mesh, 0, program, mtx);

		// Advance to next frame. Rendering thread will be kicked to 
		// process submitted rendering primitives.
		bgfx::frame();
	}

	meshUnload(mesh);

	// Cleanup.
	bgfx::destroyProgram(program);

	bgfx::destroyUniform(u_time);

	// Shutdown bgfx.
	bgfx::shutdown();

	return 0;
}
