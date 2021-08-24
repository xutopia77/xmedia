

#include <string>

#include "xlog.h"
#include "Unitest.h"
#include "AppCfg.h"
#include "Reflector.h"

#include "rtmp_unit_test.h"
#include "rtsp_unit_test.h"
#include "rtp_unit_test.h"

using namespace std;

extern int flv_read_write_test(int argc, char const *argv[]);
extern int flv_reader_test(int argc, char const *argv[]);
extern int avc2flv_test(int argc, char const *argv[]);
extern int rtmp_push(int argc, char const *argv[]);
extern int rtmp_play(int argc, char const *argv[]);
extern int corcfg_test(int argc, char const *argv[]);
extern int RtmpSrvPushPullTest(int argc, char const *argv[]);
extern int RtspSrvClientTest(int argc, char const *argv[]);
extern int unitestMisc(int argc, char const *argv[]);
extern int parseMainArgvTest(int argc, char const *argv[]);

extern int http_server_test(int argc, char const *argv[]);
extern int http_unit_test(int argc, char const *argv[]);

extern int rtsp_server(int argc, char const *argv[]);
extern int rtsp_client(int argc, char const *argv[]);
// extern int rtsp_push_server_test(int argc, char const *argv[]);

extern int RmComment_test(int argc, char const *argv[]);

int server_test(int argc, char const *argv[]);

int register_unitest(int argc, char const *argv[]){
    string  inH264AacFlv = g_appCfg.datInPath + "/" + g_appCfg.inH264AacFlv;

    string outFlvFile(g_appCfg.datOutPath + "/out.flv");

    string strArg(argv[2]);

    RE_REGISTER(rtmp_unit_test);
    RE_REGISTER(rtmp_push);
    RE_REGISTER(rtmp_play);
    RE_REGISTER(avc2flv_test);
    RE_REGISTER(flv_reader_test);
    RE_REGISTER(flv_read_write_test);
    RE_REGISTER(http_unit_test);
    RE_REGISTER(http_server_test);
    RE_REGISTER(corcfg_test);
    RE_REGISTER(unitestMisc);
    RE_REGISTER(parseMainArgvTest);
    RE_REGISTER_SETNAME(rtmp, RtmpSrvPushPullTest);
    RE_REGISTER_SETNAME(rtsp, RtspSrvClientTest);

    RE_REGISTER(rtsp_unit_test);
    RE_REGISTER(rtsp_server);
    RE_REGISTER(rtsp_client);
    // RE_REGISTER(rtsp_push_server_test);
    RE_REGISTER(rtp_unit_test);
    RE_REGISTER(RmComment_test);
    RE_REGISTER(server_test);

    {
        rtmp_unit_test_ctrl.inputFlvFile = (char*)"../dat/in/h264.640x360_aac_31s.flv";
    }

    {
        rtsp_unit_testCtrl.sdp_test_file = (char*)"../ireader/lib3rd/libsrc/librtsp/test/sdp1.txt";
        rtsp_unit_testCtrl.in_mp4File = (char*)"../dat/in/h264.640x352_aac_31s.mp4";

        static string strHost = string("192.168.31.111");
        rtsp_unit_testCtrl.srvHost = (char*)strHost.c_str();
        rtsp_unit_testCtrl.srvPort = 8554;
        rtsp_unit_testCtrl.cliHost = (char*)strHost.c_str();
        rtsp_unit_testCtrl.cliPort = 8554;
    }

    {
        RtpUnitTestCtrl rtpUnitTestCtrl;
        rtpUnitTestCtrl.inMp4File = "../dat/in/h264.640x352_aac_31s.mp4";
        setRtpUnitTestCtrl(rtpUnitTestCtrl);
    }

    return 0;
}
