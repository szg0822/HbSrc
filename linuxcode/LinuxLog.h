/**********************************************************
* 文件名: LinuxLog.h
* 功能描述: 日志头文件
* 作者: 孙志刚
* 日期: 2021/1/14
* 备注：开发日志模块功能，单元测试通过；
        功能：1、开关控制窗口打印或者保存文件；2、文件大于5M，自动清空重写；
***********************************************************/

#ifndef _LINUXLOG_H_
#define _LINUXLOG_H_

#include <stdarg.h>


#define LOG_DEBUG  1   // debug
#define LOG_ERROR   2   // info

//LogDebug("[%s:%s %u]  deguglog \n", __FILE__, __func__, __LINE__);
//LogError("[%s:%s %u]  errorlog \n", __FILE__, __func__, __LINE__);

#define PrintLog
#ifdef  PrintLog
    #define LogDebug (printf("[Debug]  "), printf )
    #define LogError (printf("[===Error]  "), printf)
#else
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
#endif 

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
