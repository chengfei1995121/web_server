#include "http.h"
void getfilename(char a[],char filename[])
{
	int k =6;
	for (size_t i = 0; i < strlen(a); i++)
	{
		if (a[i] == '/')
		{
			for (int j = i; a[j] != ' '; j++)
				filename[k++] = a[j];
			break;
		}
	}
	filename[k] = '\0';
	if(filename[strlen(filename)-1]=='/')
		strcat(filename,"index.html");
}
//获取请求类型
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
