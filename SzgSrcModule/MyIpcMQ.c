#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>

#include "MyIpcMQ.h"
#include "LinuxLog.h"
#include "MyThread.h"

typedef struct _Msg_Def
{
    long msgType;
    char msgData[MSGQ_MAX_BLOCK];
} MSGBUF;

int MyIpcMessageQueue_GetMessageId()
{
    key_t key;
    int iMsgqId;

    key = ftok(MSGQ_PATHNAME, MSGQ_PROJID);
    if (key == -1)
    {
        LogError
        (
            "Dtu_IpcMessageQueue_GetMessageId: ftok[pathname=%s, proj_id=%d] error[errno=%d, desc=%s]",
            MSGQ_PATHNAME,
            MSGQ_PROJID,
            errno,
            strerror(errno)
        );
        return -1;
    }

    iMsgqId = msgget(key, IPC_CREAT | MSGQ_GET_FLAG);
    if (iMsgqId == -1)
    {
        LogError("Dtu_IpcMessageQueue_GetMessageId: msgget error[errno=%d, desc=%s]", errno, strerror(errno));
        return -1;
    }

    LogDebug("Dtu_IpcMessageQueue_GetMessageId: MessageQ[id=%d] OK", iMsgqId);
    return iMsgqId;
}

void MyIpcMessageQueue_Destroy(int msgId)
{
    int iReturn = msgctl(msgId, IPC_RMID, NULL);
    if (iReturn == -1)
    {
        LogError("Dtu_IpcMessageQueue_Destroy: msgget error[errno=%d, desc=%s]", errno, strerror(errno));
    }
    else
    {
        LogDebug("Dtu_IpcMessageQueue_Destroy: msgctl[IPC_RMID] success");
    }
}

int MyIpcMessageQueue_Read(int msgId, long msgType, char *msg, size_t *msgsize)
{
    int iReturn;
    MSGBUF msgbuf;

    memset(msg, 0, *msgsize);
    if (*msgsize > MSGQ_MAX_BLOCK)
    {
        LogError("Dtu_IpcMessageQueue_Read: size too long, size[receive=%d, max=%d]", *msgsize, MSGQ_MAX_BLOCK);
        return -1;
    }

    iReturn = msgrcv(msgId, (void*)&msgbuf, *msgsize, msgType, IPC_NOWAIT);
    if (iReturn == -1)
    {
        if (errno != ENOMSG)
        {
            LogError("Dtu_IpcMessageQueue_Read: msgrcv[type=%ld], error[errno=%d, desc=%s]", msgType, errno, strerror(errno));
        }
        return -1;
    }

    *msgsize = iReturn;
    memcpy(msg, msgbuf.msgData, *msgsize);
    LogDebug("Dtu_IpcMessageQueue_Read: msgrcv[type=%ld, size=%lu] success",msgType, iReturn);

    return 0;
}

int MyIpcMessageQueue_Write(int msgId, long msgType, const char *msg, size_t msgsize)
{
    int iReturn;
    MSGBUF msgbuf;

    if (msgsize > MSGQ_MAX_BLOCK)
    {
        LogError("Dtu_IpcMessageQueue_Write: message too large[size=%d, max=%d]", msgsize, MSGQ_MAX_BLOCK);
        return -1;
    }

    msgbuf.msgType = msgType;
    memcpy(msgbuf.msgData, msg, msgsize);
    msgbuf.msgData[msgsize] = '\0';

    iReturn = msgsnd(msgId, &msgbuf, msgsize, IPC_NOWAIT);
    if (iReturn == -1)
    {
        LogError("Dtu_IpcMessageQueue_Write: msgsnd[size=%d, msg=%s], error[errno=%d, desc=%s]", msgsize, msg, errno, strerror(errno));
        return -1;
    }

    LogDebug("Dtu_IpcMessageQueue_Write: msgsnd[msgType=%ld, sndsize=%d, msgsize=%lu] success", msgType, iReturn, msgsize);

    return 0;
}

#define _TEST

#ifndef _TEST
static int _msgId = -1;
void Tread()
{
    
    char buf[128];
    int iReturn;
    size_t rdSize;
    //类型可以用MSGQ_TYPE(0x01, 0x02, 0x03)组合表示
    long msgType = 0xeee;
    printf("read ok\n");
    if (_msgId == -1)
    {
        _msgId = MyIpcMessageQueue_GetMessageId();
        if (_msgId == -1)
        {
            LogError("Watcher_Process_Control: get messageQ id failed");
            return;
        }
    }

    rdSize = sizeof(buf);
    iReturn = MyIpcMessageQueue_Read(_msgId, msgType, buf, &rdSize);
    if (iReturn == -1)
    {
        return;
    }
    printf("read buf=[%s]\n", buf);
}
void Twrite()
{
    printf("write ok\n");
    int iReturn;
    char cstr[128] = "messageOk";
    //类型可以用MSGQ_TYPE(0x01, 0x02, 0x03)组合表示
    long MsgeType = 0xeee;
    if (-1 == _msgId) {
        _msgId = MyIpcMessageQueue_GetMessageId();
        if (_msgId == -1)
        {
            LogError(" error");
            return ;
        }
    }
    iReturn = MyIpcMessageQueue_Write(_msgId, MsgeType, cstr, strlen(cstr));
    if (iReturn != 0)
    {
        LogError("str=[%s]\n", cstr);
    }
}

static void ThreadHandle_read(void *arg)
{
    MyThread_t *thread = (MyThread_t*)arg;

    while (MyThreadGetState(thread) == THREAD_RUNNING)
    {
        Tread();
        MyThreadSleep(thread);
    }

}

static void ThreadHandle_write(void *arg)
{
    MyThread_t *thread = (MyThread_t*)arg;

    while (MyThreadGetState(thread) == THREAD_RUNNING)
    {
        Twrite();
        MyThreadSleep(thread);
    }

}

int main()
{
    MyThread_t * sigThread_read;
    MyThread_t * sigThread_write;
    printf("thread run\n");
    //日志初始化
    InitLinuxLog();
    LogDebug("[%s:%s %u]=== pthread run", __FILE__, __func__, __LINE__);
    sigThread_read = MyThreadCreate(1000, NULL, ThreadHandle_read);
    if (sigThread_read)
    {
        MyThreadStart(sigThread_read);
    }
    else
    {
        printf("register: Create SIG collect thread failed");
    }
    sigThread_write = MyThreadCreate(1000, NULL, ThreadHandle_write);
    if (sigThread_write)
    {
        MyThreadStart(sigThread_write);
    }
    else
    {
        printf("register: Create SIG collect thread failed");
    }
    printf("thread end\n");
    while(1); //保存主程序活性，线程才能一直跑，当主程序结束时，线程资源回收
    MyThreadRelease(sigThread_read);
    MyThreadRelease(sigThread_write);
}
#endif