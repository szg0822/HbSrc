/**********************************************************
* 文件名: LinuxLog.cpp
* 功能描述: 日志文件
* 作者: 孙志刚
* 日期: 2021/1/14
* 备注：
***********************************************************/

#include "LinuxLog.h"
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#define LOGFILE_PATH_LEN 260

static int _fileSize = 1024*1024;
static int _level = LOG_DEBUG;
static char *_fileName = NULL;
static pthread_mutex_t _writeLock = PTHREAD_MUTEX_INITIALIZER;


static void LogSetLevel(const char *level)
{
    if (level == NULL || strcasecmp(level, "debug") == 0)
    {
        _level = LOG_DEBUG; // set default with debug
    }
    else if (strcasecmp(level, "error") == 0)
    {
        _level = LOG_ERROR;
    }
    else
    {
        _level = LOG_DEBUG; // not matched, set default with debug
    }
}

static void LogSetFileSize(int size)
{
    _fileSize = size*1024*1024;
}

static void LogSetFile(const char *filename)
{
    if (_fileName)
    {
        free(_fileName);
    }
    _fileName = filename ? strdup(filename) : NULL;
}


void LogControl(int level, const char *fmt, ...)
{
    FILE *fp;
    struct stat fstate;
    int iReturn;

    if (level < _level)
    {
        return;
    }

    pthread_mutex_lock(&_writeLock);
    iReturn = stat(_fileName, &fstate);
    if (iReturn == 0)
    {
        if(fstate.st_size >= _fileSize)
        {
            remove(_fileName);
        }
    }

    fp = fopen(_fileName, "a");
    if (fp == NULL)
    {
        pthread_mutex_unlock(&_writeLock);
        return;
    }

    switch(level)
    {
    case LOG_DEBUG:
        fprintf(fp, "[Debug] - ");
        break;
    case LOG_ERROR:
        fprintf(fp, "[===Error] - ");
        break;
    default:
        fprintf(fp, " - ");
    }
	
	fprintf(fp, "P[%d] ", getpid());

    {
        struct timeval tv;
        struct tm *ptm;
        char timeString[64];
        va_list args;

        gettimeofday(&tv, NULL);
        ptm = localtime(&tv.tv_sec);
        strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", ptm);
        fprintf(fp, "%s.%03d ", timeString, (int)tv.tv_usec/1000);

        va_start(args, fmt);
        vfprintf(fp, fmt, args);
        va_end(args);
        //fprintf(fp, "\n");
    }

    fflush(fp);
    fclose(fp);
    pthread_mutex_unlock(&_writeLock);
    sync();
}

static int PathFileExists(const char *filepath)
{
	if( 0 != access(filepath, 0))
		return 0;
	else
		return -1;
}

static int CreateDirectory( char* dir, unsigned int  m)
{
    if(NULL == dir || '\0' == *dir)
        return -1;
              
    char *p = strrchr(dir, '/');
                  
    if(p != NULL)
    {    
        char parent_dir[4096] = { 0 }; 
        strncpy(parent_dir, dir, p - dir);
        if(access(parent_dir, F_OK) != 0)
            CreateDirectory(parent_dir, m);
    }    
                      
    if(access(dir, F_OK) != 0)
    {    
        if( mkdir(dir, m) != 0)
                return -1;
    }    
                                                                                              
    return 0;
}

/***************************************************************************
* 函 数 名: InitLinuxLog
* 功能描述: 初始化日志
* 参数说明:
* 返 回 值：	    
* 备    注:在当前目录下创建目录HBILinuxLog，并写log到HBILinux.log,大小为5兆
****************************************************************************/
void InitLinuxLog()
{
    char logfile[LOGFILE_PATH_LEN] = { 0 };
    memset(logfile, 0, sizeof(logfile));
    sprintf(logfile, "%s/HBILinuxLog", ".");
    if(!PathFileExists(logfile)) CreateDirectory(logfile,0755);

    memset(logfile, 0, sizeof(logfile));
    sprintf(logfile, "%s/HBILinuxLog/%s", "./", "HBILinux.log");

    LogSetFile(logfile);
	LogSetLevel("debug");
	LogSetFileSize(5);  //5M
    //LogDebug("[%s:%s %u]=== ", __FILE__, __func__, __LINE__);

    return ;
}