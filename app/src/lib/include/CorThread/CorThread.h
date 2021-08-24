/**
 * cor_thread的c++的封装
 * 
*/
#ifndef _CORTHREAD_H
#define _CORTHREAD_H

#include <functional>
#include <string>
#include "cor_thread.h"

class CorThread
{
public:
    typedef std::function<void(void*)>	eventCallbackFuncType;
    bool start();
    const std::string getDesc(){ return desc; }
    CorThread(const char* threadDesc, eventCallbackFuncType cb, void* threadArg);
    ~CorThread();
private:
    eventCallbackFuncType callBack = nullptr;
    void* threadArg = nullptr;
    std::string desc;
    static void* _thread_main(void* arg);
};

#endif
