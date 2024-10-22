#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<windows.h>
#include<graphics.h>//easyX
#include <process.h>
#pragma comment(lib, "ws2_32.lib")

SOCKET clientSocket;
HWND hwnd;
//记录信息数量
int count = 0;
//从服务器接受信息并输出到每个客户的群聊里
void recvive() {

	char recvBuff[1024];
	int r;
	while (1) {
		r = recv(clientSocket, recvBuff, 1023, NULL);
		if (r > 0) {
			recvBuff[r] = 0;
			//将信息输出到界面上，一条信息二十像素间距
		
			outtextxy(0, count * 20, recvBuff);
			count++;
		}
	}

}
int main() {
	//初始化界面
	hwnd = initgraph(400, 400, SHOWCONSOLE);
	//1 请求协议版本
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//判断版本是否正确
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		printf("请求协议版本失败！\n");
		return -1;
	}
	//2 创建客户端的socket
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == clientSocket)
	{
		printf("创建socket失败！\n");
		WSACleanup();
		return -2;
	}
	printf("创建socket成功！");
	//3 获取服务器协议地址族
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
	// 4 connect连接服务器
	int r = connect(clientSocket, (sockaddr*)&addr_server, sizeof(addr_server));
	if (-1 == r)
	{
		printf("连接服务器失败！");
		closesocket(clientSocket);
		WSACleanup();
		return -3;
	}
	printf("连接服务器成功！");
	// 5 通信

	char buff[1024];

	unsigned int threadId;
	//创建客户端的线程，在一个进程当中创建一个线程去处理另外一部分代码逻辑
	_beginthreadex(NULL, 0, (_beginthreadex_proc_type)recvive, NULL, 0, &threadId);
	//向服务器发送聊天信息
	while (1) {
		memset(buff, 0, 1024);
		printf("你想说啥：\n");
		scanf("%s", buff);
		printf("你说：%s\n", buff);
		send(clientSocket, buff, strlen(buff), NULL);
		
	}
	return 0;
}