# C

## Course Description

This course covers the fundamental concepts of C programming and reaches into advanced concepts related to CNO. The bulk of the course revolves around learning C98/C99 and a practical portion that invloves the implementation of an agent program.

## Prerequisites 

Basic computer science principles and previous programming experience.

## Objectives

- Understanding of the C language
- Understanding of module programming
- Understanding of serialization
- Basic understanding of compiling 
- Basic debugging 
- Networking fundamentals

## Training Hours

Class will be scheduled from 0800-1630 everyday. There will be a 30 min lunch break scheduled from 1200-1230. Expect to be in class for the full duration everday. If you have conflicts or need to leave early please notify the class instructor and chain of command as soon as possible.

## Course Calendar

### Monday

- Course Introduction (0800-0830)
- Lecture (0830-1200)
    1. Comments
    2. Headers
    3. Preprocessor Commands
    4. Data Types
    5. Arrays
    6. Conditionals
    7. Strings
- Lunch (1200-1230)
- Agent Introduction (1230-1330)
    1. Project Structure
    2. CMake/Make
    3. Building
    4. Testing
- Exercises/Agent Development (1330-1630)
    1. Exercies[Day1]
    2. Agent Development (not in any particular order)
        - RoadRunner.c
            1. perform_command
        - helloworld.c
            1. say_hello
        - core.c
            1. checkin_command
                Tests: should_provide_checkin_response
            2. sleep_command
                Tests: should_execute_good_sleep_cmd
                       should_error_on_bad_sleep_args
        - sys.c
            1. hostname_command
                Tests: should_provide_hostname_response
                       should_error_failed_hostname


### Tuesday

- Lecture (0800-1200)
    1. For/While/DoWhile Loops
    2. Functions
    3. Static
    4. Pointers
    5. Passing Arguments by Reference
    6. Casting Between Data Types
    7. Dynamic Allocations
    8. Input, Output, and Files
- Lunch (1200-1230)
- Exercises/Agent Development (1230-1630)
    1. Exercise[Day2]
    2. Agent Development (not in any particular order)
        - Files.c
            1. download_file_command
                Tests: should_provide_download_response
                       should_error_on_bad_download_file_name
            2. read_file
                Tests: should_read_good_file
                       should_error_read_on_bad_file
            3. write_file
                Tests: should_write_file
                       should_error_on_write_null

### Wednesday

- Lecture (0800-1200)
    1. Structures
    2. Typedef
    3. Linked Lists
    4. Unions
    5. Function Pointers
    6. Bitmasks
- Lunch (1200-1230)
- Exercises/Agent Development (1230-1630)
    1. Exercises[Day3]
    2. Agent Development (not in any particular order)
        - Sys.c
            1. invoke_command
                Tests: should_provide_valid_cmd_return
                       should_touch_file_with_invoke
            2. parse_conn_line
            3. print_connections
            4. hex_to_ipaddr
            5. free_connections
            6. get_netstat_command
                Tests: should_error_fail_netstat
                       should_provide_valid_netstat
            

### Thursday

- Lecture (0800-1200)
    1. Serializing
    2. Networking
    3. Protocols
- Lunch (1200-1230)
- Exercises/Agent Development (1230-1630)
    1. Exercises[Day4]
    2. Agent Development (not in any particular order)
        - Roadrunner.c
            1. connect_to_server
            2. send_response
            3. receive_command
        - Commands.c
            1. serialize_response
            2. deserialize_command
        - Files.c
            1. deserialize_file_command
            2. deserialize_upload_file_contents
            3. upload_file_command
                Tests:  should_provide_upload_response
                        should_error_on_bad_upload_file_name_length
                        should_error_on_bad_upload_content_length

### Friday

- Exercises/Agent Development (0800-1200)
- Lunch (1200-1230)
- Exercises/Agent Development (1230-1600)
- Course Wrap-Up (1600-1630)

## Recommended Reading

1. http://beej.us/guide/bgnet/ - [Beej’s Guide to Network Programming] (free)
2. https://www.amazon.com/dp/0131103628 - [C Programming Language, 2nd Edition]
3. https://www.amazon.com/dp/0131411551 - [Unix Network Programming: The Sockets Networking API]

## Recommended Videos
1. https://www.youtube.com/watch?v=8PopR3x-VMY - [C Programming for Beginners]
2. https://www.youtube.com/playlist?list=PL9IEJIKnBJjH_zM5LnovnoaKlXML5qh17 - [Network Programming]
3. https://www.youtube.com/watch?v=uS37TujnLRw - [Serialization - A Crash Course]
