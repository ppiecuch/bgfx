/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * Copyright 2014-2015 Pawel Piecuch. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "entry_p.h"
#include "osx_utils.h"

#ifdef QT_GUI_LIB

#include <bgfx/bgfxplatform.h>
#include "dbg.h"
 	
// This is just trivial implementation of Qt integration.
// It's here just for testing purpose.

#include <Qt>
#include <QDebug>
#include <QTime>
#include <QKeyEvent>
#include <QFileInfo>
#include <QDirIterator>
#include <QWindow>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QApplication>
#include <QGuiApplication>


namespace entry
{
	QtMsgType level = QtDebugMsg;

	void fprintfMsg(const char *name,
			const  QMessageLogContext &context,
			const QString &msg) {
	  QByteArray localMsg=msg.toLocal8Bit();
	  fprintf(stderr, "%s: %s(%s:%u, %s)\n",
		  name, localMsg.constData(),
		  context.file,
		  context.line,
		  context.function);
	}

	void _MessageOutput(QtMsgType type,
			     const QMessageLogContext &context,
			     const QString &msg){

	  if(type >= level) {
	    switch (type) {
	    case QtDebugMsg: fprintfMsg("Debug", context, msg); break;
	    case QtWarningMsg: fprintfMsg("Warning", context, msg); break;
	    case QtCriticalMsg: fprintfMsg("Critical", context, msg); break;
	    case QtFatalMsg: fprintfMsg("Fatal", context, msg); exit(EXIT_FAILURE);
	    }
	  }
	}
  

    // --- bgfx Context

    class OpenGLWindow;

	struct Context
	{
		Context() : m_window(0), m_scrollPos(0) { }
		~Context();

        int run(int _argc, char** _argv);
		
        WindowHandle findHandle(OpenGLWindow *window) { return { UINT16_MAX }; }

        void processEvents() { m_app->processEvents(); }

		EventQueue m_eventQueue;

		static QGuiApplication *m_app;
		OpenGLWindow *m_window;
		double m_scrollPos;
	};

    QGuiApplication *Context::m_app = 0;

	Context s_ctx;

    // -- Qt

	static MouseButton::Enum translateMouseButton(Qt::MouseButtons buttons)
	{
		if (buttons & Qt::LeftButton) {
			return MouseButton::Left;
		} else if (buttons & Qt::RightButton) {
			return MouseButton::Right;
		}

		return MouseButton::Middle;
	}

	static MouseButton::Enum translateMouseButton(Qt::MouseButton button)
	{
		if (button == Qt::LeftButton) {
			return MouseButton::Left;
		} else if (button == Qt::RightButton) {
			return MouseButton::Right;
		}

		return MouseButton::Middle;
	}

	class OpenGLWindow : public QWindow, public QOpenGLFunctions
	{
	  Q_OBJECT
	  Q_PROPERTY(bool alwaysRefresh READ alwaysRefresh WRITE setAlwaysRefresh)

	private:
	  bool _isGLInitialized;
	  bool _isWindowInitialized;
	public:
	  void setWindowInitialized() { _isWindowInitialized = true; }

	public:
	  explicit OpenGLWindow(QScreen *screen = 0) : QWindow(screen),
              _isGLInitialized(0), _isWindowInitialized(0),
						  _context(0), 
						  _alwaysRefresh(false),
						  _pendingRefresh(false)
	  { initializeContext(); }

	  explicit OpenGLWindow(QWindow *parent) : QWindow(parent),
              _isGLInitialized(0), _isWindowInitialized(0),
		          _context(0),
		          _alwaysRefresh(false),
		          _pendingRefresh(false)
	  { initializeContext(); }

	  virtual ~OpenGLWindow() { if (_context) delete _context; }

	  bool alwaysRefresh() const { return _alwaysRefresh; }
	  void setAlwaysRefresh(bool alwaysRefresh) {
	    _alwaysRefresh = alwaysRefresh;
	    if (alwaysRefresh) requestRefresh();
	  }
    
	signals:
	  void aboutToResize();
	  void resized();

	public slots:
	  QOpenGLContext *context() const { Q_ASSERT(_context!=NULL); return _context; }
	  void makeCurrent() { _context->makeCurrent(this); }
	  void swapBuffers() { _context->swapBuffers(this); }

	protected:
	  bool event(QEvent *event) {
	    switch (event->type()) {
	    case QEvent::UpdateRequest:
	      _pendingRefresh = false;
	      render();
	      return true;        
	    default:
	      return QWindow::event(event);
	    }
	  }
	  void exposeEvent(QExposeEvent *event) { requestRefresh(); }
	  void resizeEvent(QResizeEvent *event) { requestRefresh(); }
	  void keyPressEvent(QKeyEvent *event) {
	    switch (event->key()) {
	    case Qt::Key_Q :
	      if (!(QApplication::keyboardModifiers() & Qt::ControlModifier)) break;
	    case Qt::Key_Escape : 
		    QApplication::exit(EXIT_SUCCESS); break;
	    }
	  }
	  void mousePressEvent(QMouseEvent *event) {
		WindowHandle handle = s_ctx.findHandle(this);
		const bool down = true;
		s_ctx.m_eventQueue.postMouseEvent(handle
			, (int32_t) event->x()
			, (int32_t) event->y()
			, (int32_t) s_ctx.m_scrollPos
			, translateMouseButton(event->buttons())
			, down
			);
      }
	  void mouseReleaseEvent(QMouseEvent *event) {
		WindowHandle handle = s_ctx.findHandle(this);
		const bool down = false;
		s_ctx.m_eventQueue.postMouseEvent(handle
			, (int32_t) event->x()
			, (int32_t) event->y()
			, (int32_t) s_ctx.m_scrollPos
			, translateMouseButton(event->button())
			, down
			);
      }
	  void mouseMoveEvent(QMouseEvent *event) {
		WindowHandle handle = s_ctx.findHandle(this);
		s_ctx.m_eventQueue.postMouseEvent(handle
			, (int32_t) event->x()
			, (int32_t) event->y()
			, (int32_t) s_ctx.m_scrollPos
			);
      }
	  void mouseWheelEvent(QWheelEvent *event) {
		WindowHandle handle = s_ctx.findHandle(this);
        const float step = event->delta() / 240.0;
		s_ctx.m_scrollPos += step;
		s_ctx.m_eventQueue.postMouseEvent(handle
			, (int32_t) event->x()
			, (int32_t) event->y()
			, (int32_t) s_ctx.m_scrollPos
			);
      }

	public slots:
	  void render() {
      
      	if (!_isGLInitialized) {
       		_context->makeCurrent(this);
        	initializeOpenGLFunctions();
        	initializeWindow();
        	_isGLInitialized = true;
            // update title with version info
            setTitle(QString("%1 (Qt %2/%3)").arg(title()).arg(QT_VERSION_STR).arg((char*)glGetString(GL_RENDERER)));
    	}
	  }
	  void requestRefresh() {
	    if (!_pendingRefresh) {
	      _pendingRefresh = true;
	      QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
	    }
	  }

  public:
		template<QObject *target>
		void operator()(const char *sig) {
			QMetaObject::invokeMethod(target, sig);
			return;
			// ((*((X*)NULL)).*((Y)NULL))();
		}

		template<QObject *target, typename T1>
		void operator()(const char *sig, const char *a1, const T1 & t1) {
			QMetaObject::invokeMethod(target, sig, QArgument<T1>(a1, t1));
			return;
			// ((*((X*)NULL)).*((Y)NULL))(t1);
		}

	private:
    void initializeContext() {
	    setSurfaceType(QWindow::OpenGLSurface); // from default format    
	    create();
    
	    _context = new QOpenGLContext(this); // from default context
	    _context->create();
	    _context->makeCurrent(this);
	  }
	  void initializeWindow() {      
	    _time.start();
	  }

	  QOpenGLContext *_context;
    
	  bool _alwaysRefresh;
	  bool _pendingRefresh;
	  QTime _time;
	};

	static void _finishCtx();

    Context::~Context()
    {
        m_window->close();
        m_app->exit();
    }
    int Context::run(int _argc, char** _argv)
    {
        if (0 == m_app)
            m_app = new QGuiApplication(_argc, _argv);
        QObject::connect( m_app, &QGuiApplication::lastWindowClosed, &_finishCtx );

        m_window = new OpenGLWindow;
        m_window->resize(ENTRY_DEFAULT_WIDTH, ENTRY_DEFAULT_HEIGHT);
        m_window->setTitle(QFileInfo(_argv[0]).fileName());
        
        bgfx::PlatformData pd;
# if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
        pd.ndt          = NULL;
        pd.nwh          = (void*)(uintptr_t)wmi.info.x11.window;
# elif BX_PLATFORM_OSX
        pd.ndt          = NULL;
        pd.nwh          = get_nswindow_from_nsview(reinterpret_cast<void*>(m_window->winId()));
# elif BX_PLATFORM_WINDOWS
        pd.ndt          = NULL;
        pd.nwh          = m_window->winId();
# elif BX_PLATFORM_STEAMLINK
        pd.ndt          = NULL;
        pd.nwh          = m_window->winId();;
# endif // BX_PLATFORM_
        pd.context      = NULL;
        pd.backBuffer   = NULL;
        pd.backBufferDS = NULL;
        bgfx::setPlatformData(pd);

        m_window->show();
        m_window->setWindowInitialized();
        
        return entry::main(_argc, _argv);
    }	  

	static void _finishCtx() { s_ctx.m_eventQueue.postExitEvent(); }

	const Event* poll()
	{
		s_ctx.processEvents(); // process Qt events
		return s_ctx.m_eventQueue.poll();
	}

	const Event* poll(WindowHandle _handle) { return s_ctx.m_eventQueue.poll(_handle); }

	void release(const Event* _event) { s_ctx.m_eventQueue.release(_event); }

	WindowHandle createWindow(int32_t _x, int32_t _y, uint32_t _width, uint32_t _height, uint32_t _flags, const char* _title)
	{
		BX_UNUSED(_x, _y, _width, _height, _flags, _title);
		WindowHandle handle = { UINT16_MAX };
		return handle;
	}

	void destroyWindow(WindowHandle _handle)
	{
		BX_UNUSED(_handle);
	}

	void setWindowPos(WindowHandle _handle, int32_t _x, int32_t _y)
	{
		BX_UNUSED(_handle, _x, _y);
	}

	void setWindowSize(WindowHandle _handle, uint32_t _width, uint32_t _height)
	{
		BX_UNUSED(_handle, _width, _height);
	}

	void setWindowTitle(WindowHandle _handle, const char* _title)
	{
		BX_UNUSED(_handle, _title);
	}

	void toggleWindowFrame(WindowHandle _handle)
	{
		BX_UNUSED(_handle);
	}

	void toggleFullscreen(WindowHandle _handle)
	{
		BX_UNUSED(_handle);
	}

	void setMouseLock(WindowHandle _handle, bool _lock)
	{
		BX_UNUSED(_handle, _lock);
	}
}

int main(int _argc, char** _argv)
{
	using namespace entry;	
	return s_ctx.run(_argc, _argv);
}

#include "entry_qt.moc"

#endif // QT_GUI_LIB
