
#include "AppCfg.h"
#include "CorCfg.h"
#include "xlog.h"
#include <string>
#include <map>
#include "parseMainArgv.h"

using namespace std;

AppCfg g_appCfg;

bool getMediaMiscCfg(AppCfg& cfg){
    CorRe re;
    string cfgstrValTmp;
    if(!CORRE_IS_OK(re=corcfg_getStr("logLevel", cfgstrValTmp))){
        LOG_ERR("get cfg logLevel err %s\n", re.desc().c_str());
    }else{
        map<string, enum logLevelE> logLevelMap;
        logLevelMap["NONE"] = LOG_LVL_NONE;
        logLevelMap["ERR" ] = LOG_LVL_ERR;
        logLevelMap["WARN"] = LOG_LVL_WARN;
        logLevelMap["INFO"] = LOG_LVL_INFO;
        logLevelMap["DBG" ] = LOG_LVL_DBG;
        cfg.logLevel = logLevelMap[cfgstrValTmp];
        log_setLogLevel(cfg.logLevel);
    }
    usr_assert(CORRE_IS_OK(corcfg_getStr("logTank", cfgstrValTmp)));
    if( cfgstrValTmp == "logFile" ){
        LOG_INFO("log file %s\n", "trance.log");
        log_setLogWay((const char*)"trance.logg", LOG_WAY_BOTH);
    }
    usr_assert(CORRE_IS_OK(corcfg_getStr("daemon", cfgstrValTmp)));
        cfg.isDaemon = cfgstrValTmp == "on" ? true : false;

    usr_assert(CORRE_IS_OK(corcfg_getStr("datInPath",       g_appCfg.datInPath)));
    usr_assert(CORRE_IS_OK(corcfg_getStr("datOutPath",      g_appCfg.datOutPath)));
    usr_assert(CORRE_IS_OK(corcfg_getStr("inH264AacFlv",    g_appCfg.inH264AacFlv)));

    return true;
}

bool getMediaSrvCfg(AppCfg::MediaSrvCfg& cfg){
    usr_assert(CORRE_IS_OK(corcfg_getBool("mediaSrvCfg.isStartRtmpSrv", cfg.isStartRtmpSrv)));
    usr_assert(CORRE_IS_OK(corcfg_getBool("mediaSrvCfg.isStartRtspSrv", cfg.isStartRtspSrv)));
    return true;
}

bool getRtspSrvCfg(AppCfg::RtspSrvCfg& rtspSrvCfg){
    usr_assert(CORRE_IS_OK(corcfg_getStr("rtspSrvCfg.ip",           rtspSrvCfg.ip)));
    usr_assert(CORRE_IS_OK(corcfg_getInt("rtspSrvCfg.port",         rtspSrvCfg.port)));
    usr_assert(CORRE_IS_OK(corcfg_getInt("rtspSrvCfg.listenQSize",  rtspSrvCfg.listenQSize)));
    return true;
}

bool getHttpSrvCfg(AppCfg::HttpSrvCfg& httpSrvCfg){
    usr_assert(CORRE_IS_OK(corcfg_getStr("httpSrvCfg.ip",           httpSrvCfg.ip)));
    usr_assert(CORRE_IS_OK(corcfg_getInt("httpSrvCfg.port",         httpSrvCfg.port)));
    usr_assert(CORRE_IS_OK(corcfg_getInt("httpSrvCfg.listenQSize",  httpSrvCfg.listenQSize)));
    return true;
}

bool getRtmpSrvCfg(AppCfg::RtmpSrvCfg& rtmpSrvCfg){
    usr_assert(CORRE_IS_OK(corcfg_getStr("rtmpSrvCfg.ip",               rtmpSrvCfg.ip)));
    usr_assert(CORRE_IS_OK(corcfg_getInt("rtmpSrvCfg.port",             rtmpSrvCfg.port)));
    usr_assert(CORRE_IS_OK(corcfg_getInt("rtmpSrvCfg.listenQSize",      rtmpSrvCfg.listenQSize)));
    usr_assert(CORRE_IS_OK(corcfg_getInt("rtmpSrvCfg.staticPrtLevel",   rtmpSrvCfg.staticPrtLevel)));
    return true;
}

bool getRtmpPusherCfg(AppCfg::RtmpPusherCfg& rtmpPusherCfg){
    usr_assert(CORRE_IS_OK(corcfg_getStr("rtmpPusherCfg.pushHost",          rtmpPusherCfg.pushHost)));
    usr_assert(CORRE_IS_OK(corcfg_getStr("rtmpPusherCfg.pushApp",           rtmpPusherCfg.pushApp)));
    usr_assert(CORRE_IS_OK(corcfg_getStr("rtmpPusherCfg.pushStream",        rtmpPusherCfg.pushStream)));
    usr_assert(CORRE_IS_OK(corcfg_getStr("rtmpPusherCfg.pushOriginFile",    rtmpPusherCfg.pushOriginFile)));
    return true;
}

bool getRtmpPlayerCfg(AppCfg::RtmpPlayerCfg& rtmpPlayerCfg){
    usr_assert(CORRE_IS_OK(corcfg_getStr("rtmpPlayerCfg.playerHost",          rtmpPlayerCfg.playerHost)));
    usr_assert(CORRE_IS_OK(corcfg_getStr("rtmpPlayerCfg.playerApp",           rtmpPlayerCfg.playerApp)));
    usr_assert(CORRE_IS_OK(corcfg_getStr("rtmpPlayerCfg.playerStream",        rtmpPlayerCfg.playerStream)));
    usr_assert(CORRE_IS_OK(corcfg_getBool("rtmpPlayerCfg.bRecord",            rtmpPlayerCfg.bRecord)));
    return true;
}

bool getCfgServer(AppCfg& g_appCfg){
    usr_assert(getMediaSrvCfg(g_appCfg.mediaSrvCfg));
    usr_assert(getRtspSrvCfg(g_appCfg.rtspSrvCfg));
    usr_assert(getHttpSrvCfg(g_appCfg.httpSrvCfg));
    usr_assert(getRtmpSrvCfg(g_appCfg.rtmpSrvCfg));
    usr_assert(getRtmpPusherCfg(g_appCfg.rtmpPusherCfg));
    usr_assert(getRtmpPlayerCfg(g_appCfg.rtmpPlayerCfg));

    return true;
}

bool getCfg(const char* cfgPath){
    CorRe re;
    if(!CORRE_IS_OK(re=corcfg_readFile(cfgPath))){
        LOG_ERR("read cfg file err %s\n", re.desc().c_str());
        return false;
    }

    usr_assert(getMediaMiscCfg(g_appCfg));

    if(APP_server == g_appCfg.funcName){
        getCfgServer(g_appCfg);
    }

    if( (APP_rtsp_server==g_appCfg.funcName) ){
        usr_assert(getRtspSrvCfg(g_appCfg.rtspSrvCfg));
        correct_cfg_str('h', g_appCfg.rtspSrvCfg.ip);
        correct_cfg_int('p', g_appCfg.rtspSrvCfg.port);
        g_appCfg.mediaSrvCfg.isStartRtspSrv = true;
    }

    if(g_appCfg.funcName == APP_rtsp_client){
        correct_cfg_str('h', g_appCfg.rtspCliCfg.ip);
        correct_cfg_str('w', g_appCfg.rtspCliCfg.file);
    }

    if(g_appCfg.funcName == APP_rtmp_server){
        usr_assert(getRtmpSrvCfg(g_appCfg.rtmpSrvCfg));
        g_appCfg.mediaSrvCfg.isStartRtmpSrv = true;
    }

    if(g_appCfg.funcName == APP_rtmp_play){
        usr_assert(getRtmpPlayerCfg(g_appCfg.rtmpPlayerCfg));
        correct_cfg_str('h', g_appCfg.rtmpPlayerCfg.playerHost);
        correct_cfg_str('a', g_appCfg.rtmpPlayerCfg.playerApp);
        correct_cfg_str('s', g_appCfg.rtmpPlayerCfg.playerStream);
    }
    if(g_appCfg.funcName == APP_rtmp_push){
        usr_assert(getRtmpPusherCfg(g_appCfg.rtmpPusherCfg));
        correct_cfg_str('h', g_appCfg.rtmpPusherCfg.pushHost);
        correct_cfg_str('a', g_appCfg.rtmpPusherCfg.pushApp);
        correct_cfg_str('s', g_appCfg.rtmpPusherCfg.pushStream);
        correct_cfg_str('w', g_appCfg.rtmpPusherCfg.pushOriginFile);
    }

    return true;
}


void appCfgPrint(AppCfg& cfg){
    
}