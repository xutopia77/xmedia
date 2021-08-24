#ifndef _rtmp_client_h_
#define _rtmp_client_h_

#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include "rtmp-util.h"
#include "rtmp-session.h"

typedef struct rtmp_client_t rtmp_client_t;


struct rtmp_client_handler_t
{
	///network implementation
	///@return >0-sent bytes, <0-error
	int (*send)(cor_netfd_t netFd, const void* header, size_t len, const void* payload, size_t bytes);

	///VOD only
	///@param[in] video FLV VideoTagHeader + AVCVIDEOPACKET: AVCDecoderConfigurationRecord(ISO 14496-15) / One or more NALUs(four-bytes length + NALU)
	///@param[in] audio FLV AudioTagHeader + AACAUDIODATA: AudioSpecificConfig(14496-3) / Raw AAC frame data in UI8
	///@param[in] script AMF0/AMF3
	///@return 0-ok, other-error
	int (*onvideo)(void* param, const void* video, size_t bytes, uint32_t timestamp);
	int (*onaudio)(void* param, const void* audio, size_t bytes, uint32_t timestamp);
	int (*onscript)(void* param, const void* script, size_t bytes, uint32_t timestamp);
};

/// URL: rtmp://host/app/playpath (TCURL: rtmp://host/app)
/// e.g.: rtmp://live.alivecdn.com/live/hello
/// rtmp_client_create("live", "hello", "rtmp://live.alivecdn.com/live", param, handler)
/// @param[in] appname rtmp app name
/// @param[in] playpath rtmp playpath
/// @param[in] tcurl rtmp url (only host and app name)
rtmp_client_t* rtmp_client_create(const char* appname, const char* playpath, const char* tcurl, void* param, const struct rtmp_client_handler_t* handler);
void client_init_session(rtmp_session_t* session);
void rtmp_client_destroy(rtmp_client_t* rtmp);

///@return 0-ok, other-error
RtmpRe rtmp_client_input(rtmp_session_t* session, const void* data, size_t bytes);

///@param[in] publish 0-Publish(push stream to server), 1-LIVE/VOD(pull from server), 2-LIVE only, 3-VOD only
///@return 0-ok, other-error
int rtmp_client_start(rtmp_session_t* session, int publish);
int rtmp_client_stop(rtmp_session_t* session);
int rtmp_client_pause(rtmp_session_t* session, int pause); // VOD only
int rtmp_client_seek(rtmp_session_t* session, double timestamp); // VOD only

///@return RTMP_STATE_START(4): push video/audio
int rtmp_client_getstate(rtmp_client_t* rtmp);

///@param[in] video FLV VideoTagHeader + AVCVIDEOPACKET: AVCDecoderConfigurationRecord(ISO 14496-15) / One or more NALUs(four-bytes length + NALU)
///@param[in] bytes video data length in bytes
///@return 0-ok, other-error
int rtmp_client_push_video(rtmp_session_t* session, const void* video, size_t bytes, uint32_t timestamp);

///@param[in] audio FLV AudioTagHeader + AACAUDIODATA: AudioSpecificConfig(14496-3) / Raw AAC frame data in UI8
///@param[in] bytes audio data length in bytes
///@return 0-ok, other-error
int rtmp_client_push_audio(rtmp_session_t* session, const void* audio, size_t bytes, uint32_t timestamp);

///@param[in] data FLV onMetaData
int rtmp_client_push_script(rtmp_session_t* session, const void* data, size_t bytes, uint32_t timestamp);

#if defined(__cplusplus)
}
#endif
#endif /* !_rtmp_client_h_ */
