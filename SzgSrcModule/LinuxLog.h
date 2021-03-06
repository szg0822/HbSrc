/**********************************************************
* 文件名: LinuxLog.h
* 功能描述: 日志头文件
* 作者: 孙志刚
* 日期: 2021/1/14
* 备注：
***********************************************************/


#ifndef _LINUXLOG_H_
#define _LINUXLOG_H_

#include <stdarg.h>

#define TestLog  //终端打印开关
#ifdef TestLog
    #define TDBG (printf("[Debug] %s:%s [%u]:%s \t", __FILE__, __func__, __LINE__, __TIME__), printf)
    #define TERR (printf("[Error] %s:%s [%u]:%s \t", __FILE__, __func__, __LINE__, __TIME__), printf)
#else
    #define TDBG 
    #define TERR
#endif

//LogError("[%s:%s %u]=== ", __FILE__, __func__, __LINE__);
//TDBG("epoll ctl failed\n");

#define LOG_DEBUG  1   // debug
#define LOG_ERROR   2   // info

/*********************************************************
* 函 数 名: LogDebug
* 功能描述: 打印Debug日志到文件
* 参数说明:
* 返 回 值：	    
* 备    注:
*********************************************************/
#define LogDebug(...)   LogControl(LOG_DEBUG, __VA_ARGS__)

/*********************************************************
* 函 数 名: LogError
* 功能描述: 打印Error日志到文件
* 参数说明:
* 返 回 值：	    
* 备    注:
*********************************************************/
#define LogError(...)   LogControl(LOG_ERROR, __VA_ARGS__)

/*********************************************************
* 函 数 名: InitLinuxLog
* 功能描述: 初始化日志
* 参数说明:
* 返 回 值：	    
* 备    注:
*********************************************************/
void InitLinuxLog();    
    

void LogControl(int level, const char *fmt, ...);

#endif // DTU_LOG_H
