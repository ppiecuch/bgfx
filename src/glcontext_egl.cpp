/*
 * Copyright 2011-2014 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "bgfx_p.h"

#if (BGFX_CONFIG_RENDERER_OPENGLES|BGFX_CONFIG_RENDERER_OPENGL)
#	include "renderer_gl.h"

#	if BGFX_USE_EGL

namespace bgfx
{
#if BGFX_USE_GL_DYNAMIC_LIB

	typedef void (*EGLPROC)(void);

	typedef EGLPROC (EGLAPIENTRY* PFNEGLGETPROCADDRESSPROC)(const char *procname);
	typedef EGLBoolean (EGLAPIENTRY* PFNEGLSWAPINTERVALPROC)(EGLDisplay dpy, EGLint interval);
	typedef EGLBoolean (EGLAPIENTRY* PFNEGLMAKECURRENTPROC)(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);
	typedef EGLContext (EGLAPIENTRY* PFNEGLCREATECONTEXTPROC)(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list);
	typedef EGLSurface (EGLAPIENTRY* PFNEGLCREATEWINDOWSURFACEPROC)(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list);
	typedef EGLBoolean (EGLAPIENTRY* PFNEGLCHOOSECONFIGPROC)(EGLDisplay dpy, const EGLint *attrib_list,	EGLConfig *configs, EGLint config_size,	EGLint *num_config);
	typedef EGLBoolean (EGLAPIENTRY* PFNEGLINITIALIZEPROC)(EGLDisplay dpy, EGLint *major, EGLint *minor);
	typedef EGLDisplay (EGLAPIENTRY* PFNEGLGETDISPLAYPROC)(EGLNativeDisplayType display_id);
	typedef EGLBoolean (EGLAPIENTRY* PFNEGLTERMINATEPROC)(EGLDisplay dpy);
	typedef EGLBoolean (EGLAPIENTRY* PFNEGLDESTROYSURFACEPROC)(EGLDisplay dpy, EGLSurface surface);
	typedef EGLBoolean (EGLAPIENTRY* PFNEGLDESTROYCONTEXTPROC)(EGLDisplay dpy, EGLContext ctx);
	typedef EGLBoolean (EGLAPIENTRY* PFNEGLSWAPBUFFERSPROC)(EGLDisplay dpy, EGLSurface surface);

#define EGL_IMPORT \
			EGL_IMPORT_FUNC(PFNEGLGETPROCADDRESSPROC,      eglGetProcAddress); \
			EGL_IMPORT_FUNC(PFNEGLSWAPINTERVALPROC,        eglSwapInterval); \
			EGL_IMPORT_FUNC(PFNEGLMAKECURRENTPROC,         eglMakeCurrent); \
			EGL_IMPORT_FUNC(PFNEGLCREATECONTEXTPROC,       eglCreateContext); \
			EGL_IMPORT_FUNC(PFNEGLCREATEWINDOWSURFACEPROC, eglCreateWindowSurface); \
			EGL_IMPORT_FUNC(PFNEGLCHOOSECONFIGPROC,        eglChooseConfig); \
			EGL_IMPORT_FUNC(PFNEGLINITIALIZEPROC,          eglInitialize); \
			EGL_IMPORT_FUNC(PFNEGLGETDISPLAYPROC,          eglGetDisplay); \
			EGL_IMPORT_FUNC(PFNEGLTERMINATEPROC,           eglTerminate); \
			EGL_IMPORT_FUNC(PFNEGLDESTROYSURFACEPROC,      eglDestroySurface); \
			EGL_IMPORT_FUNC(PFNEGLDESTROYCONTEXTPROC,      eglDestroyContext); \
			EGL_IMPORT_FUNC(PFNEGLSWAPBUFFERSPROC,         eglSwapBuffers);

#define EGL_IMPORT_FUNC(_proto, _func) _proto _func
EGL_IMPORT
#undef EGL_IMPORT_FUNC

	void* eglOpen()
	{
		void* handle = bx::dlopen("libEGL.dll");
		BGFX_FATAL(NULL != handle, Fatal::UnableToInitialize, "Failed to load libEGL dynamic library.");

#define EGL_IMPORT_FUNC(_proto, _func) \
			_func = (_proto)bx::dlsym(handle, #_func); \
			BX_TRACE("%p " #_func, _func); \
			BGFX_FATAL(NULL != _func, Fatal::UnableToInitialize, "Failed get " #_func ".")
EGL_IMPORT
#undef EGL_IMPORT_FUNC

		return handle;
	}

	void eglClose(void* _handle)
	{
		bx::dlclose(_handle);

#define EGL_IMPORT_FUNC(_proto, _func) _func = NULL
EGL_IMPORT
#undef EGL_IMPORT_FUNC
	}

#else

	void* eglOpen()
	{
		return NULL;
	}

	void eglClose(void* /*_handle*/)
	{
	}
#endif // BGFX_USE_GL_DYNAMIC_LIB

#	define GL_IMPORT(_optional, _proto, _func, _import) _proto _func = NULL
#	include "glimports.h"

	void GlContext::create(uint32_t _width, uint32_t _height)
	{
		m_eglLibrary = eglOpen();

		BX_UNUSED(_width, _height);
		EGLNativeDisplayType ndt = EGL_DEFAULT_DISPLAY;
		EGLNativeWindowType nwt = (EGLNativeWindowType)NULL;
#	if BX_PLATFORM_WINDOWS
		ndt = GetDC(g_bgfxHwnd);
		nwt = g_bgfxHwnd;
#	endif // BX_PLATFORM_
		m_display = eglGetDisplay(ndt);
		BGFX_FATAL(m_display != EGL_NO_DISPLAY, Fatal::UnableToInitialize, "Failed to create display %p", m_display);

		EGLint major = 0;
		EGLint minor = 0;
		EGLBoolean success = eglInitialize(m_display, &major, &minor);
		BGFX_FATAL(success && major >= 1 && minor >= 3, Fatal::UnableToInitialize, "Failed to initialize %d.%d", major, minor);

		EGLint attrs[] =
		{
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,

#	if BX_PLATFORM_ANDROID
			EGL_DEPTH_SIZE, 16,
#	else
			EGL_DEPTH_SIZE, 24,
#	endif // BX_PLATFORM_
			EGL_STENCIL_SIZE, 8,

			EGL_NONE
		};

		EGLint numConfig = 0;
		EGLConfig config;
		success = eglChooseConfig(m_display, attrs, &config, 1, &numConfig);
		BGFX_FATAL(success, Fatal::UnableToInitialize, "eglChooseConfig");

#	if BX_PLATFORM_ANDROID
		EGLint format;
		eglGetConfigAttrib(m_display, config, EGL_NATIVE_VISUAL_ID, &format);
		ANativeWindow_setBuffersGeometry(g_bgfxAndroidWindow, _width, _height, format);
		nwt = g_bgfxAndroidWindow;
#	endif // BX_PLATFORM_ANDROID

		m_surface = eglCreateWindowSurface(m_display, config, nwt, NULL);
		BGFX_FATAL(m_surface != EGL_NO_SURFACE, Fatal::UnableToInitialize, "Failed to create surface.");

		EGLint contextAttrs[] =
		{
#	if BGFX_CONFIG_RENDERER_OPENGLES >= 30
			EGL_CONTEXT_CLIENT_VERSION, 3,
#	elif BGFX_CONFIG_RENDERER_OPENGLES
			EGL_CONTEXT_CLIENT_VERSION, 2,
#	endif // BGFX_CONFIG_RENDERER_

			EGL_NONE
		};

		m_context = eglCreateContext(m_display, config, EGL_NO_CONTEXT, contextAttrs);
		BGFX_FATAL(m_context != EGL_NO_CONTEXT, Fatal::UnableToInitialize, "Failed to create context.");

		success = eglMakeCurrent(m_display, m_surface, m_surface, m_context);
		BGFX_FATAL(success, Fatal::UnableToInitialize, "Failed to set context.");

		eglSwapInterval(m_display, 0);

#	if BX_PLATFORM_EMSCRIPTEN
		emscripten_set_canvas_size(_width, _height);
#	endif // BX_PLATFORM_EMSCRIPTEN

		import();
	}

	void GlContext::destroy()
	{
		eglMakeCurrent(EGL_NO_DISPLAY, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroyContext(m_display, m_context);
		eglDestroySurface(m_display, m_surface);
		eglTerminate(m_display);
		m_context = NULL;

		eglClose(m_eglLibrary);
	}

	void GlContext::resize(uint32_t /*_width*/, uint32_t /*_height*/, bool _vsync)
	{
		eglSwapInterval(m_display, _vsync ? 1 : 0);
	}

	void GlContext::swap()
	{
		eglMakeCurrent(m_display, m_surface, m_surface, m_context);
		eglSwapBuffers(m_display, m_surface);
	}

	void GlContext::import()
	{
		BX_TRACE("Import:");
#	if BX_PLATFORM_WINDOWS
		void* glesv2 = bx::dlopen("libGLESv2.dll");
#		define GL_EXTENSION(_optional, _proto, _func, _import) \
					{ \
						if (NULL == _func) \
						{ \
							_func = (_proto)bx::dlsym(glesv2, #_import); \
							BX_TRACE("\t%p " #_func " (" #_import ")", _func); \
							BGFX_FATAL(_optional || NULL != _func, Fatal::UnableToInitialize, "Failed to create OpenGLES context. eglGetProcAddress(\"%s\")", #_import); \
						} \
					}
#	else
#		define GL_EXTENSION(_optional, _proto, _func, _import) \
					{ \
						if (NULL == _func) \
						{ \
							_func = (_proto)eglGetProcAddress(#_import); \
							BX_TRACE("\t%p " #_func " (" #_import ")", _func); \
							BGFX_FATAL(_optional || NULL != _func, Fatal::UnableToInitialize, "Failed to create OpenGLES context. eglGetProcAddress(\"%s\")", #_import); \
						} \
					}
#	endif // BX_PLATFORM_
#	include "glimports.h"
	}

} // namespace bgfx

#	endif // BGFX_USE_EGL
#endif // (BGFX_CONFIG_RENDERER_OPENGLES|BGFX_CONFIG_RENDERER_OPENGL)
