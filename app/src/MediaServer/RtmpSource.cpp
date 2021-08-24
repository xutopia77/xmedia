
#include "RtmpSource.h"

#include "flv-writer.h"
#include "flv-proto.h"
#include "flv-muxer.h"
#include "flv-demuxer.h"

#include "xlog.h"
#include "rtmp-util.h"
#include <string>

using namespace std;

RtmpSource::RtmpSource(const char* recName){
    flvWHandle = nullptr;
    if(nullptr == recName){
        isRecord=false;
    }
    else{
        isRecord=true;
        recordFileName = recName;
    }
    
    demuxer = flv_demuxer_create(handler, this);
    // LOG_RTMP_DBG("create source %s\n", isRecord ? string(string("record file ") + recordFileName).c_str() : "not record");
    if(isRecord){
        if(flvWHandle == nullptr){
            flvWHandle = flv_writer_create(recordFileName.c_str());
            usr_assert(flvWHandle != nullptr);
        }
    }
}

RtmpSource::~RtmpSource()
{
    if (demuxer)
        flv_demuxer_destroy(demuxer);
    if(flvWHandle)
        flv_writer_destroy(flvWHandle);
}

void RtmpSource::doRecord(int type, const void* data, size_t bytes, uint32_t timestamp){
    if(isRecord){
        if(flvWHandle == nullptr){
            usr_assert(flvWHandle != nullptr);
            LOG_RTMP_DBG("record file name %s\n", recordFileName.c_str());
            flvWHandle = flv_writer_create(recordFileName.c_str());
            usr_assert(flvWHandle != nullptr);
        }
        flv_writer_input(flvWHandle, type, data, bytes, timestamp);
    }
}

int RtmpSource::dohandler(int codec, const void* data, size_t bytes, uint32_t pts, uint32_t dts, int flags){
    int r = 0;
    AutoThreadLocker alocker(locker);
    
    for (auto it = players.begin(); it != players.end(); ++it)
    {
        // TODO: push to packet queue

        switch (codec)
        {
        case FLV_VIDEO_H264:
            r = flv_muxer_avc((*it)->muxer, data, bytes, pts, dts);
            break;
        case FLV_VIDEO_H265:
            r = flv_muxer_hevc((*it)->muxer, data, bytes, pts, dts);
            break;
        case FLV_AUDIO_AAC:
            r = flv_muxer_aac((*it)->muxer, data, bytes, pts, dts);
            break;
        case FLV_AUDIO_MP3:
            r = flv_muxer_mp3((*it)->muxer, data, bytes, pts, dts);
            break;

        case FLV_VIDEO_AVCC:
        case FLV_VIDEO_HVCC:
        case FLV_AUDIO_ASC:
            break; // ignore

        default:
            LOG_RTMP_WARN("codec=%d\n", codec);
            if(768 != codec)
                assert(0);
        }
    }
    return r;
}

int RtmpSource::handler(void* param, int codec, const void* data, size_t bytes, uint32_t pts, uint32_t dts, int flags){ 
    RtmpSource* s = (RtmpSource*)param;
    s->dohandler(codec, data, bytes, pts, dts, flags);
    return 0; // ignore error
}
