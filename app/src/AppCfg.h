
#ifndef _APPUTIL_H
#define _APPUTIL_H

#include "CorCfg.h"

#include <string>
#include <vector>

// #define CREATE_APP_TAG(a) APP_##a #a


#define APP_http_test           "http_test"
#define APP_http_server_test    "http_server_test"
#define APP_corcfg_test         "corcfg_test"
#define APP_unitestMisc         "unitestMisc"
#define APP_parseMainArgvTest   "parseMainArgvTest"

#define APP_rtsp_server         "rtsp_server"
#define APP_rtsp_client         "rtsp_client"

// rtsp测试群
#define APP_rtsp                "rtsp"
// rtmp测试群
#define APP_rtmp                "rtmp"
// 主应用程序
#define APP_server              "server"

#define APP_rtsp_server         "rtsp_server"
#define APP_rtsp_client         "rtsp_client"

#define APP_rtmp_server         "rtmp_server"
#define APP_rtmp_play           "rtmp_play"
#define APP_rtmp_push           "rtmp_push"

// #define CREATE_APP_TAG(test)

#define LISTENQ_SIZE_DEFAULT 256

struct AppCfg{
    std::string funcName="nullptr";
    enum logLevelE logLevel = LOG_LVL_INFO;
    int HostDumpCount       = 0;
    bool isDaemon           = false;
    std::string datInPath, datOutPath, inH264AacFlv;
    struct MediaSrvCfg{
        bool isStartRtspSrv = false;
        bool isStartRtmpSrv = false;
    }mediaSrvCfg;

    struct HttpSrvCfg{
        std::string ip;
        int port = 8000;
        int listenQSize = -1;
    }httpSrvCfg;

    struct RtmpSrvCfg{
        std::string ip;
        int port=1935;
        int listenQSize = -1;
        //0不打印，1 全部打印
        int staticPrtLevel = 1;
    }rtmpSrvCfg;
    
    struct RtmpPusherCfg{
        std::string pushHost, pushApp, pushStream, pushOriginFile;
    }rtmpPusherCfg;
    struct RtmpPlayerCfg{
        std::string playerHost, playerApp, playerStream, playerRecordFile;
        bool bRecord=false;
    }rtmpPlayerCfg;

    struct RtspSrvCfg{
        std::string ip;
        int port=8554;
        int listenQSize = -1;
        //0不打印，1 全部打印
        int staticPrtLevel = 1;
    }rtspSrvCfg;
    struct RtspCliCfg{
        std::string ip;
        std::string file;
        std::string datSavePath = "./../dat/out/rtspRecvDat";
    }rtspCliCfg;

};
extern AppCfg g_appCfg;

#endif
