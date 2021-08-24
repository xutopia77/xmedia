
#ifndef _RTMP_PLAYER_H
#define _RTMP_PLAYER_H

#include "rtmp-server.h"

#include "flv-writer.h"
#include "flv-proto.h"
#include "flv-muxer.h"
#include "flv-demuxer.h"

class RtmpPlayer
{
public:
    RtmpPlayer();
    RtmpPlayer(RtmpPlayer &&) = default;
    RtmpPlayer(const RtmpPlayer &) = default;
    RtmpPlayer &operator=(RtmpPlayer &&) = default;
    RtmpPlayer &operator=(const RtmpPlayer &) = default;

    // TODO: add packet queue
    rtmp_session_t* session;
    struct flv_muxer_t* muxer;

    RtmpPlayer(rtmp_session_t* _session) : session(_session){
        muxer = flv_muxer_create(&handler, session);
    }

    ~RtmpPlayer(){
        if(muxer)
            flv_muxer_destroy(muxer);
    }

private:
    static int handler(void* session, int type, const void* data, size_t bytes, uint32_t timestamp);
};

#endif
