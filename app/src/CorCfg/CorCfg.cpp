
#include "CorCfg.h"

#include <string>
#include <vector>

using namespace std;

// 使用字符串分割
void stringsplit(const string& str, const string& splits, vector<string>& res){
	if (str == "")		return;
	//在字符串末尾也加入分隔符，方便截取最后一段
	string strs = str + splits;
	size_t pos = strs.find(splits);
	int step = splits.size();

	// 若找不到内容则字符串搜索函数返回 npos
	while (pos != strs.npos)
	{
		string temp = strs.substr(0, pos);
		res.push_back(temp);
		//去掉已分割的字符串,在剩下的字符串中进行分割
		strs = strs.substr(pos + step, strs.size());
		pos = strs.find(splits);
	}
}

/**
 * return:-1:err, >0: read size
*/
int readFileAll(const char* file, char** outFile) {
    FILE* pfile = fopen(file, "rb");
    if (pfile == NULL) {
        return -1;
    }
    fseek(pfile, 0, SEEK_END);
    size_t length = ftell(pfile);
    char* buf = new char[length+1];
    rewind(pfile);
    if (fread(buf, 1, length, pfile) != length) {
        delete buf;
        return -1;
    }
    buf[length] = '\0';
    *outFile = buf;
    fclose(pfile);
    return  length + 1;
}

int writeFile(const char* file, const char* inFile, size_t len) {
    FILE* pfile = fopen(file, "wb");
    if (pfile == NULL) {
        return -1;
    }
    size_t re = fwrite(inFile,len,1,pfile);
    fclose(pfile);
    return re;
}

static cJSON* sg_root;


/**
 * err NULL
*/
static cJSON* corcfg_getTarJsonNode(const cJSON* _jsonNode, const char* desc){
    vector<string> nodeVec;
    cJSON* jsonCur = (cJSON*)_jsonNode, *jsonNode;
    stringsplit(desc, ".", nodeVec);
    for(auto& nodeCell : nodeVec){
        jsonNode = cJSON_GetObjectItem(jsonCur, nodeCell.c_str());
        if(nullptr == jsonNode){
            return NULL;
        }
        jsonCur = jsonNode;
    }
    return jsonNode;
} 

extern void rmComment(string str, string& strOut);

CorRe corcfg_readFile(const char* fileName){
    char* rdBuf = nullptr;
    if(readFileAll(fileName, &rdBuf)<0 ){
        return CORRE_ERR(string("read file err ")+fileName);
    }
    string strOut;
    rmComment(string(rdBuf), strOut);
    writeFile(string(string(fileName) + ".tmp").c_str(), strOut.c_str(), strOut.size());
    sg_root = cJSON_Parse(strOut.c_str());
    delete rdBuf;
    if(sg_root == nullptr){
        return CORRE_ERR("json file err");
    }
    return CORRE_OK;
}

CorRe corcfg_getBool(const cJSON* _jsonNode, const char* desc, bool& val){
    cJSON* jsonNode=nullptr;
    if(nullptr == (jsonNode=corcfg_getTarJsonNode(_jsonNode, desc))){
        return CORRE_ERR(string("not Find ") + desc);
    }

    if(cJSON_IsBool(jsonNode)){
        val = cJSON_IsTrue(jsonNode) ? true:false;
    }
    else{
        return CORRE_ERR(string("json fmt err ")+to_string(jsonNode->type));
    }
    return CORRE_OK;
}
CorRe corcfg_getBool(const char* desc, bool& val){
    return corcfg_getBool(sg_root, desc, val);
}

CorRe corcfg_getInt(const cJSON* _jsonNode, const char* desc, int& val){
    cJSON* jsonNode=nullptr;
    if(nullptr == (jsonNode=corcfg_getTarJsonNode(_jsonNode, desc))){
        return CORRE_ERR(string("not Find ") + desc);
    }

    if(cJSON_Number != jsonNode->type){
        return CORRE_ERR(string("json fmt err ")+to_string(jsonNode->type));
    }
    val = jsonNode->valueint;
    return CORRE_OK;
}

CorRe corcfg_getInt(const char* desc, int& val){
    return corcfg_getInt(sg_root, desc, val);
}

CorRe corcfg_getStr(const cJSON* _jsonNode, const char* desc, string& val){
    cJSON* jsonNode=nullptr;
    if(nullptr == (jsonNode=corcfg_getTarJsonNode(_jsonNode, desc))){
        return CORRE_ERR(string("not Find ") + desc);
    }

    if(cJSON_String != jsonNode->type){
        return CORRE_ERR(string("json fmt err ")+to_string(jsonNode->type));
    }
    val = jsonNode->valuestring;
    return CORRE_OK;
}

CorRe corcfg_getStr(const char* desc, string& val){
    return corcfg_getStr(sg_root, desc, val);
}

CorRe corcfg_getArr(const cJSON* _jsonNode, const char* desc, vector<cJSON*>& val){
    cJSON* jsonNode=nullptr;
    if(nullptr == (jsonNode=corcfg_getTarJsonNode(_jsonNode, desc))){
        return CORRE_ERR(string("not Find ") + desc);
    }

    if(cJSON_Array != jsonNode->type){
        return CORRE_ERR(string("json fmt err ")+to_string(jsonNode->type));
    }

    int num = cJSON_GetArraySize(jsonNode);
    for(int i=0; i<num; i++){
        cJSON* cell = cJSON_GetArrayItem(jsonNode, i);
        if(nullptr == cell){
            val.clear();
            return CORRE_ERR(string("json arr fmt err cell is null "));
        }
        val.push_back(cell);
    }
    return CORRE_OK;
}

CorRe corcfg_getArr(const char* desc, vector<cJSON*>& val){
    return corcfg_getArr(sg_root, desc, val);
}

