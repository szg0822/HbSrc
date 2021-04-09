/*
pipe（建立管道）：
1) 头文件 #include<unistd.h>
2) 定义函数： int pipe(int filedes[2]);
3) 函数说明： pipe()会建立管道，并将文件描述词由参数filedes数组返回。
              filedes[0]为管道里的读取端
              filedes[1]则为管道的写入端。
4) 返回值：  若成功则返回零，否则返回-1，错误原因存于errno中。

    错误代码:
         EMFILE 进程已用完文件描述词最大量
         ENFILE 系统已无文件描述词可用。
         EFAULT 参数 filedes 数组地址不合法。
*/
#include <unistd.h>
#include <stdio.h>
 
int main( void )
{
    int filedes[2];
    char buf[80];
    pid_t pid;
    
    pipe( filedes );
    pid=fork();        
    if (pid > 0)
    {
        printf( "This is in the father process,here write a string to the pipe.\n" );
        char s[] = "Hello world , this is write by pipe.\n";
        write( filedes[1], s, sizeof(s) );
        close( filedes[0] );
        close( filedes[1] );
    }
    else if(pid == 0)
    {
        printf( "This is in the child process,here read a string from the pipe.\n" );
        read( filedes[0], buf, sizeof(buf) );
        printf( "%s\n", buf );
        close( filedes[0] );
        close( filedes[1] );
    }
    
    waitpid( pid, NULL, 0 );
    
    return 0;
}

#if 0  //study参考
#include <stdio.h>
#include <unistd.h>


static int pip_fd1[2], pip_fd2[2];
static int fd1_pip, fd2_pip;


	//创建读写管道，fd[0]在管道读取，fd[1]在管道写入 
	if(pipe(pip_fd1) < 0)
	{
		LogError("[%s:%s %u]  pip_fd1 error \n", __FILE__, __func__, __LINE__);
	}
	// if(pipe(pip_fd2) < 0)
	// {
		// TERR("pip_fd2 error\n");
	// }

	fd1_pip = pip_fd1[0];
	fd2_pip = pip_fd2[1];
	udpfunc.m_pPipFd1 = &pip_fd1[1];
	udpfunc.m_pPipFd2 = &pip_fd1[0];


	//UCHAR readbuf[2] = {0};
	//UCHAR writebuf[2] = {0};
//从管道里读取cmd
	// if (read(fd1_pip, readbuf, 1) <= 0) {
		// LogError("[%s:%s %u]  Read Pip Faild, buf=0x%x \n", __FILE__, __func__, __LINE__, readbuf[0]);
		// break;
	// }


    //将接收到的cmd写入管道，以便UdpSend()函数使用。过滤下面3个cmd	
	// if ((*(m_pRecvCmd) == PC_COMMON_TYPE_SINGLE_SHORT) || (*(m_pRecvCmd) == PC_COMMON_TYPE_LIVE_ACQ) || (*(m_pRecvCmd) == PC_COMMON_TYPE_STOP_ACQ))
		// write( *(int*)m_pPipFd1, m_pRecvCmd, 1);  

#endif 