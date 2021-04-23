#include "thread.h"
#include "LinuxLog.h"
#include <string.h> 
 
void* Thread::run1()
{
    threadStatus = THREAD_STATUS_RUNNING;
    tid = pthread_self();
    run();
    threadStatus = THREAD_STATUS_EXIT;
    tid = 0;
    pthread_exit(NULL);
}
 
Thread::Thread()
{
    tid = 0;
	var = 99;
    threadStatus = THREAD_STATUS_NEW;
}
 
bool Thread::start()
{
	int iRet = 0;
    iRet = pthread_create(&tid, NULL, thread_proxy_func, this);
	if(iRet < 0){
        LogError("[%s:%s %u]=== pthread creat error! [%s:%d]", __FILE__, __func__, __LINE__, strerror(errno), errno);
	}
		
}
 
pthread_t Thread::getThreadID()
{
    return tid;
}
 
int Thread::getState()
{
    return threadStatus;
}
 
void Thread::join()
{
    if (tid > 0)
    {
        pthread_join(tid, NULL);
    }
}
//线程回调
void * Thread::thread_proxy_func(void * args)
{
 		Thread * pThread = static_cast<Thread *>(args); //强制转化为对象
 
		pThread->run(); 
 		
 		return NULL; 
}
 
void Thread::join(unsigned long millisTime)
{
    if (tid == 0)
    {
        return;
    }
    if (millisTime == 0)
    {
        join();
    }else
    {
        unsigned long k = 0;
        while (threadStatus != THREAD_STATUS_EXIT && k <= millisTime)
        {
            usleep(100);
            k++;
        }
    }
}
