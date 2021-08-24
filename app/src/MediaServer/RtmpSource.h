
#ifndef _RTMP_SOURCCE_H
#define _RTMP_SOURCCE_H

#include "RtmpPlayer.h"

#include "sys/sync.hpp"

#include <list>
#include <memory>

class RtmpSource
{
public:
    RtmpSource(const char* recName);
    RtmpSource(RtmpSource &&) = default;
    RtmpSource(const RtmpSource &) = default;
    RtmpSource &operator=(RtmpSource &&) = default;
    RtmpSource &operator=(const RtmpSource &) = default;
    ~RtmpSource();

    ThreadLocker locker;
    struct flv_demuxer_t* demuxer;
    std::list<std::shared_ptr<RtmpPlayer> > players;
    bool isRecord;
    std::string recordFileName = "source.flv";
    std::string key;
    void* flvWHandle = nullptr;
    void doRecord(int type, const void* data, size_t bytes, uint32_t timestamp);
private:
    int dohandler(int codec, const void* data, size_t bytes, uint32_t pts, uint32_t dts, int flags);
    static int handler(void* param, int codec, const void* data, size_t bytes, uint32_t pts, uint32_t dts, int flags);
};

#endif
