#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/wait.h>
#include <limits.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <syslog.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#define BUFF_MAX 256

char cmd[BUFF_MAX];

int correctIp(){
	int sock = socket(PF_INET, SOCK_DGRAM, 0);
	struct ifreq req;
	if (sock < 0) exit(EXIT_FAILURE);
	memset(&req, 0, sizeof(req));
	strncpy(req.ifr_name, "eth0", IF_NAMESIZE - 1);
	if (ioctl(sock, SIOCGIFADDR, &req) < 0){
		close(sock);
		return 0;
	}
	close(sock);
	return strcmp("192.168.137.216", inet_ntoa(((struct sockaddr_in *)&req.ifr_addr)->sin_addr))==0;
}

void setIp(){
	if (correctIp()) sleep (5);
	else system (cmd);
}

void initBuffer(){
	strcpy(cmd, "sudo ifconfig eth0 192.168.137.216 netmask 255.255.255.0 up");
}

void daemonInit(){
	pid_t pid, sid;
	time_t timebuf;
	int fd;
	pid = fork();
	if (pid < 0){
		syslog(LOG_ERR, "forkerror");
		exit(EXIT_FAILURE);
	}
	if (pid > 0) exit(EXIT_SUCCESS);
	openlog("lpudated", LOG_PID, LOG_DAEMON);
	umask(0);
	
	//NO SE CAMBIA DE SESION PARA PODER AFECTAR AL USUARIO

	if ((chdir("/")) < 0){
		syslog(LOG_ERR, "chdir");
		exit(EXIT_FAILURE);
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

int main(int argc, char** argv){
	//COPIAR COMANDO A cmd BUFFER
	initBuffer();
	//INICIALIZAR DAEMON
	daemonInit();
	//SET IP
    while(1) setIp();
	//TERMINAR
   	closelog();
	exit(EXIT_SUCCESS);
}
