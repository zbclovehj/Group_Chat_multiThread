
# 通过网络编程实现群聊功能

什么叫做网络编程：
	就是多个程序之间能够交换数据
	同一主机上      局域网内      互联网
即时，安全，全双工   tcp协议      ip协议  

tcp协议：通信的方式
ip：就是地址  用来区分主机  

windows操作系统上用TCP协议  bs架构 
	服务器  server                
	1.请求协议版本					
	2.创建socket					
	3.创建协议地址族				
		ip地址
		网络端口
		通信协议
	4.绑定
	5.监听
	6.等待客户端连接				
	7.通信						
	8.关闭socket					
	9.清理协议					
客户端 client
1.请求协议版本
2.创建socket 
4.连接服务器
5.通信
6.关闭socket
7.清理协议
客户端和服务器之间通信

多客户端通信：
	1.服务器要接收多个客户端的连接
	2.多线程来解决多客户端通信问题
		2.1 有客户端连接到服务器，启动一个线程来和这个客户端通信
		2.2 线程里：
			2.2.1 接收客户端发来的数据
			2.2.2 把数据广播给当前连接到服务器的所有客户端
