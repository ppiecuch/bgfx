/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "bgfx_p.h"

#if BGFX_USE_QT || defined QT_OPENGL_LIB
#	include "renderer_gl.h"

#	include <QTime>
#	include <QWindow>
#	include <QOpenGLContext>

namespace bgfx { namespace gl
{

	class OpenGLSurface : public QWindow
	{
	  Q_OBJECT
	  Q_PROPERTY(bool alwaysRefresh READ alwaysRefresh WRITE setAlwaysRefresh)
    
	public:
	  explicit OpenGLSurface(QScreen *screen = 0);
	  explicit OpenGLSurface(QWindow *parent);
	  virtual ~OpenGLSurface();
    
	  bool alwaysRefresh() const;
	  void setAlwaysRefresh(bool alwaysRefresh);
    
	  QOpenGLContext const *context() { return _context; }

	  void makeCurrent() { _context->makeCurrent(this); }
	  void swapBuffers() { _context->swapBuffers(this); }

	protected:
	  virtual bool event(QEvent *event);
	  virtual void exposeEvent(QExposeEvent *event);
	  virtual void resizeEvent(QResizeEvent *event);

	public slots:
	  void render();
	  void requestRefresh();
    
	private:
	  void initialize();

	  QOpenGLContext *_context;
    
	  bool _alwaysRefresh;
	  bool _pendingRefresh;
	  QTime _time;
	};

  
	struct SwapChainGL
	{
		SwapChainGL()
		{
		}

		~SwapChainGL()
		{
		}

		void makeCurrent()
		{
			m_window.makeCurrent();
		}

		void swapBuffers()
		{
			m_window.swapBuffers();
		}

		OpenGLSurface m_window;
	};

	void GlContext::create(uint32_t _width, uint32_t _height)
	{
		BX_UNUSED(_width, _height);

#if BGFX_CONFIG_RENDERER_OPENGL >= 31
#else
#endif // BGFX_CONFIG_RENDERER_OPENGL >= 31

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glXSwapBuffers( (::Display*)g_bgfxX11Display, (::Window)g_bgfxX11Window);
	}

	void GlContext::destroy()
	{
		glXMakeCurrent( (::Display*)g_bgfxX11Display, 0, 0);
		if (NULL == g_bgfxGLX)
		{
			glXDestroyContext( (::Display*)g_bgfxX11Display, m_context);
			XFree(m_visualInfo);
		}
		m_context    = NULL;
		m_visualInfo = NULL;
	}

	bool GlContext::isValid() const
	{
		return m_current && m_current->isValid();
	}
	void GlContext::resize(uint32_t /*_width*/, uint32_t /*_height*/, bool _vsync)
	{
		int32_t interval = _vsync ? 1 : 0;

		if (NULL != glXSwapIntervalEXT)
		{
			glXSwapIntervalEXT( (::Display*)g_bgfxX11Display, (::Window)g_bgfxX11Window, interval);
		}
		else if (NULL != glXSwapIntervalMESA)
		{
			glXSwapIntervalMESA(interval);
		}
		else if (NULL != glXSwapIntervalSGI)
		{
			glXSwapIntervalSGI(interval);
		}
	}

	bool GlContext::isSwapChainSupported()
	{
		return true;
	}

	SwapChainGL* GlContext::createSwapChain(void* _nwh)
	{
		return BX_NEW(g_allocator, SwapChainGL)( (::Window)_nwh, m_visualInfo, m_context);
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
			}
			else
			{
				_swapChain->makeCurrent();
			}
		}
	}

	void GlContext::import()
	{
#	define GL_EXTENSION(_optional, _proto, _func, _import) \
				{ \
					if (NULL == _func) \
					{ \
						_func = (_proto)glXGetProcAddress( (const GLubyte*)#_import); \
						BX_TRACE("%p " #_func " (" #_import ")", _func); \
						BGFX_FATAL(_optional || NULL != _func, Fatal::UnableToInitialize, "Failed to create OpenGL context. glXGetProcAddress %s", #_import); \
					} \
				}
#	include "glimports.h"
	}

} /* namespace gl */ } // namespace bgfx

#endif // BGFX_USE_QT || defined QT_OPENGL_LIB
