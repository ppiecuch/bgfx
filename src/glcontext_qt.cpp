/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "bgfx_p.h"

#if BGFX_USE_QT || defined QT_OPENGL_LIB
#	include "renderer_gl.h"

#include <QtOpenGL>
#include <QApplication>
#include <assert.h>

class QSurface;

namespace bgfx { namespace gl
{

#	define GL_IMPORT(_optional, _proto, _func, _import) _proto _func = NULL
#	include "glimports.h"
#       undef GL_IMPORT

	struct SwapChainGL
	{
	  SwapChainGL(QSurface *surf) : m_surf(surf), m_context(0) { }
		~SwapChainGL() { delete m_context; }
		void makeCurrent() {
		  if (m_context == 0) init();
		  m_context->makeCurrent(m_surf);
		}
		void swapBuffers() {
		  if (m_context == 0) init();
		  m_context->swapBuffers(m_surf);
		}
		bool isValid() { return m_context!=0; }
		void init() {
		  m_context = new QOpenGLContext;
		  m_context->create();
		}
		QOpenGLContext *m_context;
		QSurface *m_surf;
	};

	void GlContext::create(uint32_t _width, uint32_t _height)
	{
		BX_UNUSED(_width, _height);

#if BGFX_CONFIG_RENDERER_OPENGL >= 31
#else
#endif // BGFX_CONFIG_RENDERER_OPENGL >= 31

		if (QThread::currentThread() == qApp->thread()) 
		{
		  // single thread rendering
		}
		if (QOpenGLContext::currentContext()) m_ctx->setShareContext(QOpenGLContext::currentContext());
		m_ctx->create();

		m_ctx->makeCurrent(g_bgfxQtWindow);		
		import();
	}

	void GlContext::destroy()
	{
		if (m_current) { delete m_current; m_current = 0; }
	}

	bool GlContext::isValid() const
	{
		return m_current && m_current->isValid();
	}
	void GlContext::resize(uint32_t /*_width*/, uint32_t /*_height*/, bool _vsync)
	{
	}

	bool GlContext::isSwapChainSupported()
	{
		return true;
	}

	SwapChainGL* GlContext::createSwapChain(void* _nwh)
	{
		return BX_NEW(g_allocator, SwapChainGL)( (QSurface*)_nwh );
	}

	void GlContext::destroySwapChain(SwapChainGL* _swapChain)
	{
		BX_DELETE(g_allocator, _swapChain);
	}

	void GlContext::swap(SwapChainGL* _swapChain)
	{
		makeCurrent(_swapChain);

		if (NULL == _swapChain)
		{
			if (g_bgfxQtWindow)
			  g_bgfxQtWindow->metaObject()->invokeMethod(g_bgfxQtWindow, "swapBuffers"); // swap a main window
		}
		else
		{
			_swapChain->swapBuffers();
		}
	}

	void GlContext::makeCurrent(SwapChainGL* _swapChain)
	{
		if (m_current != _swapChain)
		{
			m_current = _swapChain;

			if (NULL == _swapChain)
			{
				if (g_bgfxQtWindow)
				  g_bgfxQtWindow->metaObject()->invokeMethod(g_bgfxQtWindow, "makeCurrent"); // make current a main window
			}
			else
			{
				_swapChain->makeCurrent();
			}
		}
	}

	void GlContext::import()
	{
#	       define GL_IMPORT(_optional, _proto, _func, _import) if (0 == _func) _func = (_proto)QOpenGLContext::currentContext()->getProcAddress(#_func);
#	       include "glimports.h"
#	       undef GL_IMPORT
	}

} /* namespace gl */ } // namespace bgfx

#endif // BGFX_USE_QT || defined QT_OPENGL_LIB
