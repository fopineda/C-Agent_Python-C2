/**
 * sys.c contains all of the functionality for the system commands.
 * @file sys.c
 */

#include <sys.h>

#define PLACEHOLDER "replace"
#define PLACEHOLDER_SIZE 7

// processlist helper functions
static uint32_t is_pid_folder(char *dir);
static uint32_t parse_pid_stat(char *proc_pid_dir, pid_stat_t *pid_stats);
static uint32_t parse_pid_owner(char *proc_pid_dir, pid_stat_t *pid_stats);
static uint32_t parse_pid_cmdline(char *proc_pid_dir, pid_stat_t *pid_stats);
static uint32_t return_plist_line(char *proc_pid_dir, char *proc_line);

// netstat helper functions
static uint32_t parse_conn_line(char *line, tcp_conn_t *connections);
static char *hex_to_ipaddr(u_int32_t ip_h);
static void free_connections(tcp_conn_t *connections);
static void print_connections(tcp_conn_t *connections, char *print_line);


/************************INSTRUCTIONS*****************************

Build 4 commands:
    hostname                Returns hostname
    invoke                  Returns shell command output
    proclist                Returns process list or proc info
    netstat                 Returns netstat

Hostname Functions:
    hostname_command

Invoke Functions
    invoke_command

Proclist Functions:
    process_list_command
        is_pid_folder
        return_pslist
            parse_pid_stat
            parse_pid_owner
            parse_pid_cmdline

Netstat Functions:
    get_netstat_command
        parse_conn_line
            hex_to_ipaddr
        print_connections
        free_connections

If you want to make use of the test harness, do not remove the MACROs. 
The macros will perform the required functionality for the function. 
See the defined macros in sys.h for more information.

*****************************END**********************************/

/**
 * @brief Returns hostname of host machine
 * @param cmd (in) a pointer to the command
 * @return Response where the message is the hostname
 * @return HOSTNAME_ERROR_MSG on error
 */
Response *hostname_command(Command *cmd)
{
    uint32_t ret_code;
    char hostname[BUFF_MAX + 1];
    Response *rsp = NULL;

    // Get hostname
    HOSTNAME_MACRO(hostname, BUFF_MAX + 1, ret_code)
    // remove the PLACEHOLDER values and put the hostname string and its size
    if(ret_code == 0){
        rsp = alloc_response(0, hostname, strlen(hostname));
    }
    else{
        char *ermsg = "error: failed to get hostname";
        rsp = alloc_response(1, ermsg, strlen(ermsg));
    }
    return rsp;
}

/**
 * @brief Builds formatted process list in one heap buffer and passed to Response
 * arg[1] = "" (empty) - Return full process list
 * arg[1] = pid - Return process information for just 1 pid
 * 
 * @param cmd (in) Command selected with args
 * @return Response* Formatted process list on success
 * @return Response* PROCESS_ERROR_MSG on error
 */
Response *process_list_command(Command *cmd)
{
    
    
    Response *rsp = alloc_response(0, PLACEHOLDER, PLACEHOLDER_SIZE + 1);    
    return rsp;


// ----------------------------
    // struct dirent* ent;
    // Response *rsp = NULL;
    // char prints [BUFF_MAX];
    // int print_length = 0;
    // int offset = 0;
    // pid_stat_t *singular_process = calloc(1, sizeof(pid_stat_t));

    // // Use this Format for your output header to (needed to pass the test)
    // // Uncomment the line below
    // char*  process_list_buff = malloc(sizeof(char*)*BUFF_MAX);
    // snprintf(process_list_buff, PROCESS_LINE_MAX + 1, "%-10s %-8s %-8s %-20s %s\n", "OWNER", "PID", "PPID", "PNAME", "CMDLINE");

    // // Return process list for just 1 pid
    //  // or
    // // process all directories
    // // Return process list

    // DIR* proc = opendir(PROC_DIR);

    // if (proc == NULL){
    //     perror("opendir(/proc)");
    //     rsp = alloc_response(1, PROCESS_ERROR_MSG, strlen(PROCESS_ERROR_MSG));
    //     free(singular_process);
    //     return rsp;
    // }


    // printf("\n  cmd.cmd: %s", cmd->cmd);
    // printf("\n  cmd.len: %d", cmd->cmd_len);
    // printf("\n  cmd.args: %s", cmd->args);
    // printf("\n  cmd.args_len: %d", cmd->args_len);

    // if (cmd->args_len <= 1){  // no pid specified
    //     while (ent = readdir(proc)){
            
    //         printf("\n  ent.d_name: %s", ent->d_name);
            
    //         if (is_pid_folder(ent->d_name) == 0){ // if pid  had non-digit numbers in it
    //             continue;            
    //         }
            
    //         printf("\n  inside while loop after if statement");
            
    //         print_length = return_plist_line(ent->d_name, prints);
    //         // strcat(process_list_buff, prints);
    //         // offset += print_length;

    //     }
    // }
    // else{ // pid was specified
    //     // singular_process
    // }

    
    // rsp = alloc_response(0, PLACEHOLDER, PLACEHOLDER_SIZE + 1);   
    // free(process_list_buff); 
    // free(singular_process);
    // free(proc);
    // return rsp;
}

/**
 * @brief Checks if folder contains all digits
 * @param entry (in) dirent structure - returned from readdir
 * @return uint32_t Returns 1 on success, 0 on failure
 */
static uint32_t is_pid_folder(char *dir)
{    
    return 1;
    //-------------------
    // // if the folder has other non-digit characters in the name
    // if (!isdigit(*dir)){
    //     printf("\n  not a digit");
    //     return 0;
    // }

    // // success
    // return 1;
}

/**
 * @brief Loads struct pid_stat_t with information for 1 process from /proc
 *  processid -> pid
 *  parentpid -> ppid
 *  process name -> comm
 *  State of process -> state
 *
 * @param proc_pid_dir (in) Directory of process
 * @param pid_stats (out) pointer to struct pid_stat_t
 * @return Returns 0 if unable to process information
 * @return Returns 1 if successful
 */
static uint32_t parse_pid_stat(char *proc_pid_dir, pid_stat_t *pid_stats)
{

        // read process statistics /proc/<pid>/stat
    char *raw_pid_stats = NULL;
    uint32_t contents_size = 0;

    /* Open and read /proc/<pid>/stat, Output are statistics, delimited by " "
     *  "man proc" for details */
    READ_PROC_MACRO(proc_pid_dir, &raw_pid_stats, contents_size);

    // load pid_stats with information

    return 1;


    // --------------
    // char* line = NULL;
    // char* name = NULL;
    // // /proc/<pid>/status
    // char* stat_proc_pid_dir = (char *)calloc(50,sizeof(char*)); // TODO come back and fix as it's causing memory leak
    // strcat(stat_proc_pid_dir, PROC_DIR);
    // strcat(stat_proc_pid_dir,"/");
    // strcat(stat_proc_pid_dir, proc_pid_dir);
    // strcat(stat_proc_pid_dir, "/status");
    

    // // read process statistics /proc/<pid>/stat
    // char *raw_pid_stats = NULL;
    // uint32_t contents_size = 0;

    // /* Open and read /proc/<pid>/stat, Output are statistics, delimited by " "
    //  *  "man proc" for details */
    // READ_PROC_MACRO(stat_proc_pid_dir, &raw_pid_stats, contents_size);

    // if (contents_size == 0){ // unable to parse
    //     free(stat_proc_pid_dir);
    //     free(raw_pid_stats);
    //     return 0;
    // }

    // // load pid_stats with information
    // // pid_stats->state = raw_pid_stats;
    // // printf("\n  pid_stats->state: %d", pid_stats->state);

    // // printf("\n  raw_pid_stats: %s", raw_pid_stats);

    // // ex: name: blah
    // memcpy(pid_stats->comm, strtok(raw_pid_stats, "\n"), FILEPATH_MAX + 1);
    // strtok(NULL, "\n"); // throw away umask
    
    // memcpy(pid_stats->state, strtok(NULL, "\n"), FILEPATH_MAX + 1);
    // strtok(NULL, "\n"); // throw away tgid
    // strtok(NULL, "\n"); // throw away ngid
    // memcpy(pid_stats->pid, strtok(NULL, "\n"), sizeof(uint32_t));
    // memcpy(pid_stats->ppid, strtok(NULL, "\n"), sizeof(uint32_t)); 


    // // printf("\n  pid_stats->comm: %s", pid_stats->comm);
    // // printf("\n  pid_stats->state: %s", pid_stats->state);
    // // printf("\n  pid_stats->pid: %d", pid_stats->pid);   
    // // printf("\n  pid_stats->ppid: %d", pid_stats->ppid);


    // // printf("\n  line: %s", line);


    // // TODO come back and fix as it's causing memory leak
    // free(stat_proc_pid_dir);
    // free(raw_pid_stats);

    // return 1;
}

/**
 * @brief Opens /proc/<pid>/loginuid to determine owner of process.
 * Sometimes loginuid returns -1 -> sets owner to "unk"
 * Loads pid_stats->owner with the owner of the process
 * NOTE: Loginuid isn't always accurate to the owner of the process
 * 
 * @param proc_pid_dir (in) loginuid path (ex. /proc/<pid>/loginuid)
 * @param pid_stats (out) struct with process information - defined in sys.h
 * @return uint32_t returns 0 if unable to parse owner
 * @return uint32_t returns 1 on success
 */
static uint32_t parse_pid_owner(char *proc_pid_dir, pid_stat_t *pid_stats)
{

    // Initialize Variables
    char *owner = NULL;
    uint32_t contents_size = 0;

    // call getpwuid(uid)
    READ_LOGINID_MACRO(proc_pid_dir, &owner, contents_size);
    return 1;


    // -----------------------------------------

    // // Initialize Variables
    // char *owner = NULL;
    // uint32_t contents_size = 0;

    // // /proc/<pid>/loginuid
    // char* owner_proc_pid_dir = (char *)calloc(50,sizeof(char*)); // TODO come back and fix as it's causing memory leak
    // strcat(owner_proc_pid_dir, PROC_DIR);
    // strcat(owner_proc_pid_dir,"/");
    // strcat(owner_proc_pid_dir, proc_pid_dir);
    // strcat(owner_proc_pid_dir, "/loginuid");

    // // call getpwuid(uid)
    // READ_LOGINID_MACRO(owner_proc_pid_dir, &owner, contents_size);

    // if (contents_size == 0){ // unable to parse
    //     free(owner_proc_pid_dir);
    //     free(owner);
    //     return 0;
    // }

    // memcpy(pid_stats->owner, owner, contents_size);
    // printf("\n  pid_stats->owner: %s", pid_stats->owner);

    // free(owner_proc_pid_dir);
    // free(owner);
    // return 1;
}

/**
 * @brief Opens /proc/<pid>/cmdline to get the exact command that kicked off the process
 * Loads pid_stats->cmdline with the command
 * 
 * @param proc_pid_dir (in) path to cmdline (ex. /proc/<pid>/cmdline)
 * @param pid_stats (out) struct with process information - defined in sys.h
 * @return uint32_t returns 0 if unable to parse cmdline
 * @return uint32_t returns 1 on success
 */
static uint32_t parse_pid_cmdline(char *proc_pid_dir, pid_stat_t *pid_stats)
{
     // Initialize variables
    char *cmdline = NULL;
    uint32_t contents_size = 0;

    // read proc_dir and returns the cmdline string
    READ_CMDLINE_MACRO(proc_pid_dir, &cmdline, contents_size);
    return 1;

    // ---------------------------------------

    // // Initialize variables
    // char *cmdline = NULL;
    // uint32_t contents_size = 0;

    // // read proc_dir and returns the cmdline string
    // READ_CMDLINE_MACRO(proc_pid_dir, &cmdline, contents_size);
    // return 1;
}

/**
 * @brief Returns a formatted line with all corresponding pid information 
 * @param pid (in) process id (string of digits)
 * @param proc_line (out) formatted string with all process information
 * @return uint32_t length of proc_line (0 on failure)
 * *NOTE: Caller needs to free return
 */
static uint32_t return_plist_line(char *pid, char *proc_line)
{

    //parse_pid_stat
    //parse_pid_owner
    //parse_pid_cmdline

    /* Format process line for return
    snprintf(proc_line, PROCESS_LINE_MAX, "%-10s %8d %8d %-20s %s\n",
             pid_stats->owner,
             pid_stats->pid,
             pid_stats->ppid,
             pid_stats->comm,
             pid_stats->cmdline);*/
    return 1;


//------------------------------

    // int stat_return_value = 0;
    // int owner_return_value = 0;
    // int cmdline_return_value = 0;
    // pid_stat_t *singular_process = calloc(1, sizeof(pid_stat_t));
    
    // stat_return_value = parse_pid_stat(pid, singular_process);
    // printf("\n  stat_return_value: %d", stat_return_value); // 0 or 1
    // owner_return_value = parse_pid_owner(pid, singular_process);
    // printf("\n  owner_return_value: %d", owner_return_value);
    // //parse_pid_cmdline

    // /* Format process line for return
    // snprintf(proc_line, PROCESS_LINE_MAX, "%-10s %8d %8d %-20s %s\n",
    //          pid_stats->owner,
    //          pid_stats->pid,
    //          pid_stats->ppid,
    //          pid_stats->comm,
    //          pid_stats->cmdline);*/
    
    // free(singular_process);
    // return 1;
}

/**
 * @brief Calls Popen to execute shell commmand, not built for interactive cmds
 * 
 * @param cmd (in) 
 * @return Response* Returns command output on success or failure
 */
Response *invoke_command(Command *cmd)
{
    Response *rsp = NULL;
    char* errmsg = " ";
    char buf[BUFF_MAX];
    FILE *ptr;
    unsigned int size = 1;
    unsigned int strlength;
    char* buffer = NULL;
    char* temp = NULL;
    // Use the man pages to learn about he Popen command
    // Use the function to invoke the user specified command
    // Read the output from the command into a buffer and return it to the user
    // in a response
    

    ptr = popen(cmd->args,"r");
    if(ptr == NULL){
        rsp = alloc_response(1,errmsg,strlen(errmsg));
        return rsp;
    }
   
   
    while(fgets(buf,sizeof(buf),ptr)!= NULL){
       
        strlength = strlen(buf);
        temp = realloc(buffer,size+strlength);
        if(temp == NULL){
            rsp = alloc_response(1,errmsg,strlen(errmsg));
            return rsp;   
        }
        else{
            buffer = temp;
        }
        strcpy(buffer+ size-1,buf);
        size += strlength;
        
    }
    
    rsp = alloc_response(0,buffer,size-1);
    pclose(ptr);
    free(temp);
    return rsp;
}

/**
 * @brief Opens /proc/net/tcp and sends current tcp connections
 * 
 * @param cmd (in)
 * @return Response* returns formatted tcp socket information on success
 * @return Response* returns NETSTAT_ERROR_MSG on failure
 */
Response *get_netstat_command(Command *cmd)
{
    char *tcp_stream = NULL;
    uint32_t contents_size = 0;
    tcp_conn_t *connections = calloc(1, sizeof(tcp_conn_t));//alloc space for head
    tcp_conn_t *temp = NULL;//calloc(1, sizeof(tcp_conn_t)); //alloc for temp variable
    char* token = NULL;
    char prints [BUFF_MAX];

    // Read /proc/net/tcp and get stat information
    READ_NET_STAT("/proc/net/tcp", &tcp_stream, contents_size);
    
    if(tcp_stream == NULL){ //if stream is null return error
       Response *rsp = alloc_response(1, NETSTAT_ERROR_MSG, sizeof(NETSTAT_ERROR_MSG));
       return rsp;
    }
     
    token = strtok(tcp_stream, "\n"); //catch the first line and set it as the head
    
    token = strtok(NULL, "\n"); //tokinize the next line
   
    parse_conn_line(token, connections); //fill connections 
    
    while(token != NULL){       //while there are still lines in the stream
        
        tcp_conn_t *current = calloc(1, sizeof(tcp_conn_t)); //allocate space for the current connections
        token = strtok(NULL, "\n"); //split the next line
        
        parse_conn_line(token, current); //fill current node
        
        current->next = NULL;   //set next to null
        if(connections == NULL){    //set current as head if no head
            connections = current;
            
        }
        else{                     //add current to end of connections linked list
            tcp_conn_t *lastnode = connections;
            while(lastnode->next != NULL){
                lastnode = lastnode->next;
            }
            lastnode->next = current;
            
        }
        
    }
    
    // Set up header for netstat, use this line for the test harness   
    char*  netstat_buff = malloc(sizeof(char*)*contents_size);
    snprintf(netstat_buff, BUFF_MAX, "%-21s %-21s %-10s %-20s %10s\n", "LOCAL", "REMOTE", "STATUS", "OWNER", "INODE");
    int offset = 0;
    temp = connections;
    while(temp->next != NULL){ //loop through linked list and concatinate formatted lines to netstat_buffer
        
        print_connections(temp,prints);
        
        strcat(netstat_buff,prints);
        offset += strlen(prints);
        temp = temp->next;
    }   
    //free connections
    free_connections(connections);
   
    
    // set up response
    Response *rsp = alloc_response(0, netstat_buff , strlen(netstat_buff)-1);
    free(netstat_buff);
   
    free(tcp_stream);
   
    return rsp;
}

/**
 * @brief Parse network connections from /proc/net/tcp and set relevant fileds in tcp_conn_t struct
 * 
 * @param line (in) Line of network connections from /proc/net/tcp (1 network connection at a time)
 * @param connections (out) linked list structure to hold network info for each connection
 * @return uint32_t returns 1 on success
 * @return uint32_t returns 0 on failure
 */
static uint32_t parse_conn_line(char *line, tcp_conn_t *connections)
{

    struct passwd *pwd = NULL;    
    uint32_t  uid ,localadr, remoteadr;
    
    
    
    if(line == NULL){//if passed a null line return error
        return 0;
    }
    
    //read in lines formatted
    sscanf(line,"%*x: %x:%x %x:%x %x %*x:%*x %*x:%*x %*x %x %*x %ld", &localadr,&(connections->local_port),&remoteadr, &(connections->remote_port), &(connections->state), &uid ,&(connections->inode));
    
    // Get username from uid - calls getpwuid
    GET_UID_MACRO(uid, pwd);
   // char * name = pwd->pw_name;
    
    if(pwd == NULL){
        memcpy(connections->owner,"unknown",7);
    }
    else{
        memcpy(connections->owner,pwd->pw_name,strlen(pwd->pw_name));
    }
   
   
    
    memmove(connections->local_addr,hex_to_ipaddr(localadr),10);
    
    memcpy(connections->remote_addr,hex_to_ipaddr(remoteadr),9);
    

    return 1;
}

/**
 * @brief Format a netstat connection
 * 
 * @param connections (in) structure with connection information - defined in sys.h
 * @param print_line (out) formatted connection line
 */
static void print_connections(tcp_conn_t *connections, char *print_line)
{
    //Format line ("-" makes it left justified)
    //* Need this format for the test harness
    //* Uncomment the line belo
    char* status;
    if(connections->state == 1){ //convert state to correct status message
        status = "ESTAB\0";
    }
    else if(connections->state == 2){
        status = "SYN_SENT\0";
    }
    else if(connections->state == 3){
        status = "SYN_RECV\0";
    }
    else if(connections->state == 4){
        status = "FIN_WAIT1\0";
    }
    else if(connections->state == 5){
        status = "FIN_WAIT2\0";
    }
    else if(connections->state == 6){
        status = "TIME_WAIT\0";
    }
    else if(connections->state == 7){
        status = "CLOSE\0";
    }
    else if(connections->state == 8){
        status = "CLOSE_WAIT\0";
    }
    else if(connections->state == 9){
        status = "LAST_ACK\0";
    }
    else if(connections->state == 10){
        status = "LISTENING\0";
    }
    else if(connections->state == 11){
        status = "CLOSING\0";
    }
    else if(connections->state == 12){
        status = "NEW_SYN_RECV\0";
    }
    else if(connections->state == 13){
        status = "TCP_MAX_STATES\0";
    }
    // format string
    snprintf(print_line, BUFF_MAX, "%15s:%-5d %15s:%-5d %-10s %-20s %10ld\n",
                connections->local_addr,
                connections->local_port,
                connections->remote_addr,
                connections->remote_port,
                status,
                connections->owner,
                connections->inode);
    
    return;
}

/**
 * @brief Free connection linked list
 * 
 * @param connections (in) structure with connection information - defined in sys.h
 */
static void free_connections(tcp_conn_t *connections)
{
    tcp_conn_t *temp = NULL; 
    tcp_conn_t *next = NULL;
    next = NULL;        //set to NULL
    temp = connections; //set to head
    while(temp != NULL){ //loop through and free each node
        next = temp->next;
        free(temp);
        temp = next;
    }
    free(next);
    free(temp);
     // free the header
    return;
} 

/**
 * @brief convert network order hex to ipv4 string notation
 *       This function is useful to make the hex representation found in
 *       /proc/net/tcp human readable
 * 
 * @param ip_h (in) hex value found in /proc/net/tcp
 * @return char* ipv4 string notation
 */
static char *hex_to_ipaddr(uint32_t ip_h)
{    
    struct in_addr addr; //convert hex to IPv4 address

    addr.s_addr= ip_h;
    char * address = inet_ntoa(addr);

    return address;
}
