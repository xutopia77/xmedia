
#ifndef _RTSPSRV_H
#define _RTSPSRV_H

#include <CorThread.h>
#include <AppCfg.h>
#include "rtsp-server-aio.h"
#include "sys/sync.hpp"
#include "media/mp4-file-source.h"

#include <map>
#include <memory>

typedef struct rtsp_server_listen_t{
	// void* aio;
	void* param; //在 srv中传入的是NULL
	struct aio_rtsp_handler_t handler;
}rtsp_server_listen_t;


struct rtsp_media_t
{
	std::shared_ptr<IMediaSource> media;
	std::shared_ptr<IRTPTransport> transport;
	uint8_t channel; // rtp over rtsp interleaved channel
	int status; // setup-init, 1-play, 2-pause
	rtsp_server_t* rtsp;
};
typedef std::map<std::string, rtsp_media_t> TSessions;
static TSessions s_sessions;

struct TFileDescription
{
	int64_t duration;
	std::string sdpmedia;
};

struct RtspClientInfo{
    std::string name;
    std::string info="";
};

struct RtspSourceInfo{
    std::string name;
    double duration=0;
    std::map<std::string, RtspClientInfo> playerInfo;
};

struct RtspSrvInfo{
    std::string hostName = "rtspServer";
    size_t rxBytes=0;
    size_t txBytes = 0;
    std::map<std::string, RtspSourceInfo> sourceInfo;
};

class RtspSrv
{
private:
    AppCfg::RtspSrvCfg srvCfg;
    cor_netfd_t corSrvfd;
    AppCfg::RtspSrvCfg rtspSrvCfg;
    rtsp_server_listen_t* sg_rtsp_server_listen = NULL;
    RtspSrvInfo srvInfo;
    ThreadLocker s_locker;
public:
    void accept_cor_thread(void* arg);
    void statisc_info();
    void statisc_cor_thread(void* arg);
    void mediaSource_cor_thread(void* arg);
    bool start(const char* ip, int port);
    void handle_session_thread(void* arg);
    void handle_session(cor_netfd_t cliSock);  
    
    int ondescribe(     void* /*ptr*/, rtsp_server_t* rtsp, const char* uri);
    int onsetup(        void* /*ptr*/, rtsp_server_t* rtsp, const char* uri, const char* session, const struct rtsp_header_transport_t transports[], size_t num);
    int onplay(         void* /*ptr*/, rtsp_server_t* rtsp, const char* uri, const char* session, const int64_t *npt, const double *scale);    
    int onpause(        void* /*ptr*/, rtsp_server_t* rtsp, const char* /*uri*/, const char* session, const int64_t* /*npt*/);
    int onteardown(     void* /*ptr*/, rtsp_server_t* rtsp, const char* /*uri*/, const char* session);
    int onannounce(     void* /*ptr*/, rtsp_server_t* rtsp, const char* uri, const char* sdp);
    int onrecord(       void* /*ptr*/, rtsp_server_t* rtsp, const char* uri, const char* session, const int64_t *npt, const double *scale);
    int onoptions(      void* ptr, rtsp_server_t* rtsp, const char* uri);
    int ongetparameter( void* ptr, rtsp_server_t* rtsp, const char* uri, const char* session, const void* content, int bytes);
    int onsetparameter( void* ptr, rtsp_server_t* rtsp, const char* uri, const char* session, const void* content, int bytes);
    int rtsp_onclose(   void* /*ptr2*/);
    void rtsp_onerror(  void* /*param*/, rtsp_server_t* rtsp, int code);

    RtspSrv();
    ~RtspSrv();
    void setCfg(AppCfg::RtspSrvCfg& cfg){srvCfg = cfg;}
    RtspSrvInfo& getSrvInfo(){return srvInfo;}
};



#endif
