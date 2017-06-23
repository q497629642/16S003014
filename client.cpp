#include <Winsock2.h>
#include <stdio.h>

#define TCP_PORT 8010
#define UDP_PORT 8020
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512
#pragma comment(lib,"WS2_32")

char SERVER_IP[20];

/////////////////////
//使用TCP协议进行传输
void TCP_CLIENT() {
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
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (SOCKET_ERROR == sockClient) {
		printf("Create Socket Error!");
		exit(1);
	}
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(SERVER_IP); //设置服务器端IP地址
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(TCP_PORT);//设置端口号

	//连接服务器端
	if (SOCKET_ERROR == connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR))) {
		printf("Can Not Connect To Server IP!\n");
		exit(1);
	}

	//输入文件名
	char file_name[FILE_NAME_MAX_SIZE];
	memset(file_name, 0, FILE_NAME_MAX_SIZE);
	printf("Please Input File Name On Server:\n");
	scanf("%s", &file_name);

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));

	//向服务器发送文件名
	if (send(sockClient, buffer, BUFFER_SIZE, 0) < 0) {
		printf("Send File Name Failed\n");
		exit(1);
	}

	//从服务器端接收数据
	FILE*fp = fopen(file_name, "wb");
	if (NULL == fp) {
		printf("File:%s Can Not Open To Write\n", file_name);
		exit(1);
	}
	else {
		memset(buffer, 0, BUFFER_SIZE);
		int length = 0;
		while ((length = recv(sockClient, buffer, BUFFER_SIZE, 0)) > 0) {
			if (fwrite(buffer, sizeof(char), length, fp) < length) {
				printf("File: %s Write Failed\n", file_name);
				break;
			}
			memset(buffer, 0, BUFFER_SIZE);
		}
		printf("Receive File:%s From Server Successful!\n", file_name);
	}
	fclose(fp);
	closesocket(sockClient);

	WSACleanup();
}

//使用UDP协议进行传输
void UDP_CLIENT() {
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
	SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);
	if (SOCKET_ERROR == sockClient) {
		printf("Create Socket Error!");
		exit(1);
	}
	
	//设置服务器端IP地址和端口号
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(UDP_PORT);

	//输入文件名
	char file_name[FILE_NAME_MAX_SIZE];
	memset(file_name, 0, FILE_NAME_MAX_SIZE);
	printf("Please Input File Name On Server:\n");
	scanf("%s", &file_name);

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));

	//向服务器发送文件名
	if (sendto(sockClient, buffer, BUFFER_SIZE, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) < 0) {
		printf("Send File Name Failed\n");
		exit(1);
	}

	int len = sizeof(SOCKADDR);

	//从服务器端接收数据
	FILE*fp = fopen(file_name, "wb");
	if (NULL == fp) {
		printf("File:%s Can Not Open To Write\n", file_name);
		exit(1);
	}
	else {
		memset(buffer, 0, BUFFER_SIZE);
		int length = 0;
		length=recvfrom(sockClient, buffer, BUFFER_SIZE, 0, (SOCKADDR*)&addrSrv, &len);
		fwrite(buffer, sizeof(char), length, fp);
		memset(buffer, 0, BUFFER_SIZE);
		printf("Receive File:%s From Server Successful!\n", file_name);
	}
	fclose(fp);
	closesocket(sockClient);

	WSACleanup();
}

void main() {
	int n=1;
	char c = 'Y';
	printf("input IP of server:");
	scanf("%s", &SERVER_IP);
	while (1) {
		while (1) {
			printf("choose a protocol:\n1、TCP 2、UDP\n");
			scanf("%d", &n);
			if (n == 1 || n == 2) {
				break;
			} else {
				printf("Please Input 1 or 2!\n");
			}
		}

		if (n == 1) {
			TCP_CLIENT();
		} else if (n == 2) {
			UDP_CLIENT();
		}
	}
}