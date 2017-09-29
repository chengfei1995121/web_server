#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include<fstream>
#include<string>
#include<cstring>
#include<vector>
#include<map>
#include<process.h>
#include<Windows.h>
using namespace std;
#pragma comment(lib , "ws2_32.lib")
void getfilename(char a[],char filename[])
{
	int k = 1;
	for (int i = 0; i < strlen(a); i++)
	{
		if (a[i] == '/')
		{
			for (int j = i; a[j] != ' '; j++)
				filename[k++] = a[j];
			break;
		}
	}
	filename[k] = '\0';
}
void get_type(char filename[], char type[])
{
	if (strstr(filename, ".html"))
		strcpy(type, "text/html");
	else
	{
		if (strstr(filename, ".gif"))
			strcpy(type, "image/gif");
		else
			if (strstr(filename, ".jpg"))
				strcpy(type, "image/jpeg");
			else
				if (strstr(filename, ".css"))
					strcpy(type, "text/css");
				else
					if (strstr(filename, ".png"))
						strcpy(type, "image/png");
					else
						strcpy(type, "text/plain");
	}
}
void respond(int n,char filename[])
{
	ifstream cf(filename,std::ios::binary);
	char a[1000000];
	char type[1000];
	int i = 0;
	int chang;
	cf.seekg(0, ios::end);
	chang = cf.tellg();
	cf.seekg(0, ios::beg);
	cf.read(a, 1000000);
	//cout << strlen(a) << endl;
	get_type(filename, type);
	char b[1000] = "http/1.0 200 ok\r\nAccept-Ranges:bytes\r\nServer:cf web server\r\nContent-type:";
	strcat(b, type);
	strcat(b, "\r\n\r\n");
	send(n, b, strlen(b), 0);
	int slen = 0;
	if (!strstr(filename, ".ico"))
		slen=send(n,a,chang,0);
	cout << "sizeof:" << chang << endl;
	cout <<"leng:"<< cf.tellg() << endl;
	cout << "slen:" << slen << endl;
}
void handle_request(int n)
{
	char a[10000];
	int l = recv(n, a, 10000, 0);
	a[l] = '\0';
	cout << a << endl;
	char filename[1000];
	filename[0] = '.';
	getfilename(a, filename);
	
	if (filename[strlen(filename) - 1] == '/')
	{
		strcat(filename, "index.html");
	}
	cout << filename << endl;
	if(!strstr(filename,"ico"))
		respond(n,filename);
}
int main(int argc, char **argv)
{
	WSADATA data;
	sockaddr_in client, server;
	WSAStartup(MAKEWORD(2, 2), &data);
	int socketd = socket(AF_INET, SOCK_STREAM, 0);
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	bind(socketd, (sockaddr*)&server, sizeof(server));
	listen(socketd, 10);
	int l = sizeof(client);
	
	while (1) {
		int n = accept(socketd, (sockaddr*)&client, &l);
		handle_request(n);
		closesocket(n);
	}
	closesocket(socketd);
	WSACleanup();
}
/*int main()
{
	char a[100] = "E://www/fav.jpg";
	char c[100] = "E://www/wo.jpg";
	ifstream cf(a,ios::binary);
	int i = 0;
	if (cf.is_open())
	{
		char b[100000];
		cf.read(b, 100000);
		ofstream aa(c, ios::binary);
		if (aa.is_open())
		{
			aa.write(b, 100000);
		}
	}
	else
	{
		cout << "shibai" << endl;
	}
}*/