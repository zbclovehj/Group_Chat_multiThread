#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>
#include <process.h>
#include<windows.h>
#pragma comment(lib, "ws2_32.lib")
SOCKADDR_IN addr_c = { 0 };
int len = sizeof(addr_c);
//通过数组来存储每个客户端的socket
SOCKET clientSocket[1024];
int count = 0;
HANDLE mutex;

void connecting(void* param) {
    int idx = *((int*)param);
    char buff[1024];
    int r;
    while (1) {

        r = recv(clientSocket[idx], buff, 1023, NULL);
        if (r > 0) {
            buff[r] = 0;
            printf("%d:%s\n", idx, buff);

            // 广播数据，实现多人群聊，向所有用户转发信息,
            // 加锁，防止每个用户的信息顺序不一致
            WaitForSingleObject(mutex, INFINITE);
            //向每个客户端发送群聊信息
            for (int i = 0; i < count; ++i) {
                send(clientSocket[i], buff, strlen(buff), NULL);
            }

            // 释放锁
            ReleaseMutex(mutex);
        }
    }
}

int main() {
    //1 请求协议版本
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    //判断版本是否正确
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        printf("请求协议版本失败！\n");
        return -1;
    }
    //2 创建服务器的socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SOCKET_ERROR == serverSocket)
    {
        printf("创建socket失败！\n");
        WSACleanup();
        return -2;
    }
    printf("创建socket成功！");
    //3 创建协议地址族
    SOCKADDR_IN addr_server = { 0 };
    addr_server.sin_family = AF_INET;
    addr_server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    /*
    ‌小端模式（Little - endian）‌是指数据的高字节保存在内存的高地址中，
    而数据的低字节保存在内存的低地址中。
    这种存储模式将地址的高低和数据位权值有效地结合起来，
    使得低地址部分权值低，高地址部分权值高。‌
    大端模式（Big - endian）‌是指数据的高字节保存在内存的低地址中，
	而数据的低字节保存在内存的高地址中。这种存储模式类似于将数据当作字符串顺序处理，
	地址由小向大增加，而数据从高位往低位放。
*/
//os内核和其他程序会占用掉一些端口,所以不能乱用
    addr_server.sin_port = htons(10086);
    // 4 将socket与服务器地址端口绑定
    int r = bind(serverSocket, (sockaddr*)&addr_server, sizeof(addr_server));
    if (-1 == r)
    {
        printf("bind失败！");
        closesocket(serverSocket);
        WSACleanup();
        return -3;
    }
    // 5 监听
    r = listen(serverSocket, 10);
    if (-1 == r)
    {
        printf("listen失败！");
        closesocket(serverSocket);
        WSACleanup();
        return -3;
    }
    printf("listen成功！");
    //6 等待客户端连接 阻塞等待 

    // 接受客户端连接并生成一个与相应客户端通信的socket，并获取客户端的端口与地址

    // 创建互斥锁
    mutex = CreateMutex(NULL, FALSE, NULL);

    // 多线程处理客户端连接
    while (1) {
        clientSocket[count] = accept(serverSocket, (sockaddr*)&addr_c, &len);
        if (clientSocket[count] == SOCKET_ERROR) {
            printf("服务器宕机了！\n");
            //清除协议信息
            WSACleanup();
            return -2;
        }
        printf("有客户端连接！地址为：%s\n", inet_ntoa(addr_c.sin_addr));

        // 创建与客户端通信的线程
    
        unsigned int threadId;
        int* idx = new int(count); // 为每个线程传递不同的索引

        // 安全复制索引
        int* safe_idx = new int(*idx);

        _beginthreadex(NULL, 0, (_beginthreadex_proc_type)connecting, safe_idx, 0, &threadId);

        count++;
    }

    // 关闭连接、释放资源
    for (int i = 0; i < count; i++) {
        closesocket(clientSocket[i]);
    }

    closesocket(serverSocket);
    WSACleanup();

    // 关闭互斥锁
    CloseHandle(mutex);

    return 0;
}