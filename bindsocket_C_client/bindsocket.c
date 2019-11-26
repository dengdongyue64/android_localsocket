#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <cutils/sockets.h>
#include <utils/Log.h>
#include <android/log.h>
#include <fcntl.h>
#include <net/if.h>
#define SOCKET_PATH "mysocket"

int  bind_socket(int     lin_socket){
	int android_localsocket = 0;
	char command[30];
	char buffer[1024];
	unsigned short count;

	if (lin_socket == -1) {
		printf("%s:%d get_local_ip_for_with_connect() could not create  socket: %s \n", __FUNCTION__, __LINE__,strerror(errno));
		return -1;
	}
	
	android_localsocket = socket_local_client(SOCKET_PATH,ANDROID_SOCKET_NAMESPACE_RESERVED, SOCK_STREAM);// |SOCK_CLOEXEC);
	if (android_localsocket < 0) 
	 	 return -1;
	memset(command,0,sizeof(command));
	memset(buffer,0,sizeof(buffer));
	sprintf(command,"%s%d","socket:eth0:",lin_socket);
	printf(" command  = %s strlen(command) = %d  \n",command,strlen(command));
	count = strlen(command);
	write(android_localsocket,&count,sizeof(count)); // write command len
	
	write(android_localsocket,command,count); //write command string
	
	read(android_localsocket, &count, sizeof(count));  //read result len
	printf(" reviceve result num  = %d  \n",count);
	read(android_localsocket, buffer, count);     //read result 
	printf(" reviceve result  = %s  \n",buffer);
	close(android_localsocket);
	return 0;
}

int main()
{
	int ret = 0;
	int  lin_socket = socket(AF_INET, SOCK_DGRAM, 0);
	ret = bind_socket(lin_socket);

    return 0;
}


