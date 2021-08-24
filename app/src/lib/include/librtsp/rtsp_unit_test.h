
#ifndef _RTSP_UNIT_TEST_H
#define _RTSP_UNIT_TEST_H

#include <string>
#include <stdint.h>

typedef struct Rtsp_unit_testCtrl{
    char* sdp_test_file;
    char* in_mp4File;
    char* srvHost;
    uint16_t    srvPort;
    char* cliHost;
    uint16_t    cliPort;
}Rtsp_unit_testCtrl;

extern Rtsp_unit_testCtrl rtsp_unit_testCtrl;

int rtsp_unit_test(int argc, char const *argv[]);


#endif
