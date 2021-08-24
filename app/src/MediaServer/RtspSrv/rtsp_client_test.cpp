
#include "RtspClient.h"
#include "xlog.h"
#include "AppCfg.h"


extern "C" void _rtsp_client_test(const char* host, const char* file, const char* savPath);
int rtsp_client(int argc, char const *argv[]){
    _rtsp_client_test(g_appCfg.rtspCliCfg.ip.c_str(), g_appCfg.rtspCliCfg.file.c_str(), g_appCfg.rtspCliCfg.datSavePath.c_str());
    return 0;
}
