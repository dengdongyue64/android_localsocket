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

#define LOG_TAG   "MySocketClientSOCKET_SERVER"
#define SOCKET_PATH "mysocket"
#define BUFFER_MAX    1024
 
static int readx(int s, void *_buf, int count)
{
    char *buf = (char *)_buf;
    int n = 0, r;
    if (count < 0) return -1;
    while (n < count) {
        r = read(s, buf + n, count - n);
        if (r < 0) {
            if (errno == EINTR) continue;
            ALOGE("read error: %s\n", strerror(errno));
            return -1;
        }
        if (r == 0) {
            ALOGE("eof\n");
            return -1; /* EOF */
        }
        n += r;
    }
    return 0;
}

static int writex(int s, const void *_buf, int count)
{
    const char *buf = (const char *)  _buf;
    int n = 0, r;
    if (count < 0) return -1;
    while (n < count) {
        r = write(s, buf + n, count - n);
        if (r < 0) {
            if (errno == EINTR) continue;
            ALOGE("write error: %s\n", strerror(errno));
            return -1;
        }
        n += r;
    }
    return 0;
}
static void execCMD(char* command ,char * result)
{
	int iLen = 0;
	int iRet = 0;
	if (command == NULL)
		return ;
    FILE *fpRead;
    strcat(command," 2>&1");
    fpRead = popen(command, "r");
    ALOGD("command = %s\n", command);

	iRet = fread(result,1,2048,fpRead);
	ALOGD("iRet = %d\n", iRet);
	if(iRet <= 0)
		return ;
	fflush(fpRead);
    if(fpRead!=NULL)
        pclose(fpRead);
    
}
static int bind_socket(int bindsocket ,char *net_node)
{
	if(bindsocket == -1 || net_node == NULL) {
		ALOGD(" get_local_ip_for_with_connect() could not create  socket: -1 or net_node == NULL\n");
		return 0;
	}
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(struct ifreq));
	snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), net_node);
	//ioctl(sock, SIOCGIFINDEX, &ifr);
	if (setsockopt(bindsocket, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(struct ifreq)) < 0)
	{
		ALOGD("22222222bind interface fail, errno: %s \r\n",strerror(errno));
		return -1;
	}
	ALOGD("22222222bind interface ok  successfully \r\n");
	return 0;
}

int main(const int argc, const char *argv[]) {
    char buf[BUFFER_MAX];
    struct sockaddr addr;
    socklen_t alen;
	int bindsocket;
	char *net_node;
    int lsocket, s;
	int ret = 0;
	char * result =(char  *)malloc(2048) ;
    ALOGD("socketserver firing up\n");
    lsocket = android_get_control_socket(SOCKET_PATH);
    if (lsocket < 0) {
        ALOGE("Failed to get socket from environment: %s\n", strerror(errno));
        exit(1);
    }
    if (listen(lsocket, 5)) {
        ALOGE("Listen on socket failed: %s\n", strerror(errno));
        exit(1);
    }
    fcntl(lsocket, F_SETFD, FD_CLOEXEC);

    for (;;) {
        alen = sizeof(addr);
        s = accept(lsocket, &addr, &alen);
        if (s < 0) {
            ALOGE("Accept failed: %s\n", strerror(errno));
            continue;
        }
        fcntl(s, F_SETFD, FD_CLOEXEC);
		memset(result,0,2048);
        ALOGD("new connection\n");
        for (;;) {
            unsigned short count;
            if (readx(s, &count, sizeof(count))) {
                ALOGE("failed to read size\n");
                break;
            }
            if ((count < 1) || (count >= BUFFER_MAX)) {
                ALOGE("invalid size %d\n", count);
                break;
            }
            if (readx(s, buf, count)) {
                ALOGE("failed to read command\n");
                break;
            }
            buf[count] = 0;
            ALOGE("buf = %s  count = %d\n", buf, count);
            if(strstr(buf,"socket") != NULL)
            {
            	char *p = strtok(buf,":");
				net_node= strtok(NULL,":");
				p = strtok(NULL,":");
            	//sscanf(buf,"socket:%d",&bindsocket);
            	bindsocket = atoi(p);
				ALOGE(" bindsocket = %d  bind to  net_node = %s \n", bindsocket,net_node);
				ret = bind_socket(bindsocket,net_node);
				if(ret < 0)
					strcpy(result,"failed");
				else
					strcpy(result,"ok");
            }else{
            	 execCMD(buf,result);       
            }
			count=strlen(result);
            if (writex(s, &count, sizeof(count))) return -1;
            if (writex(s, result, count)) return -1;

        }
        
        ALOGD("closing connection\n");
        close(s);
    }

    return 0;
}

