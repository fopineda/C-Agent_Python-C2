/**
 * proxy.h contains all of the proxy definitions and exports.
 * @file proxy.h
 */

#ifndef PROXY_H
#define PROXY_H

#if defined(PROXY_DEBUG) || defined(RR_DEBUG_ALL)
#include <stdio.h>
#define PROXY_DEBUG_PRINT(msg, ...) printf(msg, ##__VA_ARGS__);
#else
#define PROXY_DEBUG_PRINT(msg, ...) 
#endif

#include <commands.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>

#define PROXY_CMD_STR "proxy"
#define MAX_EPOLL_SIZE 1000
#define RECV_BUFF_SIZE 1000000
#define PROXY_RSP_MSG "started proxy with pid: %d on port: %d"

Response *proxy_command(Command *cmd);

#endif
