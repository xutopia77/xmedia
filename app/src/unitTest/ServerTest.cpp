
#include <unistd.h>
#include "xlog.h"
#include <errno.h>
#include <string.h>
#include <AppCfg.h>

void start_app_no_arg(const char* runapp){
    pid_t pid;
    if ((pid = fork()) < 0) {
        fatal("ERROR: can't create process: fork\n");
    }
    if (pid == 0) {
        int iret=execl("./main", "main","-c", runapp, NULL);
        if(iret == 0){
            LOG_INFO("run %s succ\n", runapp);
        }
        else{
            LOG_ERR("run %s err %d, %s\n", runapp, errno, strerror(errno));
        }
    }
}

extern void start_run_rtmp_publish(const char* runapp, const char* app, const char* stream, const char* pushFile);
extern void start_run_rtmp_play(const char* runapp, const char* app, const char* stream);
extern void start_run_rtsp_client(char* runapp, const char* ip, const char* file);

int server_test(int argc, char const *argv[]){

    start_app_no_arg((char*)APP_server);
    sleep(1);

    start_run_rtmp_publish((char*)APP_rtmp_push, "live", "avc", "h264.640x360_aac_31s.flv");
    usleep(1000*100);
    start_run_rtmp_publish((char*)APP_rtmp_push, "live", "avc1", "h264.640x360_aac_31s.flv");
    usleep(1000*100);
    start_run_rtmp_publish((char*)APP_rtmp_push, "live", "avc2", "h264.640x360_aac_31s.flv");
    usleep(1000*100);
    start_run_rtmp_publish((char*)APP_rtmp_push, "live", "avc3", "h264.640x360_aac_31s.flv");
    sleep(1);

    int sleepTime = 1;
    for(int i=0;i<10;i++){
        start_run_rtmp_play((char*)APP_rtmp_play, "live", "avc");
        usleep(sleepTime);
    }
    for(int i=0;i<10;i++){
        start_run_rtmp_play((char*)APP_rtmp_play, "live", "avc1");
        usleep(sleepTime);
    }
    for(int i=0;i<10;i++){
        start_run_rtmp_play((char*)APP_rtmp_play, "live", "avc2");
        usleep(sleepTime);
    }

    start_run_rtsp_client((char*)APP_rtsp_client, "", "live/../dat/in/h264.640x352_aac_10s.mp4");

    while (1){
        LOG_DBG("main....\n");
        sleep(30);
    }
    return 0;
}
