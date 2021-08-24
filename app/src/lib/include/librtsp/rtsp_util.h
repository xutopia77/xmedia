
#ifndef _RTSP_UTIL_H
#define _RTSP_UTIL_H

#include "xlog.h"

#define LOG_RTSP_DBG(...)      _log_util(      LOG_LVL_DBG,  __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "DBG  RTSP",    __VA_ARGS__)
#define LOG_RTSP_INFO(...)     _log_util(      LOG_LVL_INFO, __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "INFO RTSP",    __VA_ARGS__)
#define LOG_RTSP_WARN(...)     _log_util(      LOG_LVL_WARN, __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "WARN RTSP",    __VA_ARGS__)
#define LOG_RTSP_ERR(...)      _log_util(      LOG_LVL_ERR,  __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "ERR  RTSP",    __VA_ARGS__)
#define LOG_RTSP_DBG_N(...)    _log_util_n(    LOG_LVL_DBG,  __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "DBG  RTSP",    __VA_ARGS__)
#define LOG_RTSP_INFO_N(...)   _log_util_n(    LOG_LVL_INFO, __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "INFO RTSP",   __VA_ARGS__)
#define LOG_RTSP_WARN_N(...)   _log_util_n(    LOG_LVL_WARN, __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "WARN RTSP",   __VA_ARGS__)
#define LOG_RTSP_ERR_N(...)    _log_util_n(    LOG_LVL_ERR,  __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "ERR  RTSP",    __VA_ARGS__)


#endif

