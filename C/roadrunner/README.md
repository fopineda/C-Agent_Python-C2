# RoadRunner Agent

## Brief

We need to develop an agent that can be controlled remotely over a network. The agent will need to have the following basic functionality:

1. Connect back to a C2 server at a specified IP and port
2. Deserialize Command structures
3. Serialize Response structures
4. Send and receive bytes streams from the C2 server based on the RR protocol
5. Send a properly formatted "Check In" response
6. Sleep for a specified amount of time
7. Get the hostname
8. Get a process list
9. Check for network connections (netstat)
10. Download a file from the host the agent is on
11. Upload a file to the host the agent is on
12. Shutdown

Additionally, the agent will need to handle situations where it may encounter errors, such as if it were to receive bad or malformed data. It is therefore imperative that defensive programming is used throughout all of the agent's code.

In order to speed up development, a project framework has been provided along with unit tests and a few helper scripts.

### Scripts

The project contains three helper scripts:

1. build - builds the RoadRunner agent and place it in the bin directory.
2. test - builds and tests the RoadRunner agent against the unit tests and integration tests.
3. clean - removes all current build artifacts.

These scripts can be executed from the projects root directory (roadrunner).

### UnitTests

This project comes with unit tests pre-written and located in the test directory. These tests are not comprehensive, but will serve as a guide for development. Run these tests early and often. Once all tests have passed, the agent's core functionality should be working.

## Protocol

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

    {uint32:total_message_size}{uint32:9}{char[9]:"download"}{uint32 args_length}{char[file_path_size]:file_path}

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