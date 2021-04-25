// ./main -a 1 -b 3
/*
getopt（分析命令行参数）
相关函数
表头文件 #include<unistd.h>
定义函数 int getopt(int argc,char * const argv[ ],const char * optstring);
extern char *optarg;
extern int optind, opterr, optopt;
getopt() 所设置的全局变量包括：
optarg——指向当前选项参数（如果有）的指针。 optind——再次调用 getopt() 时的下一个 argv 指针的索引。 optopt——最后一个未知选项。
*/

#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int c;
    int num1, num2, num3;
    int opterr = 0;
    while((c = getopt(argc, argv, "a:b:c")) != -1) {
        switch(c) {
            case 'a':
                num1 = atoi(optarg);
                printf("===a, num=%d\n", num1);
                break;
            case 'b':
                num2 = atoi(optarg);
                printf("===b, num=%d\n", num2);
                break;
            case 'c':
                num3 = atoi(optarg);
                printf("===c, num=%d\n", num3);
                break;
            default:
                printf("===error\n");
                break;
        }
    }
}
