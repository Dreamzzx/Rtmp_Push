#ifndef LOGSERVE_H
#define LOGSERVE_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>
#include <cstdarg>
#include <mutex>

class LogServe
{
public:
    enum Level{
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        OFF,
        LEVEL_COUNT
    };

    static LogServe* instance(); // 获取实例

    void open(std::string filename);

    void close();

    void log(Level level,const char* file,int line,const char* format,...);
private:
    LogServe();
    ~LogServe();

    std::ofstream fileStream_;
    static const char* log_level[LEVEL_COUNT];
    std::string log_filename;
    static LogServe* m_instance;
    std::mutex fileMutex_;
};

#define LogDebug(format,...) \
LogServe::instance()->log(LogServe::DEBUG,__FILE__,__LINE__,format,##__VA_ARGS__);

#define LogInfo(format,...) \
LogServe::instance()->log(LogServe::INFO,__FILE__,__LINE__,format,##__VA_ARGS__);

#define LogWarn(format,...) \
LogServe::instance()->log(LogServe::WARN,__FILE__,__LINE__,format,##__VA_ARGS__);

#define LogError(format,...) \
LogServe::instance()->log(LogServe::ERROR,__FILE__,__LINE__,format,##__VA_ARGS__);

#define LogFatal(format,...) \
LogServe::instance()->log(LogServe::FATAL,__FILE__,__LINE__,format,##__VA_ARGS__);

#endif // LOGSERVE_H
