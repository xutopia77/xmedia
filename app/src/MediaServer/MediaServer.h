
#ifndef _MediaServer_H
#define _MediaServer_H

#include "RtmpSrv.h"
#include "RtspSrv/RtspSrv.h"
#include <memory>
#include "xlog.h"
#include "AppCfg.h"

class MediaServer
{
private:
    std::shared_ptr<RtmpSrv> pRtmpSrv;
    std::shared_ptr<RtspSrv> pRtspSrv;

public:
    MediaServer();
    ~MediaServer();
    RtmpSrvInfo& getRtmpSrvInfo(){return pRtmpSrv->getSrvInfo();}
    RtspSrvInfo& getRtspSrvInfo(){return pRtspSrv->getSrvInfo();}
    bool start();
};

#endif
