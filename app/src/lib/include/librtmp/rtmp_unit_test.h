
#ifndef _RTMP_UNIT_TEST_H
#define _RTMP_UNIT_TEST_H

typedef struct Rtmp_unit_test_ctrl
{
    char* inputFlvFile;
}Rtmp_unit_test_ctrl;

extern Rtmp_unit_test_ctrl rtmp_unit_test_ctrl;

int rtmp_unit_test(int argc, char const *argv[]);


#endif

