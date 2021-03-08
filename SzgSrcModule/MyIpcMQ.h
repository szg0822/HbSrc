#ifndef MYIPCMQ_H
#define MYIPCMQ_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C"{
#endif


    #define MSGQ_TYPE(sender,receiver,type)  (((0xff & (sender)) << 16)+((0xff & (receiver)) << 8)+(0xff & (type)))

    #define MSGQ_MAX_BLOCK                   512000
    #define MSGQ_MAX_MSGNUM                  100
    #define MSGQ_PCT_MAX                     0.8

    #define MSGQ_PATHNAME                    "/home/work"
    #define MSGQ_PROJID                      'a'
    #define MSGQ_GET_FLAG                    0777

    int     MyIpcMessageQueue_GetMessageId();
    void    MyIpcMessageQueue_Destroy(int msgId);
    int     MyIpcMessageQueue_Read(int msgId, long msgType, char *msg, size_t *msgsize);
    int     MyIpcMessageQueue_Write(int msgId, long msgType, const char *msg, size_t msgsize);

#ifdef __cplusplus
}
#endif

#endif 