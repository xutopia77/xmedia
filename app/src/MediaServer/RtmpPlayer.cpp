
#include "RtmpPlayer.h"
#include "xlog.h"

#include "rtmp-server.h"

RtmpPlayer::RtmpPlayer()
{
}

int RtmpPlayer::handler(void* _session, int type, const void* data, size_t bytes, uint32_t timestamp){
    // LOG_WARN("not realize yet %s\n", __FUNCTION__);
    rtmp_session_t* session = (rtmp_session_t*)_session;
    switch (type)
    {
    case FLV_TYPE_SCRIPT:
        return rtmp_server_send_script(session, data, bytes, timestamp);
    case FLV_TYPE_AUDIO:
        return rtmp_server_send_audio(session, data, bytes, timestamp);
    case FLV_TYPE_VIDEO:
        return rtmp_server_send_video(session, data, bytes, timestamp);
    default:
        assert(0);
        return -1;
    }

    return 0;
}
