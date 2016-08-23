#ifndef ENTRY_QT_H__HEADER_GUARD
#define ENTRY_QT_H__HEADER_GUARD

#include <Qt>
#include <QMetaMethod>


static void qMetaObjectInvokeMethod(QObject* p, const char *signature, const QList<QGenericArgument>& args) {
  if ( args.isEmpty() )
		QMetaObject::invokeMethod(p, signature);
	else if ( args.count() == 1 )
		QMetaObject::invokeMethod(p, signature, args.at(0));
	else if ( args.count() == 2 )
		QMetaObject::invokeMethod(p, signature, args.at(0), args.at(1));
	else if ( args.count() == 3 )
		QMetaObject::invokeMethod(p, signature, args.at(0), args.at(1), args.at(2));
	else if ( args.count() == 4 )
		QMetaObject::invokeMethod(p, signature, args.at(0), args.at(1), args.at(2), args.at(3));
	else if ( args.count() == 5 )
		QMetaObject::invokeMethod(p, signature, args.at(0), args.at(1), args.at(2), args.at(3), args.at(4));
	else if ( args.count() == 6 )
		QMetaObject::invokeMethod(p, signature, args.at(0), args.at(1), args.at(2), args.at(3), args.at(4), args.at(5));
	else if ( args.count() == 7 )
		QMetaObject::invokeMethod(p, signature, args.at(0), args.at(1), args.at(2), args.at(3), args.at(4), args.at(5), args.at(6));
	else if ( args.count() == 8 )
		QMetaObject::invokeMethod(p, signature, args.at(0), args.at(1), args.at(2), args.at(3), args.at(4), args.at(5), args.at(6), args.at(7));
	else if ( args.count() == 9 )
		QMetaObject::invokeMethod(p, signature, args.at(0), args.at(1), args.at(2), args.at(3), args.at(4), args.at(5), args.at(7), args.at(8));
	else if ( args.count() == 10 )
		QMetaObject::invokeMethod(p, signature, args.at(0), args.at(1), args.at(2), args.at(3), args.at(4), args.at(5), args.at(7), args.at(8), args.at(9));
}

static void qMetaObjectInvokeMethod(QObject* o, const char *signature, const QList<QVariant>& args) {
	QList<QGenericArgument> temp;
	foreach (const QVariant& a, args)
		temp << QGenericArgument(a.typeName(), a.data());
	qMetaObjectInvokeMethod(o, signature, temp);
}

static QByteArray createMethodSignature(const char* methodName, const QList<QVariant>& args) {
	QByteArray signature = methodName;
	if (args.isEmpty()) signature.append("()");
	else {
		signature.append("(");
		for (int i=0;i<args.size();i++){
			if (i!=0) signature.append(",");
			signature.append(args[i].typeName());
		}
		signature.append(")");
	}
	return signature;
}


// Just a helper macro:
#define NO_RETURN_VALUE

// This does the real work:
#define THREAD_CALL(targetThread, returnValue, args)                 \
    if(QThread::currentThread() != targetThread)                     \
    {                                                                \
        QString slotName = __FUNCTION__;                             \
        slotName.remove(QRegExp("^.*::"));                           \
        bool ret = metaObject()->invokeMethod(this,                  \
                qPrintable(slotName), Qt::QueuedConnection,          \
                args.count() >=  1 ? args[0] : QGenericArgument(0),  \
                args.count() >=  2 ? args[1] : QGenericArgument(0),  \
                args.count() >=  3 ? args[2] : QGenericArgument(0),  \
                args.count() >=  4 ? args[3] : QGenericArgument(0),  \
                args.count() >=  5 ? args[4] : QGenericArgument(0),  \
                args.count() >=  6 ? args[5] : QGenericArgument(0),  \
                args.count() >=  7 ? args[6] : QGenericArgument(0),  \
                args.count() >=  8 ? args[7] : QGenericArgument(0),  \
                args.count() >=  9 ? args[8] : QGenericArgument(0),  \
                args.count() >= 10 ? args[9] : QGenericArgument(0)); \
        if(!ret)                                                     \
        {                                                            \
            qFatal(qPrintable(__FUNCTION__ +                         \
              QString("Could not call QMetaObject::invokeMethod(). " \
              "Check your argument list quantity and types.")));     \
        }                                                            \
        return returnValue;                                          \
     }        

#define THREAD_SAFE_0(TargetThread, returnValue)                     \
        do {                                                         \
          QList<QGenericArgument> args;                              \
          THREAD_CALL((TargetThread, returnValue, args);             \
        } while (0);                                                 \


#define THREAD_SAFE_1(TargetThread, returnValue, ArgType1, ArgName1) \
        do {                                                         \
          QList<QGenericArgument> args =                             \
            QList<QGenericArgument>() <<                             \
            Q_ARG(ArgType1, ArgName1);                               \
          THREAD_CALL(TargetThread, returnValue, args);              \
        } while (0);              


#define THREAD_SAFE_2(TargetThread, returnValue, ArgType1, ArgName1, ArgType2, ArgName2) \
        do {                                                         \
          QList<QGenericArgument> args =                             \
            QList<QGenericArgument>() <<                             \
            Q_ARG(ArgType1, ArgName1) <<                             \
            Q_ARG(ArgType2, ArgName2);                               \
          THREAD_MAGIC(TargetThread, returnValue, args);             \
        } while (0);



#define PROP(Object, Member) Object->property(#Member)
#define INVOKE(Object, Member, ...) \
{ \
  bool res = QMetaObject::invokeMethod(Object, #Member, Qt::DirectConnection, __VA_ARGS__); \
  Q_ASSERT(res); Q_UNUSED(res); \
}



// CALL(func, type1, arg1, func2, arg2): pseudo-signal to object
#define CALL(pointer, meth, ...) \
{ \
  QByteArray normalizedSignature = QMetaObject::normalizedSignature( #meth "(" ARGS(S, ##__VA_ARGS__) ")" ); \
  const QMetaObject* rmo = pointer->metaObject(); \
  int methodIndex = rmo->indexOfMethod(normalizedSignature); \
  QMetaMethod method = rmo->method(methodIndex); \
  method.invoke(pointer, Qt::DirectConnection ARGS(Q, ##__VA_ARGS__)); \
}

// e.g. ARGS(S, int, bar, double, x) => "int, double"
// e.g. ARGS(Q, int, bar, double, x) => , Q_ARG(int, bar), Q_ARG(double, x)
#define ARGS(...) ARGSN(__VA_ARGS__,4,3,3,2,2,1,1,0,0)
#define ARGSN(FMT,a,b,c,d,e,f,g,h,i,...) ARGS##i(FMT,a,b,c,d,e,f,g,h)
#define ARGS0(FMT,...) 
#define ARGS1(FMT,a,b,...) ARGHELP##FMT(a,b) 
#define ARGS2(FMT,a,b,...) ARGHELP##FMT(a,b) SEP##FMT ARGS1(FMT,__VA_ARGS__)
#define ARGS3(FMT,a,b,...) ARGHELP##FMT(a,b) SEP##FMT ARGS2(FMT,__VA_ARGS__)
#define ARGS4(FMT,a,b,...) ARGHELP##FMT(a,b) SEP##FMT ARGS3(FMT,__VA_ARGS__)
#define ARGHELPS(a,b) #a
#define ARGHELPQ(a,b) , Q_ARG(a, b)
#define SEPS ","
#define SEPQ
// end definition of CALL


#endif // ENTRY_QT_H__HEADER_GUARD

