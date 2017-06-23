#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#pragma comment(lib,"ws2_32.lib")

#define TCP_PORT 8010
#define UDP_PORT 8020
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512

using namespace std;

//基于TCP协议
void TCP_SERVER() {
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	//检查WinSock
	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		printf("Init socket dll error!");
		exit(1);
	}
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		printf("ERROR");
		exit(1);
	}

	//创建TCP协议套接字
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == sockSrv) {
		printf("Create Socket Error!");
		exit(1);
	}
	
	//设置IP地址和端口号
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//接收来自任何IP地址的请求
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(TCP_PORT);//监听8010端口
	
	//绑定套接字
	if (SOCKET_ERROR == ::bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR))) {
		printf("Server Bind Failed:%d", WSAGetLastError());
		exit(1);
	}
	
	//监听8010端口
	if (SOCKET_ERROR == listen(sockSrv, 5)) {
		printf("TCP Server Listen Failed:%d", WSAGetLastError());
		exit(1);
	}
	
	SOCKADDR_IN addrClient;
	int len = sizeof(addrClient);

	printf("TCP Listening To Client... ");
	while (1) {
		//等待客户端的连接请求
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
		if (SOCKET_ERROR == sockConn) {
			printf("Server Accept Failed:%d", WSAGetLastError());
			break;
		}
		char recvBuf[BUFFER_SIZE];
		memset(recvBuf, 0, BUFFER_SIZE);
	
		//接收数据
		if (recv(sockConn, recvBuf, BUFFER_SIZE, 0) < 0) {
			printf("Server Receive Data Failed!");
			break;
		}
	
		char file_name[FILE_NAME_MAX_SIZE + 1];
		memset(file_name, 0, FILE_NAME_MAX_SIZE);
		strncpy(file_name, recvBuf, strlen(recvBuf) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(recvBuf));
		printf("%s\n", file_name);
	
		//发送数据
		FILE*fp = fopen(file_name, "rb");
		if (NULL == fp) {
			printf("File:%s Not Found\n", file_name);
		}
		else {
			memset(recvBuf, 0, BUFFER_SIZE);
			int length = 0;
			while ((length = fread(recvBuf, sizeof(char), BUFFER_SIZE, fp)) > 0) {
				if (send(sockConn, recvBuf, length, 0) < 0) {
					printf("Send File:%s Failed\n", file_name);
					break;
				}
				memset(recvBuf, 0, BUFFER_SIZE);
			}
			fclose(fp);
			printf("File:%s Transfer Successful\n", file_name);
		}
		closesocket(sockConn);

		printf("TCP Listening To Client... UDP Listening To Client...\n");
	}
	closesocket(sockSrv);
	WSACleanup();
}

//基于UDP协议
void UDP_SERVER() {
	Sleep(100);
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	//检查WinSock
	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		printf("Init socket dll error!");
		exit(1);
	}
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		printf("ERROR");
		exit(1);
	}

	//创建UDP协议套接字
	SOCKET sockSrv = socket(AF_INET, SOCK_DGRAM, 0);
	if (SOCKET_ERROR == sockSrv) {
		printf("Create Socket Error!");
		exit(1);
	}

	//设置IP地址和端口号
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//接收来自任何IP地址的请求
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(UDP_PORT);//监听8020端口

	//绑定套接字
	if (SOCKET_ERROR == ::bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR))) {
		printf("Server Bind Failed:%d", WSAGetLastError());
		exit(1);
	}

	SOCKADDR_IN addrClient;
	int len = sizeof(addrClient);

	printf("UDP Listening To Client...\n");
	while (1) {
		char recvBuf[BUFFER_SIZE];
		memset(recvBuf, 0, BUFFER_SIZE);

		//接收数据
		if (recvfrom(sockSrv, recvBuf, BUFFER_SIZE, 0, (SOCKADDR*)&addrClient, &len) < 0) {
			printf("Server Receive Data Failed!");
			break;
		}

		char file_name[FILE_NAME_MAX_SIZE + 1];
		memset(file_name, 0, FILE_NAME_MAX_SIZE);
		strncpy(file_name, recvBuf, strlen(recvBuf) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(recvBuf));
		printf("%s\n", file_name);

		//发送数据
		FILE*fp = fopen(file_name, "rb");
		if (NULL == fp) {
			printf("File:%s Not Found\n", file_name);
		} else {
			memset(recvBuf, 0, BUFFER_SIZE);
			int length = 0;
			while ((length = fread(recvBuf, sizeof(char), BUFFER_SIZE, fp)) > 0) {
				if (sendto(sockSrv, recvBuf, length, 0, (SOCKADDR*)&addrClient, sizeof(SOCKADDR)) < 0) {
					printf("Send File:%s Failed\n", file_name);
					break;
				}
				memset(recvBuf, 0, BUFFER_SIZE);
			}
			fclose(fp);
			printf("File:%s Transfer Successful\n", file_name);
		}
		printf("TCP Listening To Client... UDP Listening To Client...\n");
	}
	closesocket(sockSrv);
	WSACleanup();
}

//主函数：创建两个独立线程，分别使用TCP协议和UDP协议
void main() {
	thread taskTCP(TCP_SERVER);
	thread taskUDP(UDP_SERVER);
	taskTCP.detach();
	taskUDP.detach();
	while (1) {

	}
}



