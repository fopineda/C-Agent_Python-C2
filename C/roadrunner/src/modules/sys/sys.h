/**
 * sys.h contains all of the sys definitions and exports.
 * @file sys.h
 */

#ifndef SYS_H
#define SYS_H

#if defined(SYS_DEBUG) || defined(RR_DEBUG_ALL)
#include <stdio.h>
#define SYS_DEBUG_PRINT(msg, ...) printf(msg, ##__VA_ARGS__);
#else
#define SYS_DEBUG_PRINT(msg, ...) 
#endif

#include <commands.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <files.h>
#include <stdio.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define BUFF_MAX 255
#define FILEPATH_MAX 511
#define CMDLINE_MAX 1023
#define PROCESS_LINE_MAX 2047
#define IP_ADDR_LEN 15
#define OWNER_MAX 19
#define HOSTNAME_CMD_STR "hostname"
#define PROCESS_LIST_CMD_STR "proclist"
#define NETSTAT_CMD_STR "netstat"
#define INVOKE_CMD_STR "invoke"
#define HOSTNAME_ERROR_MSG "error: failed to get hostname"
#define PROCESS_ERROR_MSG "error: failed to get process list"
#define NETSTAT_ERROR_MSG "error: failed to get netstat"
#define TEST_PROC_SIZE 209


// Mocked Test Macros (Mock system calls)
#ifdef TEST
extern bool ERROR_OUT;
#define PROC_DIR "./test_proc"
#define HOSTNAME_MACRO(buffer, size, ret_code)          \
        if (ERROR_OUT)                                  \
        {                                               \
            ret_code = -1;                              \
        }                                               \
        else                                            \
        {                                               \
            ret_code = 0;                               \
            strcpy(buffer, "test_host");                \
        }
#else
#define PROC_DIR "/proc"
#define HOSTNAME_MACRO(buffer, size, ret_code)           \
    ret_code = gethostname(buffer, size);
#endif

#ifdef TEST
#define READ_PROC_MACRO(proc_pid_dir, raw_pid_stats, contents_size)      \
    if (ERROR_OUT)                                        \
    {                                                     \
        contents_size = 0;                                \
    }                                                     \
    else                                                  \
    {                                                     \
        *raw_pid_stats = malloc(TEST_PROC_SIZE + 1);           \
        strcpy((*raw_pid_stats), "55166 (snapd) S 1 55166 55166 0 -1 1077936384 15872 168979 521 189 748 230 381 495 20 0 12 0 4502571 768618496 5184 18446744073709551615 1 1 0 0 0 0 1006254592 0 2143420159 0 0 0 17 0 0 0 6 0 0 0 0 0 0 0 0 0 0"); \
        contents_size = strlen(*raw_pid_stats);                   \
    }
#else
#define READ_PROC_MACRO(proc_pid_dir, raw_pid_stats, contents_size) \
    contents_size = read_file(proc_pid_dir, raw_pid_stats);
#endif

#ifdef TEST
#define READ_LOGINID_MACRO(proc_pid_dir, owner, contents_size)          \
    if (ERROR_OUT)          \
    {           \
        contents_size = 0;          \
    }           \
    else            \
    {           \
        *owner = malloc(2);         \
        strcpy((*owner), "0");          \
        contents_size = strlen(*owner);          \
    }           
#else
#define READ_LOGINID_MACRO(proc_pid_dir, owner, contents_size) \
    contents_size = read_file(proc_pid_dir, owner)
#endif

#ifdef TEST
#define READ_CMDLINE_MACRO(proc_pid_dir, cmdline, contents_size) \
    if (ERROR_OUT) \
    { \
        contents_size = 0; \
    } \
    else \
    { \
        *cmdline = calloc(sizeof(char), 21); \
        strncpy((*cmdline), "/usr/lib/snapd/snapd", 21); \
        contents_size = strlen(*cmdline); \
    }
#else
#define READ_CMDLINE_MACRO(proc_pid_dir, cmdline, contents_size) \
    contents_size = read_file(proc_pid_dir, cmdline)
#endif

// NETSTAT MACROS
#ifdef TEST
#define READ_NET_STAT(net_file, tcp_stream, contents_size)\
    if (ERROR_OUT)                          \
    {                                       \
        contents_size = 0;                  \
    }                                       \
    else                                    \
    {                                       \
        *tcp_stream = calloc(sizeof(char), 435);          \
        strcpy((*tcp_stream), "  sl  local_address rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode                          \n                           \
   0: 3500007F:0035 00000000:0000 0A 00000000:00000000 00:00000000 00000000   102        0 40449 1 0000000000000000 100 0 0 10 0                 \n  \
   1: 0100007F:0539 0100007F:DF74 01 00000000:00000000 00:00000000 00000000  1000        0 1532812 1 0000000000000000 20 0 0 10 -1");\
        contents_size = strlen(*tcp_stream);\
    }                                       
#else
#define READ_NET_STAT(net_file, tcp_stream, contents_size) \
    contents_size = read_file(net_file, tcp_stream)
#endif

#ifdef TEST
#define GET_UID_MACRO(uid, pwd) \
    pwd = (struct passwd *) malloc(sizeof(struct passwd)); \
    if (ERROR_OUT) \
    { \
        pwd->pw_name = NULL; \
    } \
    else \
    { \
        pwd->pw_name = "alice"; \
    }
#else
#define GET_UID_MACRO(uid, pwd) \
    pwd = getpwuid(uid);
#endif

#ifdef TEST
#define FREE_MACRO(pwd) \
    free(pwd);
#else
#define FREE_MACRO(pwd)
#endif
                                            
typedef struct tcp_conn_t tcp_conn_t;       

// pulled from man proc
typedef struct pid_stat_t{
    uint32_t pid;                                    //pid
    char comm[FILEPATH_MAX + 1];                //filename
    char state[FILEPATH_MAX + 1];                                 //proc state
    uint32_t ppid;                                   //parent pid
    char cmdline[CMDLINE_MAX + 1];              //cmd executed
    char owner[OWNER_MAX + 1];             //owner of process
}pid_stat_t;



// parsed info from /proc/net/tcp
struct tcp_conn_t{
    char local_addr[IP_ADDR_LEN + 1];
    uint32_t local_port;
    char remote_addr[IP_ADDR_LEN + 1];
    uint32_t remote_port;
    uint32_t state;
    char owner[OWNER_MAX + 1];
    ino_t inode;                                // ino_t is a unsigned long from types.h
    tcp_conn_t *next;
};

Response *hostname_command(Command *cmd);
Response *process_list_command(Command *cmd);
Response *get_netstat_command(Command *cmd);
Response *invoke_command(Command *cmd);

#endif
