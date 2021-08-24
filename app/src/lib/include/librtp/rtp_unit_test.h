
#ifndef _RTP_UNIT_TEST_H
#define _RTP_UNIT_TEST_H

#include <string>

typedef struct RtpUnitTestCtrl{
    std::string inMp4File;
}RtpUnitTestCtrl;


void setRtpUnitTestCtrl(RtpUnitTestCtrl dat);
int rtp_unit_test(int argc, char const *argv[]);



#endif
