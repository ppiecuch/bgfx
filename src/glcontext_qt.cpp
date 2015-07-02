/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "bgfx_p.h"

#if BGFX_USE_QT || defined QT_GUI_LIB
#	include "renderer_gl.h"

#include <qopengl.h>
#include <qopenglcontext.h>
#include <qwindow.h>
#include <qthread.h>
#include <qapplication.h>
#include <assert.h>

class QSurface;

#define QWIN ((QWindow*)g_platformData.nwh)
#define QCTX ((QOpenGLContext*)g_platformData.context)

namespace bgfx { namespace gl
{

#	define GL_IMPORT(_optional, _proto, _func, _import) _proto _func = NULL
#	include "glimports.h"
#       undef GL_IMPORT

	struct SwapChainGL
	{
	  SwapChainGL(QSurface *surf, QOpenGLContext *ctx) : m_surf(surf), m_context(ctx) { }
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
		QSurface *m_surf;
		QOpenGLContext *m_context;
	};

	void GlContext::create(uint32_t _width, uint32_t _height)
	{
		BX_UNUSED(_width, _height);
		
		Q_ASSERT(NULL != QCTX);
		Q_ASSERT(NULL != QWIN);
   
    QWIN->setProperty("bgfxReady", true);

    // make sure context is in our thread when we are creating and
    // initilizing object:
    struct raii {
      raii()  { qtReqCtxForThread(QThread::currentThread()); }
      ~raii() { qtReleaseCtx(); }
    } _res;
		Q_ASSERT(QCTX->thread()==QThread::currentThread());
    
    // create default swapchain:
		m_current =  BX_NEW(g_allocator, SwapChainGL)( QWIN, QCTX );

#if BGFX_CONFIG_RENDERER_OPENGL >= 31
#else
#endif // BGFX_CONFIG_RENDERER_OPENGL >= 31

		m_current->makeCurrent();		
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
	void GlContext::resize(uint32_t /*_width*/, uint32_t /*_height*/, bool /*_vsync*/)
	{
	}

	bool GlContext::isSwapChainSupported()
	{
		return true;
	}

	SwapChainGL* GlContext::createSwapChain(void* _nwh)
	{
		return BX_NEW(g_allocator, SwapChainGL)( (QSurface*)_nwh, QCTX );
	}

	void GlContext::destroySwapChain(SwapChainGL* _swapChain)
	{
		BX_DELETE(g_allocator, _swapChain);
	}

	void GlContext::swap(SwapChainGL* _swapChain)
	{
		makeCurrent(_swapChain);

		if (NULL == _swapChain) {
			m_current->makeCurrent();		
		} else {
			_swapChain->swapBuffers();
		}
	}

	void GlContext::makeCurrent(SwapChainGL* _swapChain, QThread *_moveToThread)
	{
		if (m_current != _swapChain)
		{
			m_current = _swapChain;

			if (NULL == _swapChain) {
				m_current->makeCurrent();		
			} else {
				_swapChain->makeCurrent();
			}

      if (_moveToThread)
        m_current->m_context->moveToThread(_moveToThread);
		}
	}

	void GlContext::import()
	{
#	       define GL_IMPORT(_optional, _proto, _func, _import) if (0 == _func) _func = (_proto)QOpenGLContext::currentContext()->getProcAddress(#_func);
#	       include "glimports.h"
#	       undef GL_IMPORT
	}

} /* namespace gl */ } // namespace bgfx

#endif // BGFX_USE_QT || defined QT_GUI_LIB
