
#include <unistd.h>
#include "xlog.h"
#include <errno.h>
#include <string.h>
#include <AppCfg.h>

static char* sg_rtspSrvIp = (char*)"127.0.0.1";

void start_run_rtsp_server(char* runapp, char* ip, char* port){
    pid_t pid;
    LOG_DBG("%s:%s\n", ip, port);
    if ((pid = fork()) < 0) {
            fatal("ERROR: can't create process: fork\n");
        }
    if (pid == 0) {
        int iret=execl("./main", "main","-c", runapp, "-h", ip, "-p", port, NULL);
        if(iret == 0){
            LOG_INFO("run %s succ\n", runapp);
        }
        else{
            LOG_ERR("run %s err %d, %s\n", runapp, errno, strerror(errno));
        }
        exit(0);
    }
}

void start_run_rtsp_client(char* runapp, const char* ip, const char* file){
    pid_t pid;
    if ((pid = fork()) < 0) {
            fatal("ERROR: can't create process: fork\n");
        }
    if (pid == 0) {
        int iret=execl("./main", "main","-c", runapp, "-h", ip, "-w", file, NULL);
         if(iret == 0){
            LOG_INFO("run %s succ\n", runapp);
        }
        else{
            LOG_ERR("run %s err %d, %s\n", runapp, errno, strerror(errno));
        }
    }
}

int RtspSrvClientTest(int argc, char const *argv[]){

    start_run_rtsp_server((char*)APP_rtsp_server, sg_rtspSrvIp, (char*)"8554");
    sleep(3);

    start_run_rtsp_client((char*)APP_rtsp_client, "", "live/a.mp4");

//     sleep(1);

//     for(int i=0;i<10;i++){
//         start_run_rtmp_play("live", "avc");
//         usleep(1000*100);
//     }
//     for(int i=0;i<10;i++){
//         start_run_rtmp_play("live", "avc1");
//         usleep(1000*100);
//     }
//     for(int i=0;i<10;i++){
//         start_run_rtmp_play("live", "avc2");
//         usleep(1000*100);
//     }

    while (1){
        LOG_DBG("main....\n");
        sleep(20);
    }
    


    return 0;
}
