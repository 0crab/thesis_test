/*
Binary Protocol Reference:
 https://github.com/memcached/memcached/wiki/BinaryProtocolRevamped
 */
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/unistd.h>
#include <cstring>
#include <netinet/in.h>
#include "tracer.h"
#define UNIX_DOMAIN "/home/czl/memcached.sock"
#define TEST_NUM 10000000
#define PACKAGE_NUM 1
#define MEMCACHED_MAX_BUF 8192
#define ST_LEN 100
using namespace std;

char *database[TEST_NUM];

void con_database();

int con_send_package(int num,char * st_buf);

int main(void)
{
    unsigned int connect_fd;
    static struct sockaddr_in srv_addr;
    //create unix socket
    connect_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(connect_fd < 0) {
        perror("cannot create communication socket");
        return 1;
    }
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(11211);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //connect server;
    if( connect(connect_fd, (struct sockaddr*)&srv_addr, sizeof(srv_addr)) == -1) {
        perror("cannot connect to the server");
        close(connect_fd);
        return 1;
    }

    con_database();
    char read_buf[100];
    int readnum;
    memset(read_buf,0, sizeof(read_buf));
    Tracer tracer;
    long runtime=0;
    for(int i=0;i<TEST_NUM;i++){
        tracer.startTime();
        write(connect_fd,database[i],32);
        readnum=read(connect_fd,read_buf,100);
	if(readnum>0) printf("rec:%d\n",readnum);
        while(readnum>=100){
            readnum=read(connect_fd,read_buf,100);
        }
        runtime+=tracer.getRunTime();
    }
    printf("%ld\n",runtime);
    close(connect_fd);
    return 0;
}

void con_database(){
    char * st_buf;
    for(int i=0;i<TEST_NUM;i++){
        st_buf= static_cast<char *>(malloc(50));
        con_send_package(i,st_buf);
        database[i]=st_buf;
    }
}

int con_send_package(int num,char * st_buf){
    //Magic: Request
    st_buf[0]=0x80;
    //keylength
    st_buf[3]=0x08;
    st_buf[11]=0x08;
    unsigned long *keyptr=(unsigned long *)(st_buf+24);
    *keyptr=(unsigned long )num;

    return 32;
}
