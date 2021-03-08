/**********************************************************
 * * 文件名: MyFile.h
 * * 功能描述: 文件函数
 * * 作者: 孙志刚
 * * 日期: 2021/1/26
 * * 备注：
 * ***********************************************************/

#ifndef MYFILE_H
#define MYFILE_H

#define True  1
#define False 0
#define UCHAR unsigned char
#define UINT  unsigned int 

#define MY_FILE_BUFF_SIZE 1024         //缓冲区大小
#define MY_FILE_PATH_DEPTH 256         //目录最大深度
#define MY_FILE_DIRECTORY_MODE 0755    //创建目录权限

UCHAR m_FilePath[MY_FILE_BUFF_SIZE];
//按分隔符拆分字符串;
char** MyStringSplits(const char *s, int slen, const char *sep, int seplen, int *count);
void MySplitsFree(char **splits, int slen);


//获取当前文件目录
int GetModeleFilePath();
//获取时间
char* MyGetDateTime();


//复制文件
int MyCopyFile(UCHAR* src, UCHAR* dst);
//移动文件
int MyMoveFile(char* src, char* dst);
//删除文件
int MyDeleteFile(char* src);


//创建目录
int MyCreateDir(const UCHAR *path);
//复制目录
int MyCopyDir(char* src, char* dst);
//移动目录
int MyMoveDir(char* src, char* dst);
//判断目录是否存在
int MyDirExist(UCHAR * path);
//获取当前文件目录
int GetModeleFilePath();




#endif 