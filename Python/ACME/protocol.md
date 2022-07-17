## Road Runner Protocol

Below are what each of the Command and Response streams should look like for each of the various functions of RoadRunner.

**NOTE**: All numbers should be converted to network byte order before they are sent to either the RoadRunner agent or the C2 node.

**NOTE**: The "{ }" around each section are only visual dilimeters and are not actually appart of the byte stream.

### Communication Flow

The RoadRunner agent is designed to call back to its C2 infrastructure as opposed to waiting for a connection from a potential C2 node. As such an agent will initiate all connections based on the the defined HOST and PORT values specified at compile time. Below is the basic communication flow between an agent and its C2 node.

    RR AGENT    ----Checkin Response---->  C2 NODE
    RR AGENT    <---- (Some Command) ----  C2 NODE
    RR AGENT    ----Command Response---->  C2 NODE
                          ....
    RR AGENT    <----Shutdown Command----  C2 NODE
    RR AGENT    ----Shutdown Response--->  C2 NODE
    RR AGENT    X    Connection Close   X  C2 NODE

### Generic Command Stream

    {uint32:total_message_size}{uint32:command_length}{char[command_length]:command}{uint32:args_length}{char[args_length]:args}
  
### Generic Response Stream

    {uint32:total_message_size}{uint32:ret_code}{uint32:message_length}{char[msg_length]:message}

### RoadRunner Checkin Response

**Description**: Upon connection to a potential C2 node this is the first thing a RoadRunner agent will send.

    {uint32:total_message_size}{uint32:ret_code}{uint32:19}{char[19]:"roadrunner checkin"}

### Shutdown Command

**Description**: Tell the RoadRunner agent to shutdown.

    {uint32:total_message_size}{uint32:9}{char[9]:"shutdown"}{uint32:1}{char[1]:""}

### Shutdown Response

**Description**: Final message the agent will send after receiving the shutdown command.

    {uint32:total_message_size}{uint32:ret_code}{uint32:14}{char[14]:"shutting down"}

### Sleep Command

**Description**: Tell the RoadRunner agent to sleep for a specified number of seconds.

    {uint32:total_message_size}{uint32:6}{char[6]:"sleep"}{uint32:args_length}{char[args_length]:args}

### Sleep Response

**Description**: After executing the sleep the agent will return back a statement stating the number of seconds it slept, or it will return an error message of failure.

    {uint32:total_message_size}{uint32:ret_code}{uint32:msg_length}{char[msg_length]:success_or_error_message}

### Upload Command

**Description**: This command tells a RoadRunner agent to write the contents of a file to disk at the specified location.

    {uint32:total_message_size}{uint32:7}{char[7]:"upload"}{uint32 args_length}{{uint32:file_path_size}{char[file_path_size]:file_path}{uint32:contents_size}{char[contents_size]:contents}}

### Upload Response

**Description**: After executing an upload command the RoadRunner agent will send back a response message indicating success or failure of the upload.

    {uint32:total_message_size}{uint32:ret_code}{uint32:msg_length}{char[msg_length]:success_or_error_message}

### Download Command

**Description**: This command tells a RoadRunner agent to read the contents of a file and send it back to the C2 so it can be downloaded.

    {uint32:total_message_size}{uint32:9}{char[9]:"download"}{uint32 args_length}{{uint32:file_path_size}{char[file_path_size]:file_path}}

### Download Response

**Description**: After executing a download command the RoadRunner agent will either send back the contents of the specified on success or will send back an error message on failure.

    {uint32:total_message_size}{uint32:ret_code}{uint32:message_length}{char[msg_length]:file_contents}

### Hostname Command Stream

**Description**: This command tells a RoadRunner agent to get the hostname.

    {uint32:total_message_size}{uint32:9}{char[9]:"hostname"}{uint32:1}{char[1]:""}
  
### Hostname Response Stream

**Description**: The agent returns the hostname.

    {uint32:total_message_size}{uint32:ret_code}{uint32:message_length}{char[msg_length]:message}

### Netstat Command Stream

**Description**: This command tells a RoadRunner agent to gather any network connections on the host.

    {uint32:total_message_size}{uint32:8}{char[8]:"netstat"}{uint32:1}{char[1]:""}
  
### Netstat Response Stream

**Description**: The agent will return a list of network connections.

    {uint32:total_message_size}{uint32:ret_code}{uint32:message_length}{char[msg_length]:message}

### ProcessList Command Stream

**Description**: This command tells a RoadRunner agent to gather a process list.

    {uint32:total_message_size}{uint32:9}{char[9]:"proclist"}{uint32:args_length}{char[args_length]:pid OR ""}
  
### ProcessList Response Stream

**Description**: The agent returns the process list in the message.

    {uint32:total_message_size}{uint32:ret_code}{uint32:message_length}{char[msg_length]:message}

### Invoke Command Stream

**Description**: This command tells a RoadRunner agent to invoke a shell command.

    {uint32:total_message_size}{uint32:7}{char[7]:"invoke"}{uint32:args_length}{char[args_length]:shell_command}
  
### Invoke Response Stream

**Description**: After executing the shell command the RoadRunner agent will send back a message with the result of the command

    {uint32:total_message_size}{uint32:ret_code}{uint32:message_length}{char[msg_length]:message}

### Proxy Command

**Description**: This command tells a RoadRunner agent to create a proxy process to pass network traffic from a local port to a target.

    {uint32:total_message_size}{uint32:6}{char[6]:"proxy"}{uint32 args_length}{{uint32:target_host_len}{char[target_host_len]:target_host}{uint32:target_port_len}{char[target_port_len]:target_port}{uint16:local_port}}

### Proxy Response

**Description**: After executing the proxy command the RoadRunner agent will send back a message indicating it started the proxy process.

    {uint32:total_message_size}{uint32:ret_code}{uint32:message_length}{char[msg_length]:success_or_error_message}
