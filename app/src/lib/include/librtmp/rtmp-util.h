#ifndef _rtmp_util_h_
#define _rtmp_util_h_

#include <stdint.h>
#include <stdio.h>

static inline void be_write_uint16(uint8_t* ptr, uint16_t val)
{
	ptr[0] = (uint8_t)((val >> 8) & 0xFF);
	ptr[1] = (uint8_t)(val & 0xFF);
}

static inline void be_write_uint24(uint8_t* ptr, uint32_t val)
{
	ptr[0] = (uint8_t)((val >> 16) & 0xFF);
	ptr[1] = (uint8_t)((val >> 8) & 0xFF);
	ptr[2] = (uint8_t)(val & 0xFF);
}

static inline void be_write_uint32(uint8_t* ptr, uint32_t val)
{
	ptr[0] = (uint8_t)((val >> 24) & 0xFF);
	ptr[1] = (uint8_t)((val >> 16) & 0xFF);
	ptr[2] = (uint8_t)((val >> 8) & 0xFF);
	ptr[3] = (uint8_t)(val & 0xFF);
}

static inline void be_read_uint16(const uint8_t* ptr, uint16_t* val)
{
	*val = (ptr[0] << 8) | ptr[1];
}

static inline void be_read_uint24(const uint8_t* ptr, uint32_t* val)
{
	*val = (ptr[0] << 16) | (ptr[1] << 8) | ptr[2];
}

static inline void be_read_uint32(const uint8_t* ptr, uint32_t* val)
{
	*val = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
}

static inline void le_write_uint32(uint8_t* ptr, uint32_t val)
{
	ptr[3] = (uint8_t)((val >> 24) & 0xFF);
	ptr[2] = (uint8_t)((val >> 16) & 0xFF);
	ptr[1] = (uint8_t)((val >> 8) & 0xFF);
	ptr[0] = (uint8_t)(val & 0xFF);
}

static inline void le_read_uint32(const uint8_t* ptr, uint32_t* val)
{
	*val = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
}

static inline char* convert_ms(uint32_t ms, char* buf, uint32_t bufLen)
{
	snprintf(buf, bufLen, "%02u:%02u:%02u.%03u", ms / 3600000, (ms / 60000) % 60, (ms / 1000) % 60, ms % 1000);
	return buf;
}

#if 0
#define LOG_RTMP_OFTEN(...)      _log_util(      LOG_LVL_DBG,  __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "OFT  RTMP",    __VA_ARGS__)
#else
#define LOG_RTMP_OFTEN(...)		
#endif

#define LOG_RTMP_DBG(...)      _log_util(      LOG_LVL_DBG,  __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "DBG  RTMP",    __VA_ARGS__)
#define LOG_RTMP_INFO(...)     _log_util(      LOG_LVL_INFO, __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "INFO RTMP",    __VA_ARGS__)
#define LOG_RTMP_WARN(...)     _log_util(      LOG_LVL_WARN, __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "WARN RTMP",    __VA_ARGS__)
#define LOG_RTMP_ERR(...)      _log_util(      LOG_LVL_ERR,  __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "ERR  RTMP",    __VA_ARGS__)
#define LOG_RTMP_DBG_N(...)    _log_util_n(    LOG_LVL_DBG,  __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "DBG  RTMP",    __VA_ARGS__)
#define LOG_RTMP_INFO_N(...)   _log_util_n(    LOG_LVL_INFO, __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "INFO RTMP",   __VA_ARGS__)
#define LOG_RTMP_WARN_N(...)   _log_util_n(    LOG_LVL_WARN, __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "WARN RTMP",   __VA_ARGS__)
#define LOG_RTMP_ERR_N(...)    _log_util_n(    LOG_LVL_ERR,  __FILE__, __LINE__, LOG_GET_PID, LOG_GET_TID,  "ERR  RTMP",    __VA_ARGS__)

enum RtmpReCode{
	RTMP_CODE_ERR	=	-1,
	RTMP_CODE_OK 	= 	0,
};

#define RTMPRE_DESC_MAX	1024
typedef struct RtmpRe_s{
	enum RtmpReCode	code;
	char desc[RTMPRE_DESC_MAX];
	int usrInt;
}RtmpRe;

#define RTMPRE_INIT	{RTMP_CODE_OK, {0}, -1}

#define RTMP_RETURN_OK(re) 			{ re.code = RTMP_CODE_OK; re.desc[0] = '\0'; return re;}
#define RTMP_RETURN_ERR(re, str) 	{ re.code = RTMP_CODE_ERR; snprintf(re.desc, RTMPRE_DESC_MAX, str); return re;}

#endif /* !_rtmp_util_h_ */
