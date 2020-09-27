import socket               # 导入 socket 模块

#注意！要在服务器的安全组中开放相应的进出口！
SERVER_PORT = 8000
MAX_LEN = 60000

s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)         # 创建 socket 对象
server_addr = (str(socket.INADDR_ANY),SERVER_PORT) #服务器地址+端口

s.bind(server_addr)         # 服务器启动！ 
print('服务器启动！')

data,client_addr = s.recvfrom(10)   #接收图像数据长度

imgLength = int.from_bytes(data,byteorder='little',signed=True)

print("image Length value is %d"%imgLength)

#创建本地图像文件
imgLocalFile = open("serverImg4Py.bmp",mode = 'wb')

#求解需要接收的图像数据包的个数
packNum = imgLength / MAX_LEN
#求解尾包数据个数
remain = imgLength % MAX_LEN

packNow = 0
while packNow < packNum:
    #接收数据
    data,client_addr = s.recvfrom(MAX_LEN)
    #写图文件
    imgLocalFile.write(data)
    #接收反馈
    s.sendto(packNow.to_bytes(length = 4,byteorder = 'little',signed = True),client_addr)

    packNow += 1

#读取数据
data,client_addr = s.recvfrom(remain)

imgLocalFile.write(data)

#关闭文件
imgLocalFile.close()
