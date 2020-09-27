#include <iostream>
#include <stdio.h>
#include <string>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory.h>
#include "imgProcess.h"
#include <errno.h>
#include <unistd.h> 

#define DEST_PORT           8000                    //目标服务器端口
#define DEST_IP_ADDR    "192.168.221.131"     //IP

#define MAX_LEN 60000

using namespace std;

//client [IPv4_address]
int main(int argc,char* argv[])
{
    int socket_fd = socket(AF_INET,SOCK_DGRAM,0);   //创建套接字

    if(socket_fd < 0)   //创建结果检测
    {
        std::cout<<"create socket Error!"<<std::endl;
        return -1;
    }

    struct sockaddr_in addr_server;     //服务器属性
    struct sockaddr_in cache_addr_client;     //客户端属性存储
    int len_addr_server = sizeof(addr_server);
    memset(&addr_server,0,len_addr_server);
    addr_server.sin_addr.s_addr = inet_addr(argv[1]);
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(DEST_PORT);
    

    //------------获取图像数据-------------
    imgData* image = new imgData;
    // cout<<"begin imgpro"<<endl;
    int status = imgProcess(image);
    std::cout<<"status is "<<status<<std::endl;
    //cout<<"end imgpro"<<endl;
    

    //-------------图像存储
    FILE* fp = fopen("imageClientSource.bmp", "wb");
    if (NULL == fp)
    {
        std::cout<<"fopen failed!"<<std::endl;
        return -1;
    }
    cout<<"the len ptr are: "<<&(image->length)<<" the length are: "<<sizeof(image->length)<<endl;
    cout<<"the img ptr are: "<<image->ptr<<" the length are: "<<image->length<<endl;
    fwrite(image->ptr, 1, image->length, fp);
    fclose(fp);
    

    //---------------开始发送图片数据-----------------
    int sendNum;    //发送数据个数反馈
    int recvNum;    //接收数据个数反馈

    //发送图像数据总长度
    sendNum = sendto(socket_fd,
                        &(image->length),sizeof(image->length),
                        0,
                        (struct sockaddr *)&addr_server,len_addr_server);
    if(sendNum < 0)
    {
        std::cout<<"send Length Error!"<<std::endl;
        return -1;
    }
    else{
        std::cout<<"sendNum(image length) is: "<<sendNum<<std::endl;
    }
    

    char* currPtr = (char*)image->ptr;      //当前发送数据位置指针
    int count = 0;  //当前发送数据包号
    int feedbackNow = 0;    //反馈数据包号
    int total = image->length/MAX_LEN;  //总包个数
    int remain = image->length%MAX_LEN;     //尾包余数据量
    std::cout<<"total: "<<total<<std::endl;
    std::cout<<"remain: "<<remain<<std::endl;
    //除法是向下取整，说明总次数是total加１，最后一次单列
    for(count = 0; count < total; count++)
	{
        //发送count号包
        sendNum = sendto(socket_fd,     //套接字
                        currPtr,MAX_LEN,   //发送数据,每次传送规定的大小
                        0,  //标志位
                        (struct sockaddr *)&addr_server,len_addr_server);   //接收者（服务器）地址
        currPtr += MAX_LEN;
        if(sendNum < 0)
        {
            std::cout<<"send Error!"<<std::endl;
            printf("errno is: %d\n",errno);
            return -1;
        }

        //等待接收端响应
        recvNum = recvfrom(socket_fd,
                                &feedbackNow,sizeof(feedbackNow),
                                0,
                                (struct sockaddr *)&addr_server,(socklen_t *)&len_addr_server);
        if(feedbackNow != count)
        {
            std::cout<<"feedBackError!"<<std::endl;
            return -1;
        }

    }
    //最后一次 
    sendNum = sendto(socket_fd,
                    currPtr,remain,
                    0,
                    (struct sockaddr *)&addr_server,len_addr_server);

    
    if(sendNum < 0)
    {
        std::cout<<"send remain Error!"<<std::endl;
    }
    else
    {
        std::cout<<"sendNum remain is: "<<sendNum<<std::endl;
    }
    
    return 0;
}
