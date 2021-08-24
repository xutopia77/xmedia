

#include <string>

#include "CorCfg.h"

#include "xlog.h"
#include "AppCfg.h"
#include "Reflector.h"
#include "parseMainArgv.h"

using namespace std;

int usage(int argc, char const *argv[]){
    string strUsage = "-c config.json";
    vector<string> allRegisterFunc = RE_GET_REG();
    string str_register_fun;
    for(auto& x: allRegisterFunc){
        str_register_fun += string("\t") + x + "\n";
    }
    LOG_INFO_N( "****************************************\n");
    LOG_INFO_N( "usage: \n"                                 );
    LOG_INFO_N( "./test -c <func name> \n"	                );
    LOG_INFO_N( "useful func: \n"                           );
    LOG_INFO_N( "%s", str_register_fun.c_str()              );
    LOG_INFO_N( "****************************************\n");
    return 0;
}

extern bool getCfg(const char* cfgPath);
extern int register_unitest(int argc, char const *argv[]);

extern int do_main(int argc, char const *argv[]);

extern void appCfgPrint(AppCfg& cfg);

int main(int argc, char const *argv[]){
    RE_REGISTER(usage);
    RE_REGISTER_SETNAME(server, do_main);
    RE_REGISTER_SETNAME(rtmp_server, do_main);
    register_unitest(argc, argv);
    parseMainArgv(argc, argv);
    usr_assert(correct_cfg_str('c', g_appCfg.funcName));
    string cfgFile = "../app/conf/mediaServer.json";
    correct_cfg_str('f', cfgFile);
    usr_assert(getCfg(cfgFile.c_str()));
    appCfgPrint(g_appCfg);

    // LOG_INFO("run %s\n", g_appCfg.funcName.c_str());
    RE_RUN_REG(g_appCfg.funcName.c_str(), argc, argv);
    // usage(argc, argv);
 
    // LOG_INFO("szfdsf\n");
    return 0;
}
