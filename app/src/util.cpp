
#include "util.h"
#include <string>
#include <map>

#include <time.h>
#include <sys/time.h>

using namespace std;

string getTimeStr(struct timeval* tim){
    struct timeval cur_t;
    struct tm* cur_tm;
    if(tim == NULL)
        gettimeofday(&cur_t,NULL);
    else
        cur_t = *tim;
    cur_tm=localtime(&cur_t.tv_sec);
    //printf("Current time is: %sMicro seconds is: %d.\n",asctime(cur_tm),cur_t.tv_usec);
    char arr[64];
    sprintf(arr,"%04d-%02d-%02d-%02d:%02d:%02d:%03ld.%03ld", cur_tm->tm_year+1900, cur_tm->tm_mon,cur_tm->tm_mday,cur_tm->tm_hour,cur_tm->tm_min,cur_tm->tm_sec,\
    cur_t.tv_usec/1000,cur_t.tv_usec%1000);
    return string(arr);
}

//移除指定的字符串
bool stringRemove(string& message, const string& str){
    //Transport: RTP/AVP/UDP;unicast;client_port=17766-17767;server_port=53834-53835;ssrc=15140A07;mode=play
    size_t pos = message.find(str),n=string(str).size();

    if(pos == message.npos){
        return false;
    }
    else{
        message.erase(pos,n);
    }
    return true;
}

bool stringSplitFrom(const string& strin,string str , string& out1,string& out2){
	size_t pos=0;
	if((pos=strin.find(str)) == strin.npos){return false;}
	out1 = strin.substr(0,pos);
	out2 = strin.substr(pos+str.size(),strin.size());
	return true;
}

float getSubTime(int idx,struct timeval* timeRet,string& str){
    static struct timeval first,second,sub;
    float re=0;
    char arr[64];
    if(1==idx){
        gettimeofday(&first,NULL);
        if(timeRet != NULL) *timeRet = first;
        re = first.tv_sec + (float)first.tv_usec/1000/1000;
        str = getTimeStr(&first);
    }
    else if(2==idx){
        gettimeofday(&second,NULL);
        sub.tv_sec = -first.tv_sec + second.tv_sec;
        sub.tv_usec = -first.tv_usec + second.tv_usec;
        if(timeRet != NULL)  *timeRet = sub;
        re = sub.tv_sec + (float)sub.tv_usec/1000/1000;
        str = getTimeStr(&first) + " ~ ";
        str += getTimeStr(&second) + " sub:";
        sprintf(arr,"%f",re);
        str += arr;
    }
    else{
        // LOG_ERR("err idx=%d\n",idx);
		re = 0;
    }
    return re;
}

string getSubTime(int idx){
    string str;
    getSubTime(idx,NULL,str);
    return str;
}

float getSubTime(int idx,string& str){
    return getSubTime(idx,NULL,str);
}

// ms to p
inline const char* ftimestamp(uint32_t ms, char* buf, uint32_t bufLen){
	sprintf(buf, "%02u:%02u:%02u.%03u", ms / 3600000, (ms / 60000) % 60, (ms / 1000) % 60, ms % 1000);
	return buf;
}

