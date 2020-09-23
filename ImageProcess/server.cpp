#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include "imgProcess.h"

#define server_port 8000    //宏定义服务器对外开放端口

#define MAX_LEN 60000

int main()
{
    int socket_fd = socket(AF_INET,SOCK_DGRAM,0);   //创建UDP套接字

    if(socket_fd < 0)   //创建套接字错误
    {        
        std::cout<<"create socket Error!"<<std::endl;
        exit(1);
    }

    struct sockaddr_in addr_server;     //服务器属性
    int len_addr_server = sizeof(addr_server);              //属性结构体长度
    memset(&addr_server,0,sizeof(struct sockaddr_in));      //默认数据填充0
    addr_server.sin_family = AF_INET;                       //IPV4地址类型
    // addr_server.sin_family = AF_INET6;                   //IPV6地址类型
    addr_server.sin_port    = htons(server_port);           //端口设置导入
    addr_server.sin_addr.s_addr = htonl(INADDR_ANY);        //自动获取IP地址

    //绑定socket，也就是启动服务
    if( bind( socket_fd,
                        (struct sockaddr * )&addr_server,
                        sizeof(addr_server) )
             < 0  )
    {
        std::cout <<"bind error!"<<std::endl;
        exit(1);
    }

    struct sockaddr_in cache_addr_client;     //客户端属性存储

    std::cout<<"server waiting:"<<std::endl;

    int recv_num;

    int imgLength = 0;
    //注意这是阻塞性接口！
    recv_num = recvfrom(socket_fd,              //套接字
                        &imgLength,sizeof(imgLength),          //接收缓冲设置
                        0,                          //接收标志
                        (struct sockaddr *)&cache_addr_client,(socklen_t *)&len_addr_server);     //客户端属性缓冲设置

    if(recv_num < 0)        //接收结果判断，错误是-1
    {
        std::cout<<"recvform error!"<<std::endl;
        return -1;
    }
    if(recv_num != 4)
    {
        std::cout<<"receive image length error!"<<std::endl;
        return -1;
    }
    std::cout<<"length of image is:"<<imgLength<<std::endl;

    int total = imgLength/MAX_LEN;
    int remain = imgLength%MAX_LEN;
    std::cout<<"total: "<<total<<std::endl;
    std::cout<<"remain: "<<remain<<std::endl;

    imgData recvBuf;
    recvBuf.length = imgLength;
    recvBuf.ptr = new char[imgLength];
    char * bufNow = recvBuf.ptr;

    FILE* fp = fopen("image.bmp", "wb");
    if(!fp)
    {
        std::cout<<"file system error!"<<std::endl;
        return -1;
    }

    for(int i = 0; i<total;i++)
    {
        recv_num = recvfrom(socket_fd,              //套接字
                        bufNow,MAX_LEN,          //接收缓冲设置
                        0,                          //接收标志
                        (struct sockaddr *)&cache_addr_client,(socklen_t *)&len_addr_server);     //客户端属性缓冲设置
        if(recv_num != MAX_LEN)
        {
            std::cout << "receive image segment length error!(" << i << ")" << std::endl;
        }
        bufNow += MAX_LEN;
    }

        recv_num = recvfrom(socket_fd,              //套接字
                            bufNow,MAX_LEN,          //接收缓冲设置
                            0,                          //接收标志
                            (struct sockaddr *)&cache_addr_client,(socklen_t *)&len_addr_server);     //客户端属性缓冲设置
        if(recv_num != remain)
        {
            std::cout << "receive image segment length error!(at remain)" << std::endl;
        }
    
    fwrite(recvBuf->ptr, 1, recvBuf->length, fp);
    fclose(fp);

    close(socket_fd);   //关闭套接字

    return 0;
}
