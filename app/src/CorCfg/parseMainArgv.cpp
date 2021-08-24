

#include <string>
#include <map>

#include <unistd.h>

#include "xlog.h"
// #include "AppCfg.h"

#include "parseMainArgv.h"

using namespace std;

struct MainArgv{
    char c;
    string strArg;
    string commit;
    
};

static map<char, MainArgv> sg_mainArgvMap;

#define ARGV_NOT_FIND   "nullptrErr"
#define ARGV_NULL       "nullptr"

static void initMainArgvMap(){
    sg_mainArgvMap['c'] = {'c', ARGV_NULL, "[what func to fun];\n"};
    sg_mainArgvMap['f'] = {'f', ARGV_NULL, "[config.json]// when have -f, first usage argv;\n"};//默认会读取配置文件
    sg_mainArgvMap['a'] = {'a', ARGV_NULL, "[rtmp pull or push app];\n"};
    sg_mainArgvMap['s'] = {'s', ARGV_NULL, "[rtmp pull or push stream];\n"};
    sg_mainArgvMap['w'] = {'w', ARGV_NULL, "[rtmp push file, rtspclient req file];\n"};// push的源文件
    sg_mainArgvMap['h'] = {'h', ARGV_NULL, "[host];\n"};
    sg_mainArgvMap['p'] = {'p', ARGV_NULL, "[port];\n"};
    sg_mainArgvMap['?'] = {'?', ARGV_NULL, "[help];\n"};
}

static string initArgvMsg(){
    string strArgvMsg = ""; 
    strArgvMsg +=   "argv:\n";
    map<char, MainArgv>::iterator iter = sg_mainArgvMap.begin();
    for (; iter != sg_mainArgvMap.end(); iter++){
        MainArgv argv = iter->second;
        strArgvMsg += string("    -") + argv.c + " " + argv.commit;
    }
    return strArgvMsg;
}

static string getMainArgv(char ch){
    if(sg_mainArgvMap.find(ch) != sg_mainArgvMap.end()){
        return sg_mainArgvMap[ch].strArg;
    }
    else{
        return ARGV_NOT_FIND;
    }
}

int parseMainArgv(int argc, char const *argv[])
{
    string strArgs = "";
    char strChStr[2] = {0,0};
    initMainArgvMap();
    int ch;
    opterr = 0;
    while ((ch = getopt(argc, (char **)argv, "c:f:a:s:w:h:p:?")) != -1){
        switch (ch){
        case 'c':
        case 'f':
        case 'a':
        case 's':
        case 'w':
        case 'h':
        case 'p':
        {
            sg_mainArgvMap[ch].strArg = optarg;
            strChStr[0] = ch;
            strArgs += "-" + string(strChStr) + " " + optarg + " ";
            break;
        }
        case '?':{
            LOG_INFO_N("%s\n",initArgvMsg().c_str());
            break;
        }
        default:
            printf("!!!!!!!!!!other option :%c %s\n", ch, optarg);
        }
    }
    if(0 != optopt)
        printf("optopt +%c\n", optopt);
    LOG_INFO("run main %s\n", strArgs.c_str());
    return 0;
}

int parseMainArgvTest(int argc, char const *argv[]){
    return parseMainArgv(argc, argv);
}

#define getMainArgv_is_ok(ch) ( (getMainArgv(ch)!=ARGV_NULL) && (getMainArgv(ch)!=ARGV_NOT_FIND) )
bool correct_cfg_str(char ch, std::string& cfg){
    if(getMainArgv_is_ok(ch)){ 
        cfg = getMainArgv(ch);
        return true;
    }
    else{
        return false;
    }
}    
bool correct_cfg_int(char ch, int& cfg){
    if(getMainArgv_is_ok(ch)){ 
        cfg = strtol(getMainArgv(ch).c_str(), nullptr, 10);
        return true;
    }
    else{
        return false;
    }
}


/*
int parseMainArgv(int argc, char const *argv[]){
   int ch;
   opterr = 0;
   while((ch = getopt(argc, (char **)argv, "a:bcde")) != -1)
   switch(ch){
      case 'a':
         printf("option a:'%s'\n", optarg);  break;
      case 'b':
         printf("option b :b\n");  break;
      default:
         printf("other option :%c\n", ch);
   }
   printf("optopt +%c\n", optopt);
}

$. /getopt -b
option b:b
$. /getopt -c
other option:c
$. /getopt -a
other option :?
$. /getopt -a12345
option a:'12345'

*/