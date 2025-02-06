#ifndef RTMPBASE_H
#define RTMPBASE_H


#include <map>
#include <string>


#include "globalhelp.h"
#include "rtmp.h"

//#ifdef _MSC_VER
//static inline int strcasecmp(const char *s1, const char *s2)
//{
//    //   while  (toupper((unsigned char)*s1) == toupper((unsigned char)*s2++))
//    //       if (*s1++ == '\0') return 0;
//    //   return(toupper((unsigned char)*s1) - toupper((unsigned char)*--s2));
//    while  ((unsigned char)*s1 == (unsigned char)*s2++)
//        if (*s1++ == '\0') {
//            return 0;
//        }
//    return((unsigned char) * s1 - (unsigned char) * --s2);
//}


class Properties :public std::map<std::string,std::string>
{
public :
    bool HasProperty(const std::string &key) const
    {
        return find(key) != end();
    }
    void SetProperty(const char* key, bool intval)
    {
        SetProperty(std::string(key), std::to_string(intval));
    }
    void SetProperty(const char* key, int intval)
    {
        SetProperty(std::string(key), std::to_string(intval));
    }

    void SetProperty(const char* key, uint32_t val)
    {
        SetProperty(std::string(key), std::to_string(val));
    }

    void SetProperty(const char* key, uint64_t val)
    {
        SetProperty(std::string(key), std::to_string(val));
    }

    void SetProperty(const char* key, const char* val)
    {
        SetProperty(std::string(key), std::string(val));
    }

    void SetProperty(const std::string &key, const std::string &val)
    {
        insert(std::pair<std::string, std::string>(key, val));
    }

    std::string GetProperty(const char*key)const
    {
        return GetProperty(key,"");
    }

    std::string GetProperty(const char* key, const std::string defaultValue) const
    {
        //Find item
        const_iterator it = find(std::string(key));
        //If not found
        if (it == end())
        //return default
        {
            return defaultValue;
        }
        //Return value
        return it->second;
    }

    std::string GetProperty(const std::string &key, const std::string defaultValue) const
    {
        //Find item
        const_iterator it = find(key);
        //If not found
        if (it == end())
        //return default
        {
            return defaultValue;
        }
        //Return value
        return it->second;
    }

    const char* GetProperty(const char* key, const char *defaultValue) const
    {
        //Find item
        const_iterator it = find(std::string(key));
        //If not found
        if (it == end())
        //return default
        {
            return defaultValue;
        }
        //Return value
        return it->second.c_str();
    }

    const char* GetProperty(const std::string &key, char *defaultValue) const
    {
        //Find item
        const_iterator it = find(key);
        //If not found
        if (it == end())
        //return default
        {
            return defaultValue;
        }
        //Return value
        return it->second.c_str();
    }

    int GetProperty(const char* key, int defaultValue) const
    {
        return GetProperty(std::string(key), defaultValue);
    }

    int GetProperty(const std::string &key, int defaultValue) const
    {
        //Find item
        const_iterator it = find(key);
        //If not found
        if (it == end())
        //return default
        {
            return defaultValue;
        }
        //Return value
        return atoi(it->second.c_str());
    }

    uint64_t GetProperty(const char* key, uint64_t defaultValue) const
    {
        return GetProperty(std::string(key), defaultValue);
    }

    uint64_t GetProperty(const std::string &key, uint64_t defaultValue) const
    {
        //Find item
        const_iterator it = find(key);
        //If not found
        if (it == end())
        //return default
        {
            return defaultValue;
        }
        //Return value
        return atoll(it->second.c_str());
    }

    bool GetProperty(const char* key, bool defaultValue) const
    {
        return GetProperty(std::string(key), defaultValue);
    }

//    bool GetProperty(const std::string &key, bool defaultValue) const
//    {
//        //Find item
//        const_iterator it = find(key);
//        //If not found
//        if (it == end())
//        //return default
//        {
//            return defaultValue;
//        }
//        //Get value
//        char * val = (char *)it->second.c_str();
//        //Check it
//        if (strcasecmp(val, (char *)"yes") == 0) {
//            return true;
//        } else if (strcasecmp(val, (char *)"true") == 0) {
//            return true;
//        }
//        //Return value
//        return (atoi(val));
//    }
};

class RtmpBase
{
public:
    RtmpBase();

    virtual bool Connect(std::string url);
    bool Connect();
private:
    void initRtmp();
protected:
    RTMP *rtmp_;

    std::string url_;
};

// 用于缓存解码后的数据
typedef struct Frame {
    AVFrame		*frame;         // 指向数据帧
    int		serial;             // 帧序列，在seek的操作时serial会变化
    double		pts;            // 时间戳，单位为秒
    double		duration;       // 该帧持续时间，单位为秒
    int64_t pos;
    int		width;              // 图像宽度
    int		height;             // 图像高读
    int		format;             // 对于图像为(enum AVPixelFormat)
    AVRational sar;
    int uploaded;
    int flip_v;
} Frame;

#endif // RTMPBASE_H
