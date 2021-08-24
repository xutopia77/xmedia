
#include <memory>

#include "RtspSrv.h"
#include "xlog.h"
#include "CorRe.h"

using namespace std;

#if 1

extern shared_ptr<RtspSrv> g_rtspSrv;

int rtsp_server(int argc, char const *argv[]){
	shared_ptr<RtspSrv> rtspSrv = make_shared<RtspSrv>();
	g_rtspSrv = rtspSrv;

    /* Initialize the ST library */
    if (cor_thread_init() < 0)
        fatal("initialization failed: st_init\n");
	LOG_DBG("rtsp srv start %s:%d\n", g_appCfg.rtspSrvCfg.ip.c_str(), g_appCfg.rtspSrvCfg.port);
	
	cor_sleep(2);
	g_rtspSrv->start(g_appCfg.rtspSrvCfg.ip.c_str(), g_appCfg.rtspSrvCfg.port);

	cor_thread_exit(nullptr);

	return 0;
}

#endif
