//
// Created by czl on 4/18/20.
//

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
#define PACKAGE_NUM 80
#define MEMCACHED_MAX_BUF 8192
#define ST_LEN 100
using namespace std;

char *database[TEST_NUM/PACKAGE_NUM];

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
    char read_buf[100*PACKAGE_NUM];
    int readnum;
    memset(read_buf,0, sizeof(read_buf));
    Tracer tracer;
    long runtime=0;
    tracer.startTime();
    for(int i=0;i<TEST_NUM/PACKAGE_NUM;i++){
        write(connect_fd,database[i],32*PACKAGE_NUM);
//        if((i+1)%PACKAGE_NUM==0){
//            readnum=read(connect_fd,read_buf,100*PACKAGE_NUM);
//            while(readnum>=100*PACKAGE_NUM){
//                readnum=read(connect_fd,read_buf,100*PACKAGE_NUM);
//            }
//        }
    }
    runtime+=tracer.getRunTime();
    printf("%ld\n",runtime);
    close(connect_fd);
    return 0;
}

void con_database(){
    char * st_buf;
    for(int i=0;i<TEST_NUM/PACKAGE_NUM;i++){
        st_buf= static_cast<char *>(malloc(50*PACKAGE_NUM));
        memset(st_buf,0,50*PACKAGE_NUM);
        con_send_package(i,st_buf);
        database[i]=st_buf;
    }
}

int con_send_package(int num,char * st_buf){
#define GETQ_OFFSET(i) (i*32)
#define GET_OFFSET   ((PACKAGE_NUM-1)*32)
    //OP:GETQ
    for(int i=0 ;i<PACKAGE_NUM;i++){
        st_buf[GETQ_OFFSET(i)+0]=0x80;
        st_buf[GETQ_OFFSET(i)+1]=0x09;

        st_buf[GETQ_OFFSET(i)+3]=0x08;
        st_buf[GETQ_OFFSET(i)+11]=0x08;
        unsigned long *keyptr=(unsigned long *)(st_buf+GETQ_OFFSET(i)+24);
        *keyptr=(unsigned long )(num*PACKAGE_NUM+i);
    }
    //OP:GET
    //Magic: Request
//    st_buf[GET_OFFSET+0]=0x80;
//    //keylength
//    st_buf[GET_OFFSET+3]=0x08;
//    st_buf[GET_OFFSET+11]=0x08;
//    unsigned long *keyptr=(unsigned long *)(st_buf+GET_OFFSET+24);
//    *keyptr=(unsigned long )(num*PACKAGE_NUM+PACKAGE_NUM-1);


    return 0;
}
