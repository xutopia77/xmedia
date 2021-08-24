
#include "cstringext.h"
#include "sys/sock.h"
#include "sys/system.h"
#include "sys/path.h"
#include "sys/sync.hpp"

#include "ctypedef.h"
#include "ntp-time.h"
#include "rtp-profile.h"
#include "rtsp-server.h"
#include "media/ps-file-source.h"
#include "media/h264-file-source.h"
#include "media/mp4-file-source.h"
#include "rtp-udp-transport.h"
#include "rtp-tcp-transport.h"
#include "rtsp-server-aio.h"
#include "uri-parse.h"
#include "urlcodec.h"
#include "path.h"
#include <map>
#include <memory>
#include "cpm/shared_ptr.h"

#include <CorThread.h>

#include "rtp-over-rtsp.h"

#include "rtsp-server.h"

#include "RtspSrv.h"
#include "xlog.h"
#include "CorRe.h"
#include "rtsp_util.h"

using namespace std;

#if defined(_HAVE_FFMPEG_)
#include "media/ffmpeg-file-source.h"
#include "media/ffmpeg-live-source.h"
#endif

struct rtsp_session_t{
	// socket_t socket;
	// aio_transport_t* aio;
	cor_netfd_t cliNetFd;
	struct rtp_over_rtsp_t rtp;
	int rtsp_need_more_data;//是否需要更多的数据
	uint8_t buffer[4 * 1024];

	struct rtsp_server_t *rtsp;
	struct sockaddr_storage addr;
	socklen_t addrlen;

	void (*onerror)(void* param, rtsp_server_t* rtsp, int code);
	void (*onrtp)(void* param, uint8_t channel, const void* data, uint16_t bytes);
	void* param;//在 handle_session 中被置位NULL
};

extern shared_ptr<RtspSrv> g_rtspSrv;

std::map<std::string, TFileDescription> s_describes;

static const char* s_workdir = "e:\\";
//static const char* s_workdir = "/Users/ireader/video/";

#define UDP_MULTICAST_ADDR "239.0.0.2"
#define UDP_MULTICAST_PORT 6000

int rtsp_uri_parse(const char* uri, std::string& path){
	char path1[256];
	struct uri_t* r = uri_parse(uri, strlen(uri));
	if(!r)
		return -1;

	url_decode(r->path, strlen(r->path), path1, sizeof(path1));
	path = path1;
	uri_free(r);
	return 0;
}

RtspSrv::RtspSrv(){}

RtspSrv::~RtspSrv(){}

int RtspSrv::ondescribe(void* /*ptr*/, rtsp_server_t* rtsp, const char* uri){
	static const char* pattern_vod =
		"v=0\n"
		"o=- %llu %llu IN IP4 %s\n"
		"s=%s\n"
		"c=IN IP4 0.0.0.0\n"
		"t=0 0\n"
		"a=range:npt=0-%.1f\n"
		"a=recvonly\n"
		"a=control:*\n"; // aggregate control

	static const char* pattern_live =
		"v=0\n"
		"o=- %llu %llu IN IP4 %s\n"
		"s=%s\n"
		"c=IN IP4 0.0.0.0\n"
		"t=0 0\n"
		"a=range:npt=now-\n" // live
		"a=recvonly\n"
		"a=control:*\n"; // aggregate control

    std::string filename;
	std::map<std::string, TFileDescription>::const_iterator it;
	string strSourceIdPrefix = "";
	rtsp_uri_parse(uri, filename);
	if (strstartswith(filename.c_str(), "/live/"))
	{
		filename = filename.c_str() + 6;
		strSourceIdPrefix = "/live/";
	}
	else if (strstartswith(filename.c_str(), "/vod/"))
	{
		filename = path::join(s_workdir, filename.c_str() + 5);
		strSourceIdPrefix = "/vod/";
	}
	else
	{
		// assert(0);
		fatal("err file %s\n", filename);
		return -1;
	}
	string strSourceId = strSourceIdPrefix + filename;
	char buffer[1024] = { 0 };
	{
		AutoThreadLocker locker(s_locker);
		it = s_describes.find(filename);
		if(it == s_describes.end())
		{
			// unlock
			TFileDescription describe;
			std::shared_ptr<IMediaSource> source;
			if (0 == strcmp(filename.c_str(), "camera"))
			{
#if defined(_HAVE_FFMPEG_)
				source.reset(new FFLiveSource("video=Integrated Webcam"));
#endif
				int offset = snprintf(buffer, sizeof(buffer), pattern_live, ntp64_now(), ntp64_now(), "0.0.0.0", uri);
				assert(offset > 0 && offset + 1 < (long)sizeof(buffer));
			}
			else
			{
				if (strendswith(filename.c_str(), ".ps"))
					source.reset(new PSFileSource(filename.c_str()));
				else if (strendswith(filename.c_str(), ".h264"))
					source.reset(new H264FileSource(filename.c_str()));
				else
				{
#if defined(_HAVE_FFMPEG_)
					source.reset(new FFFileSource(filename.c_str()));
#else
					source.reset(new MP4FileSource(filename.c_str()));
#endif
				}
				source->GetDuration(describe.duration);

				int offset = snprintf(buffer, sizeof(buffer), pattern_vod, ntp64_now(), ntp64_now(), "0.0.0.0", uri, describe.duration / 1000.0);
				assert(offset > 0 && offset + 1 < (long)sizeof(buffer));
			}

			source->GetSDPMedia(describe.sdpmedia);
			// re-lock
			it = s_describes.insert(std::make_pair(filename, describe)).first;

			RtspSourceInfo sourceInfo;
			sourceInfo.name = strSourceId;
			sourceInfo.duration = s_describes[filename].duration / 1000.0;
			srvInfo.sourceInfo[strSourceId] = sourceInfo;
			LOG_RTSP_DBG(" srvInfo new %d %s\n", srvInfo.sourceInfo.size(), strSourceId.c_str());

		}
	}
    
	std::string sdp = buffer;
	sdp += it->second.sdpmedia;

    return rtsp_server_reply_describe(rtsp, 200, sdp.c_str());
}

static void rtsp_get_filename_from_uri(const char* uri, string& _fileName, string& prefix){
	string filename;
	rtsp_uri_parse(uri, filename);
	string strSourceIdPrefix = "";
	if (strstartswith(filename.c_str(), "/live/"))
	{
		filename = filename.c_str() + 6;
		strSourceIdPrefix = "/live/";
	}
	else if (strstartswith(filename.c_str(), "/vod/")){
		filename = path::join(s_workdir, filename.c_str() + 5);
		strSourceIdPrefix = "/vod/";
	}
	else{
		LOG_ERR("parse uri err %s\n", uri);
		// // assert(0);
		// fatal("err file %s\n", filename);
		// return -1;
	}
	prefix = strSourceIdPrefix;
	_fileName = filename;
}

int RtspSrv::onsetup(void* /*ptr*/, rtsp_server_t* rtsp, const char* uri, const char* session, const struct rtsp_header_transport_t transports[], size_t num){
	std::string filename;
	char rtsp_transport[128];
	const struct rtsp_header_transport_t *transport = NULL;

	uint16_t	_info_cliPort;
	string 		_info_cliip;
	string 		_info_trackId;
	string 		_info_sourceId;

	// 文件路径
	rtsp_uri_parse(uri, filename);
	string strSourceIdPrefix = "";
	if (strstartswith(filename.c_str(), "/live/"))
	{
		filename = filename.c_str() + 6;
		strSourceIdPrefix = "/live/";
	}
	else if (strstartswith(filename.c_str(), "/vod/"))
	{
		filename = path::join(s_workdir, filename.c_str() + 5);
		strSourceIdPrefix = "/vod/";
	}
	else
	{
		// assert(0);
		fatal("err file %s\n", filename);
		return -1;
	}

	if ('\\' == *filename.rbegin() || '/' == *filename.rbegin())
		filename.erase(filename.end() - 1);
	const char* basename = path_basename(filename.c_str());
	if (NULL == strchr(basename, '.')) // filter track1
		filename.erase(basename - filename.c_str() - 1, std::string::npos);

	_info_trackId = basename;
	_info_sourceId = strSourceIdPrefix + filename;
	
	TSessions::iterator it;
	if(session)
	{
		AutoThreadLocker locker(s_locker);
		it = s_sessions.find(session);
		if(it == s_sessions.end())
		{
			// 454 Session Not Found
			return rtsp_server_reply_setup(rtsp, 454, NULL, NULL);
		}
		else
		{
			// don't support aggregate control
			if (0)
			{
				// 459 Aggregate Operation Not Allowed
				return rtsp_server_reply_setup(rtsp, 459, NULL, NULL);
			}
		}
	}
	else
	{
		rtsp_media_t item;
		item.rtsp = rtsp;
		item.channel = 0;
		item.status = 0;

		if (0 == strcmp(filename.c_str(), "camera"))
		{
#if defined(_HAVE_FFMPEG_)
			item.media.reset(new FFLiveSource("video=Integrated Webcam"));
#endif
		}
		else
		{
			if (strendswith(filename.c_str(), ".ps"))
				item.media.reset(new PSFileSource(filename.c_str()));
			else if (strendswith(filename.c_str(), ".h264"))
				item.media.reset(new H264FileSource(filename.c_str()));
			else
			{
#if defined(_HAVE_FFMPEG_)
				item.media.reset(new FFFileSource(filename.c_str()));
#else
				item.media.reset(new MP4FileSource(filename.c_str()));
#endif
			}
		}

		char rtspsession[32];
		snprintf(rtspsession, sizeof(rtspsession), "%p", item.media.get());

		AutoThreadLocker locker(s_locker);
		it = s_sessions.insert(std::make_pair(rtspsession, item)).first;
	}

	assert(NULL == transport);
	for(size_t i = 0; i < num && !transport; i++)
	{
		if(RTSP_TRANSPORT_RTP_UDP == transports[i].transport)
		{
			// RTP/AVP/UDP
			transport = &transports[i];
		}
		else if(RTSP_TRANSPORT_RTP_TCP == transports[i].transport)
		{
			// RTP/AVP/TCP
			// 10.12 Embedded (Interleaved) Binary Data (p40)
			transport = &transports[i];
		}
	}
	if(!transport)
	{
		// 461 Unsupported Transport
		return rtsp_server_reply_setup(rtsp, 461, NULL, NULL);
	}

	rtsp_media_t &item = it->second;
	if (RTSP_TRANSPORT_RTP_TCP == transport->transport)
	{
		// 10.12 Embedded (Interleaved) Binary Data (p40)
		int interleaved[2];
		if (transport->interleaved1 == transport->interleaved2)
		{
			interleaved[0] = item.channel++;
			interleaved[1] = item.channel++;
		}
		else
		{
			interleaved[0] = transport->interleaved1;
			interleaved[1] = transport->interleaved2;
		}

		item.transport = std::make_shared<RTPTcpTransport>(rtsp, interleaved[0], interleaved[1]);
		item.media->SetTransport(path_basename(uri), item.transport);

		// RTP/AVP/TCP;interleaved=0-1
		snprintf(rtsp_transport, sizeof(rtsp_transport), "RTP/AVP/TCP;interleaved=%d-%d", interleaved[0], interleaved[1]);		
	}
	else if(transport->multicast)
	{
        unsigned short port[2] = { transport->rtp.u.client_port1, transport->rtp.u.client_port2 };
        char multicast[65];
		// RFC 2326 1.6 Overall Operation p12
		
		if(transport->destination[0])
        {
            // Multicast, client chooses address
            snprintf(multicast, sizeof(multicast), "%s", transport->destination);
            port[0] = transport->rtp.m.port1;
            port[1] = transport->rtp.m.port2;
        }
        else
        {
            // Multicast, server chooses address
            snprintf(multicast, sizeof(multicast), "%s", UDP_MULTICAST_ADDR);
            port[0] = UDP_MULTICAST_PORT;
            port[1] = UDP_MULTICAST_PORT + 1;
        }
        
        item.transport = std::make_shared<RTPUdpTransport>();
        if(0 != ((RTPUdpTransport*)item.transport.get())->Init(multicast, port))
        {
            // log

            // 500 Internal Server Error
            return rtsp_server_reply_setup(rtsp, 500, NULL, NULL);
        }
        item.media->SetTransport(path_basename(uri), item.transport);

        // Transport: RTP/AVP;multicast;destination=224.2.0.1;port=3456-3457;ttl=16
        snprintf(rtsp_transport, sizeof(rtsp_transport),
            "RTP/AVP;multicast;destination=%s;port=%hu-%hu;ttl=%d",
            multicast, port[0], port[1], 16);
        
        // 461 Unsupported Transport
        //return rtsp_server_reply_setup(rtsp, 461, NULL, NULL);
	}
	else
	{
		// unicast
		item.transport = std::make_shared<RTPUdpTransport>();

		assert(transport->rtp.u.client_port1 && transport->rtp.u.client_port2);
		unsigned short port[2] = { transport->rtp.u.client_port1, transport->rtp.u.client_port2 };
		const char *ip = transport->destination[0] ? transport->destination : rtsp_server_get_client(rtsp, &_info_cliPort);
		_info_cliip = ip;
		if(0 != ((RTPUdpTransport*)item.transport.get())->Init(ip, port))
		{
			// log

			// 500 Internal Server Error
			return rtsp_server_reply_setup(rtsp, 500, NULL, NULL);
		}
		item.media->SetTransport(path_basename(uri), item.transport);

		// RTP/AVP;unicast;client_port=4588-4589;server_port=6256-6257;destination=xxxx
		snprintf(rtsp_transport, sizeof(rtsp_transport), 
			"RTP/AVP;unicast;client_port=%hu-%hu;server_port=%hu-%hu%s%s", 
			transport->rtp.u.client_port1, transport->rtp.u.client_port2,
			port[0], port[1],
			transport->destination[0] ? ";destination=" : "",
			transport->destination[0] ? transport->destination : "");

		std::map<std::string, RtspSourceInfo>::iterator sourceInfoIt = srvInfo.sourceInfo.find(_info_sourceId);
		if(sourceInfoIt == srvInfo.sourceInfo.end()){
			LOG_RTSP_ERR("err not find sourceInfo %s\n", uri);
		}
		else{
			std::map<std::string, RtspClientInfo>::iterator rtspClientInfoIt;
			rtspClientInfoIt = srvInfo.sourceInfo[_info_sourceId].playerInfo.find(it->first);
			if(rtspClientInfoIt != srvInfo.sourceInfo[_info_sourceId].playerInfo.end()){
				RtspClientInfo& rtspClientInfo = srvInfo.sourceInfo[_info_sourceId].playerInfo[it->first];
				rtspClientInfo.info += _info_trackId + string(" client_port=") + to_string(transport->rtp.u.client_port1) +"-"+ to_string(transport->rtp.u.client_port2);
				rtspClientInfo.info += string(" server_port=") + to_string(port[0]) +"-"+ to_string(port[1]) + " ";
			}
			else{
				RtspClientInfo rtspClientInfo;
				rtspClientInfo.name = it->first +" "+ _info_cliip+":"+to_string(_info_cliPort);
				rtspClientInfo.info = _info_trackId + string(" client_port=") + to_string(transport->rtp.u.client_port1) +"-"+ to_string(transport->rtp.u.client_port2);
				rtspClientInfo.info += string(" server_port=") + to_string(port[0]) +"-"+ to_string(port[1]) + " ";
				srvInfo.sourceInfo[_info_sourceId].playerInfo[it->first] = rtspClientInfo;
			}

			LOG_RTSP_DBG("rtspClientInfo new %s: %s\n", srvInfo.sourceInfo[_info_sourceId].playerInfo[it->first].name.c_str(), \
			srvInfo.sourceInfo[_info_sourceId].playerInfo[it->first].info.c_str());
		}

	}

    return rtsp_server_reply_setup(rtsp, 200, it->first.c_str(), rtsp_transport);
}

int RtspSrv::onplay(void* /*ptr*/, rtsp_server_t* rtsp, const char* uri, const char* session, const int64_t *npt, const double *scale){
	std::shared_ptr<IMediaSource> source;
	TSessions::iterator it;
	{
		AutoThreadLocker locker(s_locker);
		it = s_sessions.find(session ? session : "");
		if(it == s_sessions.end())
		{
			// 454 Session Not Found
			return rtsp_server_reply_play(rtsp, 454, NULL, NULL, NULL);
		}
		else
		{
			// uri with track
			if (0)
			{
				// 460 Only aggregate operation allowed
				return rtsp_server_reply_play(rtsp, 460, NULL, NULL, NULL);
			}
		}

		source = it->second.media;
	}
	if(npt && 0 != source->Seek(*npt))
	{
		// 457 Invalid Range
		return rtsp_server_reply_play(rtsp, 457, NULL, NULL, NULL);
	}

	if(scale && 0 != source->SetSpeed(*scale))
	{
		// set speed
		assert(*scale > 0);

		// 406 Not Acceptable
		return rtsp_server_reply_play(rtsp, 406, NULL, NULL, NULL);
	}

	// RFC 2326 12.33 RTP-Info (p55)
	// 1. Indicates the RTP timestamp corresponding to the time value in the Range response header.
	// 2. A mapping from RTP timestamps to NTP timestamps (wall clock) is available via RTCP.
	char rtpinfo[512] = { 0 };
	source->GetRTPInfo(uri, rtpinfo, sizeof(rtpinfo));

	// for vlc 2.2.2
	MP4FileSource* mp4 = dynamic_cast<MP4FileSource*>(source.get());
	if(mp4)
		mp4->SendRTCP(system_clock());

	it->second.status = 1;
    return rtsp_server_reply_play(rtsp, 200, npt, NULL, rtpinfo);
}

int RtspSrv::onpause(void* /*ptr*/, rtsp_server_t* rtsp, const char* /*uri*/, const char* session, const int64_t* /*npt*/){
	LOG_RTSP_DBG("rtsp server on pause\n");
	std::shared_ptr<IMediaSource> source;
	TSessions::iterator it;
	{
		AutoThreadLocker locker(s_locker);
		it = s_sessions.find(session ? session : "");
		if(it == s_sessions.end())
		{
			// 454 Session Not Found
			return rtsp_server_reply_pause(rtsp, 454);
		}
		else
		{
			// uri with track
			if (0)
			{
				// 460 Only aggregate operation allowed
				return rtsp_server_reply_pause(rtsp, 460);
			}
		}

		source = it->second.media;
		it->second.status = 2;
	}

	source->Pause();

	// 457 Invalid Range

    return rtsp_server_reply_pause(rtsp, 200);
}

int RtspSrv::onteardown(void* /*ptr*/, rtsp_server_t* rtsp, const char* uri, const char* session)
{
	LOG_INFO("rtsp server on teardown session: %s %s\n", uri, session);
	std::shared_ptr<IMediaSource> source;
	TSessions::iterator it;
	{
		AutoThreadLocker locker(s_locker);
		std::map<std::string, RtspClientInfo>::iterator infoIter;
		// infoIter = srvInfo.sourceInfo.find(session);
		// if(infoIter != srvInfo.sourceInfo.end()){
		// 	srvInfo.sourceInfo.erase(infoIter);
		// }
		string _info_filename, _info_prefix;
		rtsp_get_filename_from_uri(uri, _info_filename, _info_prefix);
		string _info_sourceId = _info_prefix + _info_filename;
		
		std::map<std::string, RtspSourceInfo>::iterator sourceInfoIt = srvInfo.sourceInfo.find(_info_sourceId);
		if(sourceInfoIt == srvInfo.sourceInfo.end()){
			LOG_RTSP_ERR("err not find sourceInfo %s\n", _info_sourceId.c_str());
		}
		else{
			std::map<std::string, RtspClientInfo>::iterator rtspClientInfoIt;
			rtspClientInfoIt = srvInfo.sourceInfo[_info_sourceId].playerInfo.find(session);
			if(rtspClientInfoIt != srvInfo.sourceInfo[_info_sourceId].playerInfo.end()){
				srvInfo.sourceInfo[_info_sourceId].playerInfo.erase(rtspClientInfoIt);
			}
			else{
				LOG_RTSP_ERR("err not find session  %s\n", session);
			}
		}



		it = s_sessions.find(session ? session : "");
		if(it == s_sessions.end())
		{
			// 454 Session Not Found
			return rtsp_server_reply_teardown(rtsp, 454);
		}

		source = it->second.media;
		s_sessions.erase(it);
	}

	return rtsp_server_reply_teardown(rtsp, 200);
}

int RtspSrv::onannounce(void* /*ptr*/, rtsp_server_t* rtsp, const char* uri, const char* sdp){
	return rtsp_server_reply_announce(rtsp, 200);
}

int RtspSrv::onrecord(void* /*ptr*/, rtsp_server_t* rtsp, const char* uri, const char* session, const int64_t *npt, const double *scale){
	return rtsp_server_reply_record(rtsp, 200, NULL, NULL);
}

int RtspSrv::onoptions(void* ptr, rtsp_server_t* rtsp, const char* uri){
	const char* require = rtsp_server_get_header(rtsp, "Require");
	if(0){
		LOG_RTSP_DBG("require=%s\n", require);
	}	
	return rtsp_server_reply_options(rtsp, 200);
}

int RtspSrv::ongetparameter(void* ptr, rtsp_server_t* rtsp, const char* uri, const char* session, const void* content, int bytes){
	const char* ctype = rtsp_server_get_header(rtsp, "Content-Type");
	const char* encoding = rtsp_server_get_header(rtsp, "Content-Encoding");
	const char* language = rtsp_server_get_header(rtsp, "Content-Language");
	if(0){
		LOG_RTSP_DBG("ctype=%s,%s,%s\n", ctype, encoding, language);
	}	
	return rtsp_server_reply_get_parameter(rtsp, 200, NULL, 0);
}

int RtspSrv::onsetparameter(void* ptr, rtsp_server_t* rtsp, const char* uri, const char* session, const void* content, int bytes){
	const char* ctype = rtsp_server_get_header(rtsp, "Content-Type");
	const char* encoding = rtsp_server_get_header(rtsp, "Content-Encoding");
	const char* language = rtsp_server_get_header(rtsp, "Content-Language");
	if(0){
		LOG_RTSP_DBG("ctype=%s,%s,%s\n", ctype, encoding, language);
	}
	return rtsp_server_reply_set_parameter(rtsp, 200);
}

int RtspSrv::rtsp_onclose(void* /*ptr2*/){
	// TODO: notify rtsp connection lost
	//       start a timer to check rtp/rtcp activity
	//       close rtsp media session on expired
	printf("rtsp close\n");
	return 0;
}

void RtspSrv::rtsp_onerror(void* /*param*/, rtsp_server_t* rtsp, int code){
	LOG_ERR("rtsp_onerror code=%d, rtsp=%p\n", code, rtsp);

	TSessions::iterator it;
	AutoThreadLocker locker(s_locker);
	for (it = s_sessions.begin(); it != s_sessions.end(); ++it){
		if (rtsp == it->second.rtsp){
			it->second.media->Pause();
			s_sessions.erase(it);
			LOG_ERR("s_sessions %s\n", it->first.c_str());
			break;
		}
	}
    //return 0;
}

static int rtsp_ondescribe(void* /*ptr*/, rtsp_server_t* rtsp, const char* uri){
	return g_rtspSrv->ondescribe(nullptr, rtsp, uri);
}

static int rtsp_onsetup(void* /*ptr*/, rtsp_server_t* rtsp, const char* uri, const char* session, const struct rtsp_header_transport_t transports[], size_t num){
	return g_rtspSrv->onsetup(nullptr, rtsp, uri, session, transports, num);
}

static int rtsp_onplay(void* /*ptr*/, rtsp_server_t* rtsp, const char* uri, const char* session, const int64_t *npt, const double *scale){
	return g_rtspSrv->onplay(nullptr, rtsp, uri, session, npt, scale);
}

static int rtsp_onpause(void* /*ptr*/, rtsp_server_t* rtsp, const char* /*uri*/, const char* session, const int64_t* /*npt*/){
	return g_rtspSrv->onpause(nullptr, rtsp, nullptr, session, nullptr);
}

static int rtsp_onteardown(void* /*ptr*/, rtsp_server_t* rtsp, const char* uri, const char* session){
	return g_rtspSrv->onteardown(nullptr, rtsp, uri, session);
}

static int rtsp_onannounce(void* /*ptr*/, rtsp_server_t* rtsp, const char* uri, const char* sdp){
    return g_rtspSrv->onannounce(nullptr, rtsp, uri, sdp);
}

static int rtsp_onrecord(void* /*ptr*/, rtsp_server_t* rtsp, const char* uri, const char* session, const int64_t *npt, const double *scale){
    return g_rtspSrv->onrecord(nullptr, rtsp, uri, session, npt, scale);
}

static int rtsp_onoptions(void* ptr, rtsp_server_t* rtsp, const char* uri){
	return g_rtspSrv->onoptions(ptr, rtsp, uri);
}

static int rtsp_ongetparameter(void* ptr, rtsp_server_t* rtsp, const char* uri, const char* session, const void* content, int bytes){
	return g_rtspSrv->ongetparameter(ptr, rtsp, uri, session, content, bytes);
}

static int rtsp_onsetparameter(void* ptr, rtsp_server_t* rtsp, const char* uri, const char* session, const void* content, int bytes){
	return g_rtspSrv->onsetparameter(ptr, rtsp, uri, session, content, bytes);
}

static int rtsp_onclose(void* /*ptr2*/){
	return g_rtspSrv->rtsp_onclose(nullptr);
}

static void rtsp_onerror(void* /*param*/, rtsp_server_t* rtsp, int code){
	return g_rtspSrv->rtsp_onerror(nullptr, rtsp, code);
}

rtsp_server_listen_t* rtsp_server_create(cor_netfd_t srvFd, void* param){
	rtsp_server_listen_t* p;
	//	void* udp = rtsp_transport_udp_create(NULL, 554, &handler, NULL); assert(udp);
	struct aio_rtsp_handler_t handler;
	memset(&handler, 0, sizeof(handler));
	handler.base.ondescribe = rtsp_ondescribe;
    handler.base.onsetup = rtsp_onsetup;
    handler.base.onplay = rtsp_onplay;
    handler.base.onpause = rtsp_onpause;
    handler.base.onteardown = rtsp_onteardown;
	handler.base.close = rtsp_onclose;
    handler.base.onannounce = rtsp_onannounce;
    handler.base.onrecord = rtsp_onrecord;
	handler.base.onoptions = rtsp_onoptions;
	handler.base.ongetparameter = rtsp_ongetparameter;
	handler.base.onsetparameter = rtsp_onsetparameter;
//	handler.base.send; // ignore
	handler.onerror = rtsp_onerror;

	p = (struct rtsp_server_listen_t*)calloc(1, sizeof(*p));
	if (!p){
		return NULL;
	}

	p->param = param;
	memcpy(&p->handler, &handler, sizeof(p->handler));
	
    return p;
}

void rtsp_server_destroy(rtsp_server_listen_t* srv){
	free(srv);
}

static void rtsp_session_ondestroy(void* param)
{
	struct rtsp_session_t *session;
	session = (struct rtsp_session_t *)param;

	// user call rtsp_server_destroy
	if (session->rtsp)
	{
		rtsp_server_destroy(session->rtsp);
		session->rtsp = NULL;
	}

	if (session->rtp.data)
	{
		assert(session->rtp.capacity > 0);
		free(session->rtp.data);
		session->rtp.data = NULL;
		session->rtp.capacity = 0;
	}

#if defined(_DEBUG) || defined(DEBUG)
	memset(session, 0xCC, sizeof(*session));
#endif
	free(session);
}

static int rtsp_session_send(void *_session, const void* data, size_t bytes){
	rtsp_session_t* session = (rtsp_session_t*)_session;
	int ire = cor_write(session->cliNetFd, data, bytes, 5*1000);
	return ire;
}

//@return 0-ok, 1-need more data, other-error
static int rtsp_session_onrecv(rtsp_session_t* session, size_t bytes){
	size_t remain;
	const uint8_t* p, *end;
	if (0 == bytes){
		return ECONNRESET;
	}
	int ire =0;
	{
		p = session->buffer;
		end = session->buffer + bytes;
		do{
			if (0 == session->rtsp_need_more_data && ('$' == *p || 0 != session->rtp.state)){
				p = rtp_over_rtsp(&session->rtp, p, end);
			}
			else{
				remain = end - p;
				//rtsp_server_input @return 0-ok, 1-need more data, other-error
				ire = rtsp_server_input(session->rtsp, p, &remain);
				session->rtsp_need_more_data = ire;
				if (0 == ire)
				{
					// TODO: pipeline remain data
					assert(bytes > remain);
					assert(0 == remain || '$' == *(end - remain));
				}
				p = end - remain;
			}
		} while (p < end && 0 == ire);
		
		if (ire >= 0){
			// LOG_DBG("need mor data ? %d\n", ire);
			return 1;
			// need more data
			// code = aio_transport_recv(session->aio, session->buffer, sizeof(session->buffer));
		}
	}
	return ire;
}

void RtspSrv::handle_session_thread(void* arg){
    rtsp_session_t *session  = (rtsp_session_t*)arg;
	CorRe tmpre;
    struct sockaddr_in* from = (struct sockaddr_in*)cor_netfd_getspecific(session->cliNetFd);
    int processRe=0;
	int ire = 0;
    #define RECV_MAX_LEN sizeof(session->buffer)
    uint8_t* packet = session->buffer; 
    while (true){
		packet = session->buffer;
        ire = cor_read(session->cliNetFd, packet, RECV_MAX_LEN, COR_UTIME_NO_TIMEOUT); 
        if ( ire < 0) {
            int errnoSave = errno;
            if(errnoSave == 104){
                // hostInfo->leaveCount++;//对端关闭不算错误
            }else{
                // hostInfo->errCount++;
            }
            LOG_ERR("can't read request from %s:%d: cor_read [%d]%s\n", inet_ntoa((*from).sin_addr), (*from).sin_port, errnoSave, getErrnoDesc(errnoSave));
            break;
        }
        if ( ire == 0) {
            tmpre = CORRE_OK_DESC(string(inet_ntoa((*from).sin_addr)) + to_string((*from).sin_port) + "disconnect");
            break;
        }
		char* tmpBuf = new char[sizeof(session->buffer)];
		memset(tmpBuf, 0, ire+5);
		snprintf(tmpBuf, ire, "%s", packet);
		// LOG_DBG("bytes %d ================\n%s\n\n", ire, tmpBuf);
		delete tmpBuf;
		processRe = rtsp_session_onrecv(session, ire);
		// LOG_DBG("on recv processRe=%d\n", processRe);
		// if( (0!=processRe) || (1!=processRe) ){
		// 	break;
		// }
    }
	session->onerror(session->param, session->rtsp, processRe ? processRe : ECONNRESET);
    rtsp_session_ondestroy(session);
	LOG_WARN("session exit processRe=%d, %d\n", processRe, ire);
}

void RtspSrv::handle_session(cor_netfd_t _cliSock){
	char ip[65];
	unsigned short port;	
	struct rtsp_handler_t rtsphandler;

	memcpy(&rtsphandler, &(sg_rtsp_server_listen->handler.base), sizeof(rtsphandler));
	rtsphandler.send = rtsp_session_send;

	rtsp_session_t *session = new rtsp_session_t;
	memset(session, 0, sizeof(rtsp_session_t));
	if (!session){
		fatal("calloc session err\n");
	}
	session->cliNetFd = _cliSock;
	
	struct sockaddr_in* from = (struct sockaddr_in*)cor_netfd_getspecific(session->cliNetFd);
	socklen_t addrlen = sizeof(struct sockaddr_in);
	cor_addr_to((sockaddr*)from, addrlen, ip, &port);
	LOG_DBG("cor_addr_to %s, %d\n", ip, port);

	assert(addrlen <= sizeof(session->addr));
	session->addrlen = addrlen < sizeof(session->addr) ? addrlen : sizeof(session->addr);
	memcpy(&session->addr, from, session->addrlen); // save client ip/port

	session->param = NULL;
	session->onerror = sg_rtsp_server_listen->handler.onerror;
	session->rtsp = rtsp_server_create(ip, port, &rtsphandler, session->param, session); // reuse-able, don't need create in every link
	if (!session->rtsp ){
		rtsp_session_ondestroy(session);
		fatal("rtsp server create err\n");
	}
	
	session->rtp.param = session->param;
	session->rtp.onrtp = sg_rtsp_server_listen->handler.onrtp;

    CorThread* ctdCli = new CorThread("cli session", std::bind(&RtspSrv::handle_session_thread,this,std::placeholders::_1), session);
    usr_assert(ctdCli->start());
}

void RtspSrv::accept_cor_thread(void* arg) {
    struct sockaddr_in from;
    int fromlen;
    fromlen = sizeof(from);
    while (1) {
        cor_netfd_t cli_nfd = cor_accept(corSrvfd, (struct sockaddr*)&from, &fromlen, COR_UTIME_NO_TIMEOUT);
        if (cli_nfd == NULL) {
            int errNoSave = errno;
            LOG_ERR("can't accept connection: %s \n", getErrnoDesc(errNoSave));
            continue;
        }
        /* Save peer address, so we can retrieve it later */
        cor_netfd_setspecific(cli_nfd, &from, nullptr);
        handle_session(cli_nfd);
    }
    cor_netfd_close(corSrvfd);    
    rtsp_server_destroy(sg_rtsp_server_listen);
	//	rtsp_transport_udp_destroy(udp);
    LOG_INFO("accept exit\n");
}

void RtspSrv::mediaSource_cor_thread(void* arg) {
    //[todo] 下面这些需要处理
	// test only
    while(1){
		cor_usleep(5*1000);
		TSessions::iterator it;
		AutoThreadLocker locker(s_locker);
		for(it = s_sessions.begin(); it != s_sessions.end(); ++it)
		{
			rtsp_media_t &session = it->second;
			if(1 == session.status)
				session.media->Play();
		}
		// TODO: check rtsp session activity
    }
}

bool RtspSrv::start(const char* ip, int port){

    string srvIp; uint16_t srvPort;
        if(nullptr == ip
        || string("localhost") == ip)   
            srvIp = "0.0.0.0";
        else
            srvIp = ip;
        srvPort = port;
        LOG_INFO("rtsp server start %s:%d\n", srvIp.c_str(), srvPort);
	
	corSrvfd = cor_tcp_listen_ipv4(srvIp.c_str(), srvPort, rtspSrvCfg.listenQSize==-1 ? LISTENQ_SIZE_DEFAULT : rtspSrvCfg.listenQSize);
	if (COR_NETFD_ERR == corSrvfd){
		LOG_ERR("listen err\n");
		return false;
	}
	sg_rtsp_server_listen = rtsp_server_create(corSrvfd, nullptr);
	usr_assert(NULL != sg_rtsp_server_listen);

    CorThread* ctdAccept = new CorThread("accept coroutine", std::bind(&RtspSrv::accept_cor_thread, this, std::placeholders::_1), nullptr);
    ctdAccept->start();

    CorThread* ctdstatisc = new CorThread("accept coroutine", std::bind(&RtspSrv::statisc_cor_thread, this, std::placeholders::_1), nullptr);
    ctdstatisc->start();

    CorThread* ctdmediaSource = new CorThread("accept coroutine", std::bind(&RtspSrv::mediaSource_cor_thread, this, std::placeholders::_1), nullptr);
    ctdmediaSource->start();

    return true;
}

void RtspSrv::statisc_info(){

}

void RtspSrv::statisc_cor_thread(void* arg) {
	while (1){
		cor_sleep(50);
		statisc_info();
	}
}

