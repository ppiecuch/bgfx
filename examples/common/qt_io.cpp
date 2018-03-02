#ifdef QT_CORE_LIB

#include <qdebug.h>
#include <qfile.h>

// (-1) if file not exists
extern "C" long qFileSize(const char *filename)
{
	long size = -1;
	QFile f(filename);
	if (f.open(QIODevice::ReadOnly)) {
	    return f.size(); // when file does open.
	} else {
        // search for standard locations
        QStringList paths;
        paths << ":/assets/" << ":/runtime/";
        foreach(const QString &p, paths)
        {
            QFile f(QString("%1%2").arg(p).arg(filename));
            if (f.open(QIODevice::ReadOnly))
                return f.size(); // when file does open.
        }
    }
	return size;
}

extern "C" long qReadFile(const char *filename, char *buffer, long maxSize)
{
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly))
  {
     // search for standard locations
     QStringList paths;
     paths << ":/assets/" << ":/runtime/";
     foreach(const QString &p, paths)
     {
        QFile file(QString("%1%2").arg(p).arg(filename));
        if (file.open(QIODevice::ReadOnly))
            return file.read(buffer, maxSize);
     }
     return 0;
  }
  return file.read(buffer, maxSize);
}

#endif // QT_CORE_LIB
