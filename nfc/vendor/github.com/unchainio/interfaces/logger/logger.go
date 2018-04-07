package logger

type Logger interface {
	Printf(format string, v ...interface{})
	Fatalf(format string, v ...interface{})
	Panicf(format string, v ...interface{})
	Debugf(format string, v ...interface{})
	Warnf(format string, v ...interface{})
	Warningf(format string, v ...interface{})
	Errorf(format string, v ...interface{})
	Print(v ...interface{})
	Fatal(v ...interface{})
	Panic(v ...interface{})
	Debug(v ...interface{})
	Warn(v ...interface{})
	Warning(v ...interface{})
	Error(v ...interface{})
	Println(v ...interface{})
	Fatalln(v ...interface{})
	Panicln(v ...interface{})
	Debugln(v ...interface{})
	Warnln(v ...interface{})
	Warningln(v ...interface{})
	Errorln(v ...interface{})
}