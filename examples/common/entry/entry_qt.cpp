/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * Copyright 2014-2015 Pawel Piecuch. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "entry_p.h"

#ifdef QT_GUI_LIB

#include <bgfxplatform.h>
#include <bgfx.qt.h>
#include "dbg.h"
 	
// This is just trivial implementation of Qt integration.
// It's here just for testing purpose.

#include <Qt>
#include <QDebug>
#include <QTime>
#include <QKeyEvent>
#include <QFileInfo>
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
	    case QtDebugMsg:
	      fprintfMsg("Debug", context, msg);
	      break;
	    case QtWarningMsg:
	      fprintfMsg("Warning", context, msg);
	      break;
	    case QtCriticalMsg:
	      fprintfMsg("Critical", context, msg);
	      break;
	    case QtFatalMsg:
	      fprintfMsg("Fatal", context, msg);
	      exit(EXIT_FAILURE);
	    }
	  }
	}
  
	class OpenGLWindow : public QWindow, public QOpenGLFunctions
	{
	  Q_OBJECT
	  Q_PROPERTY(bool alwaysRefresh READ alwaysRefresh WRITE setAlwaysRefresh)
	  Q_PROPERTY(bool bgfxReady READ bgfxReady WRITE setBgfxReady)

  
  private:
    bool _isGLInitialized;
    bool _isWindowInitialized;
  public:
    void setWindowInitialized() { _isWindowInitialized = true; }

	public:
	  explicit OpenGLWindow(QScreen *screen = 0) : QWindow(screen),
              _isGLInitialized(0), _isWindowInitialized(0),
						  _context(0), 
						  _alwaysRefresh(false), _bgfxReady(false),
						  _pendingRefresh(false)
	  { initializeContext(); }

	  explicit OpenGLWindow(QWindow *parent) : QWindow(parent),
              _isGLInitialized(0), _isWindowInitialized(0),
		          _context(0),
		          _alwaysRefresh(false), _bgfxReady(false),
		          _pendingRefresh(false)
	  { initializeContext(); }

	  virtual ~OpenGLWindow() { if (_context) delete _context; }

	  bool alwaysRefresh() const { return _alwaysRefresh; }
	  void setAlwaysRefresh(bool alwaysRefresh) {
	    _alwaysRefresh = alwaysRefresh;
	    if (alwaysRefresh) requestRefresh();
	  }
	  bool bgfxReady() const { return _bgfxReady; }
	  void setBgfxReady(bool bgfxReady) {
	    _bgfxReady = bgfxReady;
	    if (bgfxReady) initializeWindow();
	  }
    
	signals:
	  void aboutToResize();
	  void resized();

	public slots:
	  QOpenGLContext *context() const { Q_ASSERT(_context!=NULL); return _context; }
	  void makeCurrent() { qDebug() << Q_FUNC_INFO; _context->makeCurrent(this); }
	  void swapBuffers() { qDebug() << Q_FUNC_INFO; _context->swapBuffers(this); }

	  void grabContext(QThread *toThread) {
	    
      Q_ASSERT(_bgfxReady==true);
qDebug() << Q_FUNC_INFO;
      QRendererContextGL *c = qtGetRender();

      c->lockRenderer();
	    QMutexLocker lock(c->grabMutex());
	    context()->moveToThread(toThread);
	    c->grabCond()->wakeAll();
	    c->unlockRenderer();
	  }
	  
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

	public slots:
	  void render() {
      
      QRendererContextGL *c = qtGetRender();
      Q_ASSERT(c==NULL);

      // We are on the gui thread here. Wait until the render thread finishes.
      c->lockRenderer();

      if (!_isGLInitialized) {
        _context->makeCurrent(this);
        initializeOpenGLFunctions();
        initializeWindow();
        _isGLInitialized = true;
      }
      c->unlockRenderer();
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
	    qDebug() << Q_FUNC_INFO << ":\n\t" << QOpenGLContext::currentContext() << ":\n\t" << qtGetRender();

      connect(qtGetRender(), SIGNAL(contextWanted(QThread*)), this, SLOT(grabContext(QThread*)));

	    _time.start();
	  }


	  QOpenGLContext *_context;
    
	  bool _alwaysRefresh;
	  bool _bgfxReady;
	  bool _pendingRefresh;
	  QTime _time;
	};

	static void _finishCtx();

	struct Context
	{
		Context() : m_app(0), m_window(0)
		{
		}
		~Context()
		{
			m_window->close();
			m_app->exit();
		}
		int run(int _argc, char** _argv)
		{
			m_app = new QGuiApplication(_argc, _argv);

			// init default format
			QSurfaceFormat format;
			format.setSamples(4);
			format.setDepthBufferSize(24);
#if BGFX_CONFIG_RENDERER_OPENGL >= 31
			format.setProfile( QSurfaceFormat::CoreProfile );
			format.setVersion( 3, 1 );
#endif // BGFX_CONFIG_RENDERER_OPENGL >= 31
			QSurfaceFormat::setDefaultFormat(format);

			QObject::connect( m_app, &QGuiApplication::lastWindowClosed, &_finishCtx );

			m_window = new OpenGLWindow;
			m_window->resize(640, 480);
			m_window->setTitle(QFileInfo(_argv[0]).fileName());
			bgfx::qtSetWindow(m_window, m_window->context());

			m_window->show();
      m_window->setWindowInitialized();
			
			return entry::main(_argc, _argv);
		}	  
		void processEvents()
		{
			m_app->processEvents();
		}

		EventQueue m_eventQueue;

		QGuiApplication *m_app;
		OpenGLWindow *m_window;
	};

	Context s_ctx;

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
