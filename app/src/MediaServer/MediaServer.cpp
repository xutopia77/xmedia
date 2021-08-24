
#include "MediaServer.h"

#include "xlog.h"
#include <string>
#include "CorThread.h"

using namespace std;

#include "AppCfg.h"

MediaServer::MediaServer()
{
}

MediaServer::~MediaServer()
{
}

shared_ptr<RtmpSrv> g_rtmpSrv = nullptr;
shared_ptr<RtspSrv> g_rtspSrv = nullptr;

void start_rtmp_srv(std::shared_ptr<RtmpSrv>& pRtmpSrv){
    pRtmpSrv = make_shared<RtmpSrv>();
    g_rtmpSrv = pRtmpSrv;
    pRtmpSrv->setCfg(g_appCfg.rtmpSrvCfg);
    pRtmpSrv->start(g_appCfg.rtmpSrvCfg.ip.c_str(), g_appCfg.rtmpSrvCfg.port);
}

void start_rtsp_srv(std::shared_ptr<RtspSrv>& pRtspSrv){
    pRtspSrv = make_shared<RtspSrv>();
    g_rtspSrv = pRtspSrv;
    pRtspSrv->setCfg(g_appCfg.rtspSrvCfg);
    pRtspSrv->start(g_appCfg.rtspSrvCfg.ip.c_str(), g_appCfg.rtspSrvCfg.port);
}

bool MediaServer::start(){

    if(g_appCfg.mediaSrvCfg.isStartRtmpSrv){
        start_rtmp_srv(pRtmpSrv);
    }

    if(g_appCfg.mediaSrvCfg.isStartRtspSrv){
        start_rtsp_srv(pRtspSrv);
    }

    return true;
}
