/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * Copyright 2014-2015 Pawel Piecuch. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "entry_p.h"
#include "osx_utils.h"

#ifdef QT_GUI_LIB

#include <bx/mutex.h>
#include <bx/thread.h>
#include <bx/handlealloc.h>
#include <bx/readerwriter.h>
#include <bgfx/platform.h>
#include "dbg.h"
 	
// This is just trivial implementation of Qt integration.
// It's here just for testing purpose.

#include <Qt>
#include <QDebug>
#include <QSharedData>
#include <QKeyEvent>
#include <QFileInfo>
#include <QDirIterator>
#include <QResource>
#include <QWindow>
#include <QThread>
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

	struct Msg : public QSharedData
	{
		Msg()
			: m_x(0)
			, m_y(0)
			, m_width(0)
			, m_height(0)
			, m_flags(0)
		{
		}
        Msg(const Msg &other) 
            : QSharedData(other)
			, m_x(other.m_x)
			, m_y(other.m_y)
			, m_width(other.m_width)
			, m_height(other.m_height)
			, m_flags(other.m_flags)
		{
		}

        int32_t  m_x;
		int32_t  m_y;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_flags;
		QString m_title;
	};

    class OpenGLWindow;

    class Context : public QObject
	{
        Q_OBJECT

    public:
		Context() 
			: m_width(ENTRY_DEFAULT_WIDTH)
			, m_height(ENTRY_DEFAULT_HEIGHT)
			, m_aspectRatio(16.0f/9.0f)
            , m_scroll(0) 
        { 
            connect(this, SIGNAL(signalWindowCreate(WindowHandle, const Msg)), this, SLOT(onWindowCreateEvent(WindowHandle, const Msg)));
            connect(this, SIGNAL(signalWindowDestroy(WindowHandle)), this, SLOT(onWindowDestroyEvent(WindowHandle)));
            connect(this, SIGNAL(signalWindowSetTitle(WindowHandle, const Msg)), this, SLOT(onWindowSetTitleEvent(WindowHandle, const Msg)));
            connect(this, SIGNAL(signalWindowSetPos(WindowHandle, const Msg)), this, SLOT(onWindowSetPosEvent(WindowHandle, const Msg)));
            connect(this, SIGNAL(signalWindowSetSize(WindowHandle, const Msg)), this, SLOT(onWindowSetSizeEvent(WindowHandle, const Msg)));
            connect(this, SIGNAL(signalWindowToggleFrame(WindowHandle)), this, SLOT(onWindowToggleFrameEvent(WindowHandle)));
            connect(this, SIGNAL(signalWindowToggleFullScreen(WindowHandle)), this, SLOT(onWindowToggleFullScreenEvent(WindowHandle)));
            connect(this, SIGNAL(signalWindowMouseLock(WindowHandle, bool)), this, SLOT(onWindowMouseLockEvent(WindowHandle, bool)));
        }
		~Context();

        int run(int _argc, char** _argv);
		
		WindowHandle findHandle(OpenGLWindow* _window);

		void setWindowSize(WindowHandle _handle, uint32_t _width, uint32_t _height, bool _force = false);

		EventQueue m_eventQueue;
		bx::Mutex m_lock;

		bx::HandleAllocT<ENTRY_CONFIG_MAX_WINDOWS> m_windowAlloc;
		OpenGLWindow* m_window[ENTRY_CONFIG_MAX_WINDOWS];
		uint32_t m_flags[ENTRY_CONFIG_MAX_WINDOWS];

		uint32_t m_width;
		uint32_t m_height;
		float m_aspectRatio;

        QPoint m_pos;
		double m_scroll;

    signals:
        void signalWindowCreate(WindowHandle _handle, const Msg _msg);
        void signalWindowDestroy(WindowHandle _handle);
        void signalWindowSetTitle(WindowHandle _handle, const Msg _msg);
        void signalWindowSetPos(WindowHandle _handle, const Msg _msg);
        void signalWindowSetSize(WindowHandle _handle, const Msg _msg);
        void signalWindowToggleFrame(WindowHandle _handle);
        void signalWindowToggleFullScreen(WindowHandle _handle);
        void signalWindowMouseLock(WindowHandle _handle, bool _lock);

    public slots:
        void onWindowCreateEvent(WindowHandle _handle, const Msg _msg);
        void onWindowDestroyEvent(WindowHandle _handle);
        void onWindowSetTitleEvent(WindowHandle _handle, const Msg _msg);
        void onWindowSetPosEvent(WindowHandle _handle, const Msg _msg);
        void onWindowSetSizeEvent(WindowHandle _handle, const Msg _msg);
        void onWindowToggleFrameEvent(WindowHandle _handle);
        void onWindowToggleFullScreenEvent(WindowHandle _handle);
        void onWindowMouseLockEvent(WindowHandle _handle, bool _lock);
	};

	static Context s_ctx;

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

    class OpenGLWindow : public QWindow
	{
	  Q_OBJECT

	public:
	  explicit OpenGLWindow(QWindow *parent = 0) 
      : QWindow(parent)
      , m_shouldClose(false)
	  { 
            setSurfaceType(QSurface::OpenGLSurface);
	  }

	  virtual ~OpenGLWindow() { }

	protected:
	  void exposeEvent(QExposeEvent *event) { }
	  void resizeEvent(QResizeEvent *event) { 
		WindowHandle handle = s_ctx.findHandle(this);
        s_ctx.m_eventQueue.postSizeEvent(handle, event->size().width(), event->size().height());
		// Make sure mouse button state is 'up' after resize.
		s_ctx.m_eventQueue.postMouseEvent(handle, s_ctx.m_pos.x(), s_ctx.m_pos.y(), s_ctx.m_scroll, MouseButton::Left,  false);
		s_ctx.m_eventQueue.postMouseEvent(handle, s_ctx.m_pos.x(), s_ctx.m_pos.y(), s_ctx.m_scroll, MouseButton::Right, false);
      }
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
        s_ctx.m_pos = QPoint(event->x(), event->y());
		s_ctx.m_eventQueue.postMouseEvent(handle
			, (int32_t) event->x()
			, (int32_t) event->y()
			, (int32_t) s_ctx.m_scroll
			, translateMouseButton(event->buttons())
			, down
			);
      }
	  void mouseReleaseEvent(QMouseEvent *event) {
		WindowHandle handle = s_ctx.findHandle(this);
		const bool down = false;
        s_ctx.m_pos = QPoint(event->x(), event->y());
		s_ctx.m_eventQueue.postMouseEvent(handle
			, (int32_t) event->x()
			, (int32_t) event->y()
			, (int32_t) s_ctx.m_scroll
			, translateMouseButton(event->button())
			, down
			);
      }
	  void mouseMoveEvent(QMouseEvent *event) {
		WindowHandle handle = s_ctx.findHandle(this);
        s_ctx.m_pos = QPoint(event->x(), event->y());
		s_ctx.m_eventQueue.postMouseEvent(handle
			, (int32_t) event->x()
			, (int32_t) event->y()
			, (int32_t) s_ctx.m_scroll
			);
      }
#ifndef QT_NO_WHEELEVENT
	  void mouseWheelEvent(QWheelEvent *event) {
		WindowHandle handle = s_ctx.findHandle(this);
        const float step = event->delta() / 240.0;
        s_ctx.m_pos = QPoint(event->x(), event->y());
		s_ctx.m_scroll += step;
		s_ctx.m_eventQueue.postMouseEvent(handle
			, (int32_t) event->x()
			, (int32_t) event->y()
			, (int32_t) s_ctx.m_scroll
			);
      }
#endif
    
    void closeEvent(QCloseEvent *event)
    {
    }

    bool m_shouldClose;

	public slots:
	  void render(QPainter *painter) {
        Q_UNUSED(painter);
	  }
	  void render() {
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
	};

	inline static void* qtNativeWindowHandle(OpenGLWindow* _window)
    {
# if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
        return (void*)(uintptr_t)_window->winId();
# elif BX_PLATFORM_OSX
        return get_nswindow_from_nsview(reinterpret_cast<void*>(_window->winId()));
# elif BX_PLATFORM_WINDOWS
        return (void*)(uintptr_t)_window->winId();
# endif // BX_PLATFORM_
        return NULL;
    }
	inline static bool qtSetWindow(OpenGLWindow* _window)
    {
        if (0 == _window->winId())
            return false;

        bgfx::PlatformData pd;
# if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
        pd.ndt          = NULL;
        pd.nwh          = (void*)(uintptr_t)_window->winId();
# elif BX_PLATFORM_OSX
        pd.ndt          = NULL;
        pd.nwh          = get_nswindow_from_nsview(reinterpret_cast<void*>(_window->winId()));
# elif BX_PLATFORM_WINDOWS
        pd.ndt          = NULL;
        pd.nwh          = (void*)(uintptr_t)_window->winId();
# endif // BX_PLATFORM_
        pd.context      = NULL;
        pd.backBuffer   = NULL;
        pd.backBufferDS = NULL;
        bgfx::setPlatformData(pd);
        
        return true;
    }

    WindowHandle Context::findHandle(OpenGLWindow* _window)
    {
        bx::MutexScope scope(m_lock);
        for (uint32_t ii = 0, num = m_windowAlloc.getNumHandles(); ii < num; ++ii)
        {
            uint16_t idx = m_windowAlloc.getHandleAt(ii);
            if (_window == m_window[idx])
            {
                WindowHandle handle = { idx };
                return handle;
            }
        }

        WindowHandle invalid = { UINT16_MAX };
        return invalid;
    }
    void Context::setWindowSize(WindowHandle _handle, uint32_t _width, uint32_t _height, bool _force)
    {
        if (_width  != m_width
        ||  _height != m_height
        ||  _force)
        {
            m_width  = _width;
            m_height = _height;

            m_window[_handle.idx]->resize(m_width, m_height);
            m_eventQueue.postSizeEvent(_handle, m_width, m_height);
        }
    }

    void Context::onWindowCreateEvent(WindowHandle _handle, const Msg _msg) 
    {
        WindowHandle handle = _handle;
        const Msg* msg = &_msg;
        
        m_window[handle.idx] = new OpenGLWindow;
        m_window[handle.idx]->setTitle(msg->m_title);
        m_window[handle.idx]->setPosition(msg->m_x, msg->m_y);
        m_window[handle.idx]->resize(msg->m_width, msg->m_height);
        m_window[handle.idx]->show();
        m_flags[handle.idx] = msg->m_flags;

        void* nwh = qtNativeWindowHandle(m_window[handle.idx]);
        if (NULL != nwh)
        {
            m_eventQueue.postSizeEvent(handle, msg->m_width, msg->m_height);
            m_eventQueue.postWindowEvent(handle, nwh);
        }
    }
    void Context::onWindowDestroyEvent(WindowHandle _handle) 
    {
        WindowHandle handle = _handle;
        if (isValid(handle))
        {
            m_eventQueue.postWindowEvent(handle);
            delete m_window[handle.idx];
            m_window[handle.idx] = NULL;
        }
    }
    void Context::onWindowSetTitleEvent(WindowHandle _handle, const Msg _msg) 
    {
        WindowHandle handle = _handle;
        const Msg *msg = &_msg;
        if (isValid(handle))
        {
            m_window[handle.idx]->setTitle(msg->m_title);
        }
    }
    void Context::onWindowSetPosEvent(WindowHandle _handle, const Msg _msg) 
    {
        WindowHandle handle = _handle;
        const Msg *msg = &_msg;
        if (isValid(handle))
        {
            m_window[handle.idx]->setPosition(msg->m_x, msg->m_y);
        }
    }
    void Context::onWindowSetSizeEvent(WindowHandle _handle, const Msg _msg) 
    {
        WindowHandle handle = _handle;
        const Msg *msg = &_msg;
        if (isValid(handle))
        {
            m_window[handle.idx]->resize(msg->m_width, msg->m_height);
        }
    }
    void Context::onWindowToggleFrameEvent(WindowHandle _handle) 
    {
        BX_UNUSED(_handle);
    }
    void Context::onWindowToggleFullScreenEvent(WindowHandle _handle) 
    {
        BX_UNUSED(_handle);
    }
    void Context::onWindowMouseLockEvent(WindowHandle _handle, bool _lock) 
    {
        BX_UNUSED(_handle);
        BX_UNUSED(_lock);
    }
    
    struct MainThreadEntry
	{
		int m_argc;
		char** m_argv;

		static int32_t threadFunc(void* _userData);
	};
    class Worker : public QThread
    {
        Q_OBJECT
    
    	int m_argc;
		char** m_argv;
        int32_t m_exitCode;
        
        public:
            Worker(int argc, char **argv) : m_argc(argc), m_argv(argv), m_exitCode(0) { } 
            void run() Q_DECL_OVERRIDE
            {
                int32_t result = main(m_argc, m_argv);
                emit workerFinished(result);
            }

            int32_t getExitCode() const { return m_exitCode; }
            void shutdown() { wait(); }

        signals:
            void workerFinished(const int32_t result);
    };

    Context::~Context()
    {
    }

    int Context::run(int _argc, char** _argv)
    {
        QGuiApplication app(_argc, _argv);

        QDirIterator it(QCoreApplication::applicationDirPath());
        while (it.hasNext()) {
            QString filePath = it.next();
            if (it.fileInfo().suffix() == "rcc") {
                QResource::registerResource(filePath);
                qInfo() << "Registering resource" << filePath;
            }
        }

        m_windowAlloc.alloc();
        m_window[0] = new OpenGLWindow;
        m_window[0]->setTitle(QFileInfo(_argv[0]).fileName());
        m_flags[0] = 0
            | ENTRY_WINDOW_FLAG_ASPECT_RATIO
            | ENTRY_WINDOW_FLAG_FRAME
            ;
        
        qtSetWindow(m_window[0]);
        bgfx::renderFrame();

        m_window[0]->resize(m_width, m_height);
        m_window[0]->show();
        
        Worker worker(_argc, _argv);
        worker.start();

        // Force window resolution...
        WindowHandle defaultWindow = { 0 };
        setWindowSize(defaultWindow, m_width, m_height, true);

        while(!app.closingDown())
        {
            if (bgfx::RenderFrame::Exiting == bgfx::renderFrame() )
            {
                break;
            }
            app.processEvents();
        }
        s_ctx.m_eventQueue.postExitEvent();
        
        while (bgfx::RenderFrame::NoContext != bgfx::renderFrame() ) {};
        worker.shutdown();

        delete m_window[0];

        return worker.getExitCode();
    }	  

	const Event* poll() { return s_ctx.m_eventQueue.poll(); }

	const Event* poll(WindowHandle _handle) { return s_ctx.m_eventQueue.poll(_handle); }

	void release(const Event* _event) { s_ctx.m_eventQueue.release(_event); }

	WindowHandle createWindow(int32_t _x, int32_t _y, uint32_t _width, uint32_t _height, uint32_t _flags, const char* _title)
	{
		bx::MutexScope scope(s_ctx.m_lock);
		WindowHandle handle = { s_ctx.m_windowAlloc.alloc() };

		if (UINT16_MAX != handle.idx)
		{
			Msg msg;
			msg.m_x      = _x;
			msg.m_y      = _y;
			msg.m_width  = _width;
			msg.m_height = _height;
			msg.m_title  = _title;
			msg.m_flags  = _flags;

            emit s_ctx.signalWindowCreate(handle, msg);
		}

		return handle;
	}

	void destroyWindow(WindowHandle _handle)
	{
		if (UINT16_MAX != _handle.idx)
		{
            emit s_ctx.signalWindowDestroy(_handle);

			bx::MutexScope scope(s_ctx.m_lock);
			s_ctx.m_windowAlloc.free(_handle.idx);
		}
	}

	void setWindowPos(WindowHandle _handle, int32_t _x, int32_t _y)
	{
		Msg msg;
		msg.m_x = _x;
		msg.m_y = _y;

		emit s_ctx.signalWindowSetPos(_handle, msg);
	}

	void setWindowSize(WindowHandle _handle, uint32_t _width, uint32_t _height)
	{
		BX_UNUSED(_handle, _width, _height);
		Msg msg;
		msg.m_width  = _width;
		msg.m_height = _height;

		emit s_ctx.signalWindowSetSize(_handle, msg);
	}

	void setWindowTitle(WindowHandle _handle, const char* _title)
	{
		Msg msg;
		msg.m_title = _title;

		emit s_ctx.signalWindowSetTitle(_handle, msg);
	}

	void toggleWindowFrame(WindowHandle _handle)
	{
		emit s_ctx.signalWindowToggleFrame(_handle);
	}

	void toggleFullscreen(WindowHandle _handle)
	{
		emit s_ctx.signalWindowToggleFullScreen(_handle);
	}

	void setMouseLock(WindowHandle _handle, bool _lock)
	{
		emit s_ctx.signalWindowMouseLock(_handle, _lock);
	}
}

int main(int _argc, char** _argv)
{
	using namespace entry;	
	return s_ctx.run(_argc, _argv);
}

#include "entry_qt.moc"

#endif // QT_GUI_LIB
