#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "MyThread.h"
#include "LinuxLog.h"

MyThread_t* MyThreadCreate(int ms, void *privData, THREAD_RUN run)
{
    MyThread_t *thread;

    thread = malloc(sizeof(*thread));
    if (thread)
    {
        memset(thread, 0, sizeof(*thread));
        thread->sleepMS = ms;
        thread->privData = privData;
        thread->run = run;
        thread->arg = thread;
		thread->state = THREAD_STOPPED;
    }

    return thread;
}

void MyThreadRelease(MyThread_t* thread)
{
    if(thread)
    {
        MyThreadStop(thread);
        free(thread);
    }
}

void MyThreadStart(MyThread_t* thread)
{
    if(!thread || thread->state)
    {
        return;
    }

    if(pthread_create(&thread->id, NULL, (void*)thread->run, thread->arg))
    {
        return;
    }

    thread->state = THREAD_RUNNING;
}

void MyThreadStop(MyThread_t* thread)
{
    if(thread)
    {
        thread->state = THREAD_STOPPED;
        pthread_join(thread->id, NULL);
    }
}

void MyThreadSleep(MyThread_t* thread)
{
    if(thread)
    {
        usleep(thread->sleepMS*1000);
    }
}

int MyThreadGetState(MyThread_t *thread)
{
    return thread ? thread->state : 0;
}

void MyThreadSetState(MyThread_t *thread, int state)
{
    if (thread)
    {
        thread->state = state;
    }
}

void* MyThreadGetPrivData(MyThread_t *thread)
{
    return thread ? thread->privData : NULL;
}

#define _TEST
#ifndef _TEST
void TPrintf()
{
    printf("pthread ok\n");
}

static void ThreadHandle(void *arg)
{
    MyThread_t *thread = (MyThread_t*)arg;

    while (MyThreadGetState(thread) == THREAD_RUNNING)
    {
        TPrintf();
        MyThreadSleep(thread);
    }

}

int main()
{
    MyThread_t * sigThread;
    printf("thread run\n");
    //日志初始化
    InitLinuxLog();
    LogDebug("[%s:%s %u]=== pthread run", __FILE__, __func__, __LINE__);
    sigThread = MyThreadCreate(1000, NULL, ThreadHandle);
    if (sigThread)
    {
        MyThreadStart(sigThread);
    }
    else
    {
        printf("register: Create SIG collect thread failed");
    }
    printf("thread end\n");
    while(1); //保存主程序活性，线程才能一直跑，当主程序结束时，线程资源回收
    MyThreadRelease(sigThread);
}

#endif