#ifndef _rtmp_server_h_
#define _rtmp_server_h_

#include <stdint.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

#include "rtmp-session.h"

typedef struct rtmp_server_t rtmp_server_t;

struct rtmp_server_handler_t
{
	///network implementation
	///@param[in] payload rtmp payload, could be NULL
	///@param[in] bytes rtmp payload size in byte, could be 0
	///@return >0-sent bytes, <0-error
	int (*send)(cor_netfd_t netFd, const void* header, size_t len, const void* payload, size_t bytes);

	///@return 0-ok, other-error
	//int (*oncreate_stream)(void* param, uint32_t* stream_id);
	//int (*ondelete_stream)(void* param, uint32_t stream_id);

	///pull(server -> client)
	///@return 0-ok, other-error
	int (*onplay)(	rtmp_session_t* session, const char* app, const char* stream, double start, double duration, uint8_t reset);
	int (*onpause)(	rtmp_session_t* session, int pause, uint32_t ms);
	int (*onseek)(	rtmp_session_t* session, uint32_t ms);

	///push(client -> server)
	///@param[in] type: live/record/append
	///@return 0-ok, other-error
	int (*onpublish)(rtmp_session_t* session, const char* app, const char* stream, const char* type);
	///@param[in] data FLV VideoTagHeader + AVCVIDEOPACKET: AVCDecoderConfigurationRecord(ISO 14496-15) / One or more NALUs(four-bytes length + NALU)
	///@return 0-ok, other-error
	int (*onvideo)(rtmp_session_t* session, const void* data, size_t bytes, uint32_t timestamp);
	///@param[in] data FLV AudioTagHeader + AACAUDIODATA: AudioSpecificConfig(14496-3) / Raw AAC frame data in UI8
	///@return 0-ok, other-error
	int (*onaudio)(rtmp_session_t* session, const void* data, size_t bytes, uint32_t timestamp);
	///@param[in] data FLV onMetaData
	///@return 0-ok, other-error
	int (*onscript)(rtmp_session_t* session, const void* data, size_t bytes, uint32_t timestamp);

	///@param[out] duration stream length in seconds
	///@return 0-ok, other-error
	int (*ongetduration)(rtmp_session_t* session, const char* app, const char* stream, double* duration);
	void (*session_before_destroy)(rtmp_session_t* session);
};

rtmp_server_t* rtmp_server_create(void* param, const struct rtmp_server_handler_t* handler);

void rtmp_server_destroy(rtmp_server_t* rtmp);

/// @param[in] rtmp rtmp_server_create instance
/// @param[in] data rtmp chunk stream data
/// @param[in] bytes data length
/// @return 0-ok, other-error
int rtmp_server_input(rtmp_session_t* session, const uint8_t* data, size_t bytes);

/// send audio/video data(VOD only)
/// @param[in] rtmp rtmp_server_create instance
/// @return 0-ok, other-error
int rtmp_server_send_audio(rtmp_session_t* session, const void* data, size_t bytes, uint32_t timestamp);
int rtmp_server_send_video(rtmp_session_t* session, const void* data, size_t bytes, uint32_t timestamp);
int rtmp_server_send_script(rtmp_session_t* session, const void* data, size_t bytes, uint32_t timestamp);

#ifdef __cplusplus
}
#endif
#endif /* !_rtmp_server_h_ */
