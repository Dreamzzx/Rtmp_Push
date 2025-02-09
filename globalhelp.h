#ifndef GLOBALHELP_H
#define GLOBALHELP_H

extern "C"{
    #include "libavutil/avstring.h"
    #include "libavutil/eval.h"
    #include "libavutil/mathematics.h"
    #include "libavutil/pixdesc.h"
    #include "libavutil/imgutils.h"
    #include "libavutil/dict.h"
    //#include "libavutil/parseutils.h"
    #include "libavutil/samplefmt.h"
    //#include "libavutil/avassert.h"
    //#include "libavutil/time.h"
    #include "libavformat/avformat.h"
    #include "libavdevice/avdevice.h"
    #include "libswscale/swscale.h"
    #include "libavutil/opt.h"
    #include "libavcodec/avfft.h"
    #include "libswresample/swresample.h"
    #include "libavcodec/avcodec.h"
    #include "libavutil/audio_fifo.h"
    #include"libavutil/opt.h"
    #include "libavutil/samplefmt.h"
    #include "rtmp.h"
}

#include <QString>

class GlobalHelp
{
public:
    GlobalHelp();
};

#endif // GLOBALHELP_H
