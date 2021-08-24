
#include "xlog.h"
#include <string>
#include "AppCfg.h"

using namespace std;

extern void _rtmp_play_test(const char* _host, const char* app, const char* stream, const char* flv);

int rtmp_play(int argc, char const *argv[]){
	string playerRecordFile = g_appCfg.datOutPath + "/player_" + g_appCfg.rtmpPlayerCfg.playerHost + g_appCfg.rtmpPlayerCfg.playerApp + g_appCfg.rtmpPlayerCfg.playerStream + ".flv";
	_rtmp_play_test(g_appCfg.rtmpPlayerCfg.playerHost.c_str(), g_appCfg.rtmpPlayerCfg.playerApp.c_str(), g_appCfg.rtmpPlayerCfg.playerStream.c_str(), playerRecordFile.c_str());
	// LOG_RTMP_INFO("rtmp play start %d\n", i);
	return 0;
}