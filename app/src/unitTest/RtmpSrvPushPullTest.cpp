
#include <unistd.h>
#include "xlog.h"
#include <errno.h>
#include <string.h>
#include "AppCfg.h"

void start_run_rtmp_publish(const char* runapp, const char* app, const char* stream, const char* pushFile){
    pid_t pid;
    if ((pid = fork()) < 0) {
            fatal("ERROR: can't create process: fork\n");
        }
    if (pid == 0) {
        int iret=execl("./main", "main","-c", runapp, "-a", app, "-s", stream, "-w", pushFile, NULL);
        if(iret == 0){
            LOG_INFO("run %s succ\n", runapp);
        }
        else{
            LOG_ERR("run %s err %d, %s\n", runapp, errno, strerror(errno));
        }
        exit(0);
    }
}

void start_run_rtmp_play(const char* runapp, const char* app, const char* stream){
    pid_t pid;
    if ((pid = fork()) < 0) {
            fatal("ERROR: can't create process: fork\n");
        }
    if (pid == 0) {
        int iret=execl("./main", "main","-c", runapp, "-a", app, "-s", stream, NULL);
        if(iret == 0){
            LOG_INFO("run %s succ\n", runapp);
        }
        else{
            LOG_ERR("run %s err %d, %s\n", runapp, errno, strerror(errno));
        }
    }
}

extern void start_app_no_arg(const char* runapp);

int RtmpSrvPushPullTest(int argc, char const *argv[]){
    start_app_no_arg(APP_rtmp_server);
    sleep(1);

    start_run_rtmp_publish(APP_rtmp_push, "live", "avc", "h264.640x360_aac_31s.flv");
    usleep(1000*100);
    start_run_rtmp_publish(APP_rtmp_push, "live", "avc1", "h264.640x360_aac_31s.flv");
    usleep(1000*100);
    start_run_rtmp_publish(APP_rtmp_push, "live", "avc2", "h264.640x360_aac_31s.flv");
    usleep(1000*100);
    start_run_rtmp_publish(APP_rtmp_push, "live", "avc3", "h264.640x360_aac_31s.flv");
    sleep(1);

    for(int i=0;i<10;i++){
        start_run_rtmp_play(APP_rtmp_play, "live", "avc");
        usleep(1000*100);
    }
    for(int i=0;i<10;i++){
        start_run_rtmp_play(APP_rtmp_play, "live", "avc1");
        usleep(1000*100);
    }
    for(int i=0;i<10;i++){
        start_run_rtmp_play(APP_rtmp_play, "live", "avc2");
        usleep(1000*100);
    }

    while (1){
        LOG_DBG("main....\n");
        sleep(20);
    }
    


    return 0;
}
