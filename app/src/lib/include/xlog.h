
/** 
 * V1.1
 * 2021.08.08
 * 
 * 打印等级全开打印示例：
 * [INFO][2021-06-16 09:42:37.186][-1][000] [main.c#040]  test
 * 前缀，时间，进程id，线程id，文件行号
 * 
 * 默认打印等级 LOG_LVL_DBG
 * 默认打印方式 LOG_FMT_PRE | LOG_FMT_FILE_LINE | LOG_FMT_TIME
 *  
 * 对外API
 * log_setLogLevel 设置打印等级，参数：LOG_LVL_xxx
 * 
 * log_setLogWay 设置日志记录文件路径，文件打不开则进程退出，
 * 程序结束时，记得调用 log_closeLogFile，防止日志丢失。
 * 
 * log_setLogFmt 设置打印方式，参数：LOG_FMT_xxx
 * 
 * log_setLogWayLenT 格式化文件名行号的输出（对齐），
 * e.g. [DBG ][2021-06-16 09:42:37.186][-1][000] [    main.c#0061]  test
 * 参数：fileLen 文件名格式打印长度 int类型
 * 参数：lineLen 行号格式打印长度 uint8类型（非负数）
 * 
 * getErrnoDesc 获取 errno 的 字符串描述
 * 
 * 若想打印 进程id和线程id，则需自己定义 LOG_GET_PID，LOG_GET_TID宏，定开启打印方式LOG_FMT_PID
 * 
*/

#ifndef _XLOG_H
#define _XLOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdlib.h>

#define LOG_MAXLINE 4096 /* max line length */
#define LOG_FILENAME_MAXLINE 256 // max log file name length 

//需要从0开始递增的，作为其它的数组下标使用
enum logLevelE{
    LOG_LVL_NONE = 0,
    LOG_LVL_ERR,
    LOG_LVL_WARN,
    LOG_LVL_INFO,
    LOG_LVL_DBG,
    LOG_LVL_MAX,
};
enum logFmtE{
    LOG_FMT_NONE        = 0x0000,
    LOG_FMT_PRE         = 0x0001,
    LOG_FMT_FILE_LINE   = 0x0002,
    LOG_FMT_PID         = 0x0004,
    LOG_FMT_TIME        = 0x0008,
    LOG_FMT_ALL         = 0xffff,
};

enum logWayE{
    LOG_WAY_NONE        = 0,
    LOG_WAY_FILE        = 1,
    LOG_WAY_CONSOLE     = 2,
    LOG_WAY_BOTH        = 3,
};



void log_setLogLevel(const enum logLevelE l);
void log_setFileLevel(const enum logLevelE l);
// 如果要保存日志，必须设置filename，如果不保存日志，可以设置为NULL
void log_setLogWay(const char* LogFileName, enum logWayE way);
void log_closeLogFile();
void log_setLogFmt(const enum logFmtE w);
void log_setFileFmt(const enum logFmtE w);
void log_setLogWayLenT(const int fileLen, const u_int8_t lineLen );
void log_setFileWayLenT(const int fileLen, const u_int8_t lineLen );
void log_setBothWayLenT(const int fileLen, const u_int8_t lineLen );
const char* getErrnoDesc(int err);

void _log_util(       const enum logLevelE logLvl, const char* file, int line, int pid, int tid, const char* strPre, const char *fmt, ...);
void _log_util_n(     const enum logLevelE logLvl, const char* file, int line, int pid, int tid, const char* strPre, const char *fmt, ...);

#if 0
//根据实际情况，定义
// #define  LOG_GET_PID  getpid()
// #define  LOG_GET_TID  get_cor_thread_id()   // pthread_self()
#else
#define  LOG_GET_PID  -1
#define  LOG_GET_TID  0
#endif

#define LOG_DBG(...)      _log_util(      LOG_LVL_DBG,  __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "DBG ",    __VA_ARGS__)
#define LOG_INFO(...)     _log_util(      LOG_LVL_INFO, __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "INFO",    __VA_ARGS__)
#define LOG_WARN(...)     _log_util(      LOG_LVL_WARN, __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "WARN",    __VA_ARGS__)
#define LOG_ERR(...)      _log_util(      LOG_LVL_ERR,  __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "ERR ",    __VA_ARGS__)

#define LOG_DBG_N(...)    _log_util_n(    LOG_LVL_DBG,  __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "DBG ",    __VA_ARGS__)
#define LOG_INFO_N(...)   _log_util_n(    LOG_LVL_INFO, __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "INFO ",   __VA_ARGS__)
#define LOG_WARN_N(...)   _log_util_n(    LOG_LVL_WARN, __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "WARN ",   __VA_ARGS__)
#define LOG_ERR_N(...)    _log_util_n(    LOG_LVL_ERR,  __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "ERR ",    __VA_ARGS__)

#define fatal(...)        do{LOG_ERR(__VA_ARGS__);abort();exit(1);}while(0)

#define usr_assert(x)           assert((x))
#define usr_return(con, re)     if(!(con)){return (re);}

#ifdef __cplusplus
}
#endif

#endif

