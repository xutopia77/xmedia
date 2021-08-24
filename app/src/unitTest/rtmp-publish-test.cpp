
#include <string>

#include "AppCfg.h"

using namespace std;

extern void _rtmp_publish_test(const char* _host, const char* app, const char* stream, const char* flv);

int rtmp_push(int argc, char const *argv[]){
	string pushOriginFile = g_appCfg.datInPath + "/" + g_appCfg.rtmpPusherCfg.pushOriginFile;
	_rtmp_publish_test(g_appCfg.rtmpPusherCfg.pushHost.c_str(), g_appCfg.rtmpPusherCfg.pushApp.c_str(),
					  g_appCfg.rtmpPusherCfg.pushStream.c_str(), pushOriginFile.c_str());
	return 0;
}
