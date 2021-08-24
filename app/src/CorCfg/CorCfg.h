
#ifndef _CORCFG_H
#define _CORCFG_H
#include "cJSON.h"
#include "xlog.h"
#include "CorRe.h"

#include <string>
#include <vector>

// api
CorRe corcfg_readFile(const char* fileName);
CorRe corcfg_getBool(const cJSON* _jsonNode, const char* desc, bool& val);
CorRe corcfg_getBool(const char* desc, bool& val);
CorRe corcfg_getInt(const cJSON* _jsonNode, const char* desc, int& val);
CorRe corcfg_getInt(const char* desc, int& val);
CorRe corcfg_getStr(const cJSON* _jsonNode, const char* desc, std::string& val);
CorRe corcfg_getStr(const char* desc, std::string& val);
CorRe corcfg_getArr(const cJSON* _jsonNode, const char* desc, std::vector<cJSON*>& val);
CorRe corcfg_getArr(const char* desc, std::vector<cJSON*>& val);


#endif
