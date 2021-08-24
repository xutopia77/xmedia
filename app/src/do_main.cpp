

#include <string>

#include "MediaServer.h"
#include "HttpServer.h"
#include "CorCfg.h"

#include "xlog.h"
#include "AppCfg.h"

using namespace std;

shared_ptr<MediaServer> pMediaSrv;

int do_main(int argc, char const *argv[]){

    /* Initialize the ST library */
    if (cor_thread_init() < 0)
        fatal("initialization failed: st_init\n");

    pMediaSrv = make_shared<MediaServer>();
    pMediaSrv->start();

    shared_ptr<HttpServer> pHttpServer = make_shared<HttpServer>();
    pHttpServer->start(nullptr, g_appCfg.httpSrvCfg.port);


    cor_thread_exit(nullptr);

    return 0;
}
