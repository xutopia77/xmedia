
#ifndef _RtmpSrv_H
#define _RtmpSrv_H

#include "sockutil.h"
#include "flv-writer.h"
#include "flv-proto.h"
#include "flv-muxer.h"
#include "flv-demuxer.h"

#include "rtmp-server.h"
#include "sys/sync.hpp"

#include "CorThread.h"

#include <list>
#include <string>
#include <map>
#include <memory>

#include "RtmpSource.h"

#include "AppCfg.h"

struct RtmpPlayerInfo{
    std::string name;
};

struct RtmpSourceInfo{
    std::string srvName;
    bool bRecord;
    uint32_t rxVideoTime=0; 
    uint32_t rxVideoBytes=0; 
    int64_t rxVideoStartTime=-1;//must be -1

    uint32_t rxAudioTime=0; 
    uint32_t rxAudioBytes=0; 
    int64_t rxAudioStartTime=-1;//must be -1
    std::map<std::string, RtmpPlayerInfo> playerInfo;
};

struct RtmpSrvInfo{
    std::string hostName = "rtmpServer";
    size_t rxBytes=0;
    size_t txBytes = 0;
    std::map<std::string, RtmpSourceInfo> sourceInfo;
};


class RtmpSrv
{
private:
    rtmp_server_t* rtmpSrv;
    ThreadLocker sourceLocker;
    std::map<std::string, std::shared_ptr<RtmpSource> > sourceLives;
    RtmpSrvInfo srvInfo;
    cor_netfd_t corSrvfd;
    AppCfg::RtmpSrvCfg srvCfg; 
public:
    RtmpSrv();
    ~RtmpSrv();
    void accept_cor_thread(void* arg);
    void handle_session_thread(void* arg);
    void handle_session(cor_netfd_t cliSock);
    void statiscInfo();
    void statisc_cor_thread(void* arg);

    void setCfg(AppCfg::RtmpSrvCfg& cfg){srvCfg = cfg;}
    bool start(const char* ip, int port);

    int onpublish(  rtmp_session_t* session, const char* app,       const char* stream, const char* type);
    int onscript(   rtmp_session_t* session, const void* script,    size_t bytes, uint32_t timestamp);
    int onvideo(    rtmp_session_t* session, const void* data,      size_t bytes, uint32_t timestamp);
    int onaudio(    rtmp_session_t* session, const void* data,      size_t bytes, uint32_t timestamp);
    CorRe onclose(   rtmp_session_t* session);
    int onplay(     rtmp_session_t* session, const char* app, const char* stream, double start, double duration, uint8_t reset);

    RtmpSrvInfo& getSrvInfo(){return srvInfo;}

};

#endif
