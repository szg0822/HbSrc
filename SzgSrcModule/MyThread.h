#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <pthread.h>

typedef void(*THREAD_RUN)(void*);

typedef struct my_thread
{
    pthread_t   id;
    void        *privData;
    void        *arg;
    int         state;
    int         sleepMS;
    THREAD_RUN  run;
}MyThread_t;

#define THREAD_STOPPED      0
#define THREAD_RUNNING      1



MyThread_t*  MyThreadCreate(int ms, void *privData, THREAD_RUN run);
void    MyThreadStart(MyThread_t* thread);
void    MyThreadStop(MyThread_t* thread);
void    MyThreadSleep(MyThread_t* thread);
void    MyThreadRelease(MyThread_t* thread);


int         MyThreadGetState(MyThread_t *thread);
void        MyThreadSetState(MyThread_t *thread, int state);
void*       MyThreadGetPrivData(MyThread_t *thread);

#endif