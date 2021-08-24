
#include "CorCfg.h"

#include <string>
#include <vector>

using namespace std;




bool corcfg_test_getInt(){
   int intVal;

    CorRe re; 
    string nodeDesc = "";

    nodeDesc = "int1";
    re = corcfg_getInt(nodeDesc.c_str(),intVal);
    if(!CORRE_IS_OK(re))    LOG_ERR("find %s err, %s\n", nodeDesc.c_str(), re.desc().c_str()); 
    else                    LOG_DBG("%s = %d\n", nodeDesc.c_str(), intVal);
    
    nodeDesc = "int2.int1";
    re = corcfg_getInt(nodeDesc.c_str(),intVal);
    if(!CORRE_IS_OK(re))    LOG_ERR("find %s err, %s\n", nodeDesc.c_str(), re.desc().c_str());
    else                    LOG_DBG("%s = %d\n", nodeDesc.c_str(), intVal);

    nodeDesc = "int2.int2.int1";
    re = corcfg_getInt(nodeDesc.c_str(),intVal);
    if(!CORRE_IS_OK(re))    LOG_ERR("find %s err, %s\n", nodeDesc.c_str(), re.desc().c_str());
    else                    LOG_DBG("%s = %d\n", nodeDesc.c_str(), intVal);

    nodeDesc = "int2.int2.int2";
    re = corcfg_getInt(nodeDesc.c_str(),intVal);
    if(!CORRE_IS_OK(re))    LOG_ERR("find %s err, %s\n", nodeDesc.c_str(), re.desc().c_str()); 
    else                    LOG_DBG("%s = %d\n", nodeDesc.c_str(), intVal);

    nodeDesc = "int2.int1.int2";
    re = corcfg_getInt(nodeDesc.c_str(),intVal);
    if(!CORRE_IS_OK(re))    LOG_ERR("find %s err, %s\n", nodeDesc.c_str(), re.desc().c_str()); 
    else                    LOG_DBG("%s = %d\n", nodeDesc.c_str(), intVal);

    return true;
}

bool corcfg_test_getStr(){
   string strVal;

    CorRe re; 
    string nodeDesc = "";

    nodeDesc = "str1";
    re = corcfg_getStr(nodeDesc.c_str(),strVal);
    if(!CORRE_IS_OK(re))    LOG_ERR("find %s err, %s\n", nodeDesc.c_str(), re.desc().c_str()); 
    else                    LOG_DBG("%s = \"%s\"\n", nodeDesc.c_str(), strVal.c_str());

    nodeDesc = "str2.str1";
    re = corcfg_getStr(nodeDesc.c_str(),strVal);
    if(!CORRE_IS_OK(re))    LOG_ERR("find %s err, %s\n", nodeDesc.c_str(), re.desc().c_str()); 
    else                    LOG_DBG("%s = \"%s\"\n", nodeDesc.c_str(), strVal.c_str());

    nodeDesc = "str2.str1.str1";
    re = corcfg_getStr(nodeDesc.c_str(),strVal);
    if(!CORRE_IS_OK(re))    LOG_ERR("find %s err, %s\n", nodeDesc.c_str(), re.desc().c_str()); 
    else                    LOG_DBG("%s = \"%s\"\n", nodeDesc.c_str(), strVal.c_str());

    nodeDesc = "str2.str2.str1";
    re = corcfg_getStr(nodeDesc.c_str(),strVal);
    if(!CORRE_IS_OK(re))    LOG_ERR("find %s err, %s\n", nodeDesc.c_str(), re.desc().c_str()); 
    else                    LOG_DBG("%s = \"%s\"\n", nodeDesc.c_str(), strVal.c_str());

    nodeDesc = "str2.str3";
    re = corcfg_getStr(nodeDesc.c_str(),strVal);
    if(!CORRE_IS_OK(re))    LOG_ERR("find %s err, %s\n", nodeDesc.c_str(), re.desc().c_str()); 
    else                    LOG_DBG("%s = \"%s\"\n", nodeDesc.c_str(), strVal.c_str());

    return true;
}

bool corcfg_test_getBool(){
    bool boolVal = false;

    CorRe re; 
    string nodeDesc = "";

    nodeDesc = "bool1.bool1";
    re = corcfg_getBool(nodeDesc.c_str(),boolVal);
    if(!CORRE_IS_OK(re))    LOG_ERR("find %s err, %s\n", nodeDesc.c_str(), re.desc().c_str()); 
    else                    LOG_DBG("%s = \"%s\"\n", nodeDesc.c_str(), boolVal?"true":"false");

    return true;
}

struct HostCfg{
    string name;
    int id;
};

CorRe getHostCfg(const char* desc, vector<HostCfg>& cfgVec){
    vector<cJSON*> jsonVec;
    CorRe re;
    if(!CORRE_IS_OK(re=corcfg_getArr(desc, jsonVec))){
        return CORRE_ERR(string(re.desc()) + "corcfg getArr err");
    }

    for(auto& cell:jsonVec){
        HostCfg cfg;
        re = corcfg_getStr(cell, "hostName", cfg.name);
        usr_return(CORRE_IS_OK(re), re);
        re = corcfg_getInt(cell, "id", cfg.id);
        usr_return(CORRE_IS_OK(re), re);
        cfgVec.push_back(cfg);
    }
    return CORRE_OK;
}


bool corcfg_test_getArr(){
     vector<HostCfg> cfgVec;
    getHostCfg("arr1", cfgVec);
    for(auto& x: cfgVec){
        LOG_DBG("host name: %s,id: %d\n", x.name.c_str(), x.id);
    }
    return true;
}

void _corcfg_test(const char* fileName){
    usr_assert(CORRE_IS_OK(corcfg_readFile(fileName)));
    corcfg_test_getInt();
    LOG_DBG("\n");
    corcfg_test_getStr();
    LOG_DBG("\n");
    corcfg_test_getArr();
    LOG_DBG("\n");
    corcfg_test_getBool();
}

int corcfg_test(int argc, char const *argv[]){
    _corcfg_test("../app/src/unitTest/class.json");
    return 0;
}


