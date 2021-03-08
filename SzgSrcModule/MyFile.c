/**********************************************************
 * * 文件名: MyFile.c
 * * 功能描述: 文件函数
 * * 作者: 孙志刚
 * * 日期: 2021/1/26
 * * 备注：
 * ***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

#include <time.h>
#include <sys/time.h>

#include "MyFile.h"

//////////////////////////////////////////////////////////////////////////////分割字符串///////////////////////////////////////////////////////////////////
//#define TEST 
/*********************************************************
 * * 函 数 名: MyStringSplits
 * * 功能描述: 按分隔符拆分字符串
 * * 参数说明: s: 要拆分的字符串
 *            slen：字符串长度
 *            sep：某种分隔符
 *            seplen：分隔符长度
 *            count：输出分割的个数
 * * 返 回 值：返回分割的字符串
 * * 备    注: 分隔符用""双引号   
 * *********************************************************/
char** MyStringSplits(const char *s, int slen, const char *sep, int seplen, int *count)
{
    int elements = 0, slots = 5, start = 0, j;
    char **tokens;

	*count = 0;
    if (seplen < 1 || slen <= 0)
    {
		return NULL;
	}

    tokens = malloc(sizeof(char*)*slots);
    if (!tokens)
    {
		return NULL;
	}

    for (j = 0; j < (slen-(seplen-1)); j++)
    {
        if (slots < elements+2)
        {
            char **newtokens;

            slots *= 2;
            newtokens = realloc(tokens,sizeof(char*)*slots);
            if (!newtokens)
            {
				goto cleanup;
			}
            tokens = newtokens;
        }

        if ((seplen == 1 && *(s+j) == sep[0]) || (memcmp(s+j,sep,seplen) == 0))
        {
			if (j == start)
			{
				start = j+seplen;
				j = j+seplen-1;
			}
            else
            {
				tokens[elements] = malloc(j-start+1);
				if (!tokens[elements])
				{
					goto cleanup;
				}
				memcpy(tokens[elements], s+start, j-start);
				tokens[elements][j-start] = '\0';
				elements++;
				start = j+seplen;
				j = j+seplen-1;
			}
        }
    }

    tokens[elements] = malloc(slen-start+1);
	if (!tokens[elements])
	{
		goto cleanup;
	}
	memcpy(tokens[elements], s+start, slen-start);
	tokens[elements][slen-start] = '\0';
    elements++;
    *count = elements;
    return tokens;

cleanup:
    for (j = 0; j < elements; j++)
    {
			free(tokens[j]);
	}
    free(tokens);
    *count = 0;
    return NULL;
}

/*********************************************************
 * * 函 数 名: MySplitsFree
 * * 功能描述: free字符串
 * * 参数说明: splits: 字符串
 *            slen：字符串个数
 * * 返 回 值：
 * * 备    注: 
 * *********************************************************/
void MySplitsFree(char **splits, int slen)
{
	if (splits)
	{
		for (int i = 0; i < slen; i++)
		{
			if (splits[i]) free(splits[i]);
		}
		free(splits);
	}
}

//////////////////////////////////////////////////////////////////////////////文件操作///////////////////////////////////////////////////////////////////
/*********************************************************
 * * 函 数 名: MyMoveFile
 * * 功能描述: 移动文件
 * * 参数说明: src: 源文件路径
 *            dst：目的文件路径
 * * 返 回 值：
 * * 备    注: 路径后面跟着文件名	   
 * *********************************************************/
int MyMoveFile(char* src, char* dst)
{
    if ((NULL == src) || (NULL == dst))
    {
        printf("[%s(), %d] src or dst is NULL!!!", __FUNCTION__, __LINE__);
        return -1;
    }

    printf("porting path1 = \n%s \n!!!! path2 = \n%s \n", src, dst);
    if (0 != rename(src, dst))
    {
        printf("[%s(), %d] move file error!!!", __FUNCTION__, __LINE__);
        return -1;
    }
    return 0;
}

/*********************************************************
 * * 函 数 名: MyDeleteFile
 * * 功能描述: 删除文件
 * * 参数说明: src: 源文件路径
 * * 返 回 值：
 * * 备    注: 路径后面跟着文件名	   
 * *********************************************************/
int MyDeleteFile(char* src)
{
    if ( NULL == src)
    {
        printf("[%s(), %d] src is NULL!!!", __FUNCTION__, __LINE__);
        return -1;
    }

    if( -1 == remove(src))
    {
        printf("[%s(), %d] delete file error  %s!!!", __FUNCTION__, __LINE__, src);
        return -1;
    }
    return 0;
}


/*********************************************************
 * * 函 数 名: GetModeleFilePath
 * * 功能描述: 获取当前文件目录
 * * 参数说明:
 * * 返 回 值：0:成功  -1:失败   
 * * 备    注: 这种方法最可靠，可用于开机启动程序获取自身目录;
 * 			   getcwd方法用于开机启动程序获取自身目录会出错
 * *********************************************************/
int GetModeleFilePath()
{
    char link[100];

    sprintf( link, "/proc/%d/exe", getpid() ); 
    int i = readlink( link, m_FilePath, sizeof(m_FilePath) );
    char *path_end = NULL;
   if(i > 0) 
   {
        path_end = strrchr( m_FilePath, '/');  //"path/a.out" 从右开始找‘/',砍掉文件得到文件目录
        if( path_end != NULL)
        {    
            *(path_end+1) = '\0';                             
			return 0;
        }    
   }
   return -1;
}

/*********************************************************
 * * 函 数 名: MyDirExist
 * * 功能描述: 判断目录是否存在
 * * 参数说明: path:目录路径
 * * 返 回 值：0:目录存在
 * * 备    注: 		   
 * *********************************************************/
int MyDirExist(UCHAR * path)
{
	struct stat statBuf = {0};
    int pbExist = -1;
	if(NULL == path)
	{
		printf("[%s(),%d] path is NULL!!!", __FUNCTION__, __LINE__);
		return -1;
	}
	memset(&statBuf, 0, sizeof(statBuf));
	if(0 != stat(path, &statBuf))
	{
		return -2;
	}

	if(S_ISDIR(statBuf.st_mode))
		pbExist = 0;
    return pbExist;
}

/*********************************************************
 * * 函 数 名: MyMkdir
 * * 功能描述: 创建多级目录
 * * 参数说明: muldir:目录
 * * 返 回 值：
 * * 备    注:  
 * *********************************************************/
static void MyMkdir(UCHAR *muldir) 
{
    int i,len;
    char str[512];    
    strncpy(str, muldir, 512);
    len=strlen(str);
    for( i=0; i<len; i++ )
    {
        if( str[i]=='/' )
        {
            str[i] = '\0';
            if( access(str,0)!=0 )
            {
                mkdir( str, 0755 );
            }
            str[i]='/';
        }
    }
    if( len>0 && access(str,0)!=0 )
    {
        mkdir( str, 0777 );
    }
    return;
}


/*********************************************************
 * * 函 数 名: MyCopyFile
 * * 功能描述: 复制文件
 * * 参数说明: src: 源文件路径
 *            dst：目的文件路径
 * * 返 回 值：
 * * 备    注: 路径后面跟着文件名	   
 * *********************************************************/
int MyCopyFile(UCHAR* src, UCHAR* dst)
{
    FILE* fp = NULL;
    FILE* fout = NULL;
    UCHAR tmpBuffer[MY_FILE_BUFF_SIZE] = { 0 };
    int sReadNum = 0, i, s32Ret;
    int bExist;

    if (NULL == src || NULL == dst)
    {
        printf("[%s(), %d] src or dst is NULL!!!", __FUNCTION__, __LINE__);
        return -1;
    }

    fp = fopen(src, "rb");
    if (NULL == fp)
    {
        printf("[%s(), %d] open src file %s error!!!", __FUNCTION__, __LINE__, src);
        return -1;
    }
    strncpy(tmpBuffer, dst, strlen(dst));
    i = strlen(dst) - 1;
    while ('/' != tmpBuffer[i])
    {
        tmpBuffer[i] = 0;
        i--;
    }
    s32Ret = MyDirExist(tmpBuffer);
    if (0 != s32Ret)
    {
        MyMkdir(tmpBuffer);
    }

    fout = fopen(dst, "wb");
    if (NULL == fout)
    {
        printf("[%s(), %d] open dst file %s error!!!", __FUNCTION__, __LINE__, dst);
        return -1;
    }
    if (0 != fseek(fp, 0, SEEK_SET))
    {
        printf("[%s(), %d] fseek error!!!", __FUNCTION__, __LINE__);
        return -1;
    }

    while (!feof(fp))
    {
        memset(tmpBuffer, 0, MY_FILE_BUFF_SIZE);
        sReadNum = fread(tmpBuffer, 1, MY_FILE_BUFF_SIZE, fp);
        fwrite(tmpBuffer, 1, sReadNum,fout);
    }

    fclose(fp);
    fclose(fout);
    return 0;
}

/*********************************************************
 * * 函 数 名: MyCreateDir
 * * 功能描述: 创建目录,如果存在则退出
 * * 参数说明: path:目录
 * * 返 回 值：
 * * 备    注: 	不存在则创建	   
 * *********************************************************/
int MyCreateDir(const UCHAR *path)
{
    UINT s32Ret = -1;
    UCHAR *ps8TmpPath = NULL, *ps8Ptr;
    int bExist = -1;

    if (!path)
    {
        printf("[%s(), %d] path is NULL", __FUNCTION__, __LINE__);
        goto end;
    }

    ps8TmpPath = calloc(1, strlen(path)+1);
    if (!ps8TmpPath)
    {
        printf("[%s(), %d] out of memory", __FUNCTION__, __LINE__);
        s32Ret = -1;
        goto end;
    }

    strcpy(ps8TmpPath, path);
    bExist = MyDirExist(ps8TmpPath);
    if (!bExist)
    {
        s32Ret = 0;
        printf("file existence\n");
        goto end;
    }
    else
    {
        MyMkdir(ps8TmpPath);
    }

end:
    if (ps8TmpPath)
    {
        free(ps8TmpPath);
        ps8TmpPath = NULL; //好习惯
    }
    return s32Ret;
}

/*********************************************************
 * * 函 数 名: MyCopyDir
 * * 功能描述: 复制目录
 * * 参数说明:  src: 源文件路径
 *              dst：目的文件路径
 * * 返 回 值：
 * * 备    注: 	dst后面跟着要目的目录名 
 * *********************************************************/
int MyCopyDir(char* src, char* dst)
{
    char srcPath[MY_FILE_PATH_DEPTH] = { 0 };
    char dstPath[MY_FILE_PATH_DEPTH] = { 0 };
    struct stat statBuf = { 0 };
    struct dirent *pDirent = NULL;
    DIR* pdir = NULL;

    if ((NULL == src) || (NULL == dst))
    {
        printf("[%s(), %d] src or dst is NULL!!!", __FUNCTION__, __LINE__);
       return -1;
    }

    memset(&statBuf, 0, sizeof(statBuf));
    if (0 != stat(src, &statBuf))
    {
        printf("[%s(), %d] path of src %s is not exist!!!", __FUNCTION__, __LINE__, src);
        return -1;
    }

    if (S_ISREG(statBuf.st_mode))
    {
        MyCopyFile(src, dst);
        return 0;
    }

    if (S_ISDIR(statBuf.st_mode))
        mkdir(dst, MY_FILE_DIRECTORY_MODE);
    pdir = opendir(src);
    if (NULL == pdir)
    {
        printf("[%s(), %d] open dir error!!!", __FUNCTION__, __LINE__);
        return -1;
    }

    while ((pDirent = readdir(pdir)) != NULL)
    {
        if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0)
            continue;

        memset(srcPath, 0, MY_FILE_PATH_DEPTH);
        memset(dstPath, 0, MY_FILE_PATH_DEPTH);
        strcpy(srcPath, src);
        srcPath[strlen(srcPath)] = '/';
        strcpy(srcPath+strlen(srcPath), pDirent->d_name);
        strcpy(dstPath, dst);
        dstPath[strlen(dstPath)] = '/';
        strcpy(dstPath+strlen(dstPath), pDirent->d_name);

        if (-1 == MyCopyDir(srcPath, dstPath))
            break;
    }

    if (0 != closedir(pdir))
    {
        printf("[%s(), %d] close dir!!!", __FUNCTION__, __LINE__);
        return -1;
    }
    return 0;
}

/*********************************************************
 * * 函 数 名: MyMoveDir
 * * 功能描述: 移动目录
 * * 参数说明:  src: 源文件路径
 *              dst：目的文件路径
 * * 返 回 值：
 * * 备    注: 	dst后面跟着要目的目录名 
 * *********************************************************/
int MyMoveDir(char* src, char* dst)
{
    if ((NULL == src) || (NULL == dst))
    {
        printf("[%s(), %d] src or dst is NULL!!!", __FUNCTION__, __LINE__);
        return -1;
    }

    if (0 != rename(src, dst))
    {
        printf("[%s(), %d] move dir error!!!", __FUNCTION__, __LINE__);
        return -1;
    }

    return 0;
}

/*********************************************************
 * * 函 数 名: MyGetDateTime
 * * 功能描述: 获取时间
 * * 参数说明:           
 * * 返 回 值：
 * * 备    注: 	获取到时间后需要free地址
 * *********************************************************/
char* MyGetDateTime()
{
    char timebuf[64];
    struct tm *ptm;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    ptm = gmtime(&tv.tv_sec);
    snprintf
    (
        timebuf,
        sizeof(timebuf),
        "%04d-%02d-%02dT%02d:%02d:%02d.%07ldZ",
        ptm->tm_year+1900,
        ptm->tm_mon+1,
        ptm->tm_mday,
        ptm->tm_hour,
        ptm->tm_min,
        ptm->tm_sec,
        tv.tv_usec
    );

    return strdup(timebuf);
}
#define _TEST
#ifndef _TEST
int main()
{
    char sPath[1024] = {0};
    char tmppath[1280] = "/home/hber/work/szg_src/SzgSrcModule/tt";
    char *str = NULL;

    //str = MyGetDateTime();
    //printf("data=%s\n", str);
    //free(str);

    //MyMoveDir("aa", "tmm/aa");
    //MyCopyDir("aa", "data/ll");

#if 0 //分割字符串
    int count;
    char **splits;
    splits = MyStringSplits(tmppath, strlen(tmppath), "/", 1, &count);

    for (int i = 0; i < count; i++) {
        printf("[%d]:%s\n",i, splits[i]);
    }
    MySplitsFree(splits, count);
    printf("\n");
    
#endif
#ifdef TEST
	if (0 != GetModeleFilePath()) {
        printf("err:path=%s\n", m_FilePath);
        return -1;
    }
	
    sprintf(sPath, "%s/tmm/tt/t.c", m_FilePath);
    printf("path=[%s]\n", sPath);

    if (0 == MyCreateDir("./aa/bb"))
    {
        printf("MyCreateDir ok\n");
    }
 
    MyCopyFile("MyFile.h", "./data/m.h");
    MyMoveFile("a.out", "data/a.i");
    MyDeleteFile("data/m.h");
#endif  


}

#endif 


