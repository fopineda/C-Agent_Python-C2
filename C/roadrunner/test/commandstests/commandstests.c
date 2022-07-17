/**
 * commandstests.c contains all tests for the commands module.
 * @file commandstests.c
 */

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

//include the test's .h file
#include <commandstests/commandstests.h>
// include the tests functions
#include <tests.h>
// include the header with the functions we want to test
#include <commands.h>
#include <utils.h>

/**
 * @brief Helper function for building command streams for testing.
 * @param cmd the command string (ensure it is null terminated)
 * @param cmd_len the command length you wish to send
 * @param args the args strings (ensure it is null terminated)
 * @param args_len the args length you wish to send
 * @return the resulting command stream
 */
static uint32_t build_command_stream(char *cmd, uint32_t cmd_len, char *args, uint32_t args_len, char **stream_out)
{
    uint32_t mem_offset = 0;
    uint32_t stream_size = sizeof(uint32_t) * 3 + cmd_len + args_len;
    uint32_t net_stream_size = htonl(stream_size);
    uint32_t net_cmd_size = htonl(cmd_len);
    uint32_t net_args_size = htonl(args_len);
    *stream_out = calloc(stream_size, sizeof(char));
    memcpy(*stream_out + mem_offset, &net_stream_size, sizeof(uint32_t));
    mem_offset += sizeof(uint32_t);
    memcpy(*stream_out + mem_offset, &net_cmd_size, sizeof(uint32_t));
    mem_offset += sizeof(uint32_t);
    memcpy(*stream_out + mem_offset, cmd, cmd_len);
    mem_offset += cmd_len;
    memcpy(*stream_out + mem_offset, &net_args_size, sizeof(uint32_t));
    mem_offset += sizeof(uint32_t);
    memcpy(*stream_out + mem_offset, args, args_len);
    return stream_size;
}

/**
 * @brief Test if the commands module can deserialize a good command stream.
 * @return bool representing success or failure
 */
bool should_deserialize_good_stream()
{
    char *test_name = "should_deserialize_good_stream";
    // start the test
    TEST_START(test_name);
    // initialize required variables for testing
    Command *cmd_struct;
    char *cmd = "test";
    uint32_t cmd_len = strlen(cmd);
    char *args = "args";
    uint32_t args_len = strlen(args);
    char *msg_stream;
    uint32_t msg_size = build_command_stream(cmd, cmd_len, args, args_len, &msg_stream);

    // now we test the function
    cmd_struct = deserialize_command(msg_size, msg_stream);
    ASSERT_NOT_NULL(cmd_struct, "cmd_struct was returned as NULL");
    ASSERT_STRING_EQUALS(cmd, cmd_struct->cmd, cmd_len, "commands are not the same");
    ASSERT_STRING_EQUALS(args, cmd_struct->args, args_len, "args are not the same");
    free_command(cmd_struct);
    cmd_struct = NULL;
    checkfree(msg_stream);
    TEST_PASSED(test_name);
}

/**
 * @brief Test if the commands module can deserialize a good command stream that contains null bytes in its args.
 * @return bool representing success or failure
 */
bool should_deserialize_null_byte_cmd_args()
{
    char *test_name = "should_deserialize_null_byte_cmd_args";
    // start the test
    TEST_START(test_name);
    // initialize required variables for testing
    Command *cmd_struct;
    char *cmd = "test";
    uint32_t cmd_len = strlen(cmd);
    char *args = "\x01\x00\x02\x00\x03\x00";
    uint32_t args_len = 6;
    char *msg_stream;
    uint32_t msg_size = build_command_stream(cmd, cmd_len, args, args_len, &msg_stream);
    // now we test the function
    cmd_struct = deserialize_command(msg_size, msg_stream);
    ASSERT_NOT_NULL(cmd_struct, "cmd_struct was returned as NULL");
    ASSERT_STRING_EQUALS(cmd, cmd_struct->cmd, cmd_len, "commands are not the same");
    ASSERT_BYTES_EQUALS(args, cmd_struct->args, args_len,"expected bytes to be the same");
    ASSERT_STRING_EQUALS(args, cmd_struct->args, args_len, "args are not the same");
    free_command(cmd_struct);
    cmd_struct = NULL;
    checkfree(msg_stream);
    TEST_PASSED(test_name);
}

/**
 * @brief Test if the commands module gracefully handles bad command streams.
 * @return bool representing success or failure
 */
bool should_fail_bad_streams()
{
    char *test_name = "should_fail_bad_streams";
    // start the test
    TEST_START(test_name);
    // initialize required variables for testing
    Command *cmd_struct;
    char *msg_stream;
    uint32_t msg_size;
    // stream has invalid command length
    msg_size = 8 + sizeof(uint32_t) * 3;
    build_command_stream("test", 10, "args", 4, &msg_stream);
    // now we test the function
    cmd_struct = deserialize_command(msg_size, msg_stream);
    ASSERT_NULL(cmd_struct, "cmd_struct was returned was not NULL");
    free_command(cmd_struct);
    checkfree(msg_stream);
    cmd_struct = NULL;
    msg_stream = NULL;

    // stream has invalid args length
    msg_size = 8 + sizeof(uint32_t) * 3;
    free(msg_stream);
    build_command_stream("test", 4, "args", 10, &msg_stream);
    // now we test the function
    cmd_struct = deserialize_command(msg_size, msg_stream);
    ASSERT_NULL(cmd_struct, "cmd_struct was returned was not NULL");
    free_command(cmd_struct);
    checkfree(msg_stream);
    cmd_struct = NULL;
    msg_stream = NULL;

    // stream is not complete or to small
    msg_size = sizeof(uint32_t);
    msg_stream = calloc(sizeof(uint32_t), sizeof(char));
    // now we test the function
    cmd_struct = deserialize_command(msg_size, msg_stream);
    ASSERT_NULL(cmd_struct, "cmd_struct was returned was not NULL");
    free_command(cmd_struct);
    checkfree(msg_stream);
    cmd_struct = NULL;
    msg_stream = NULL;

    TEST_PASSED(test_name);
}

/**
 * @brief Test if commands module can serialize a good Response struct into a byte stream.
 * @return bool representing success or failure
 */
bool should_serialize_response()
{
    char *test_name = "should_serialize_response";
    TEST_START(test_name);
    Response *rsp;
    char* msg = "test response";
    uint32_t msg_len = strlen(msg);
    int32_t ret_code = 1;
    char *msg_stream, *s_msg;
    uint32_t stream_len, s_total_size, s_msg_len;
    int s_ret_code;

    // create a new response struct
    rsp = alloc_response(ret_code, msg, msg_len);
    msg_stream = NULL;
    stream_len = serialize_response(rsp, &msg_stream);
    ASSERT_NOT_EQUALS(0, stream_len, "expected stream length to not be 0")

    // check that we actually get a stream back
    ASSERT_NOT_NULL(msg_stream, "msg_stream was returned as NULL");
    // check each component of the stream

    memcpy(&s_total_size, msg_stream, sizeof(uint32_t));
    // convert back to host byte order
    s_total_size = ntohl(s_total_size);
    ASSERT_EQUALS(stream_len, s_total_size, "expected the stream sizes to be the same");
    memcpy(&s_ret_code, msg_stream + sizeof(uint32_t), sizeof(uint32_t));
    // convert back to host byte order
    s_ret_code = ntohl(s_ret_code);
    ASSERT_EQUALS(ret_code, s_ret_code, "expected the return codes to be the same");
    memcpy(&s_msg_len, msg_stream + sizeof(uint32_t) * 2, sizeof(uint32_t));
    // convert back to host byte order
    s_msg_len = ntohl(s_msg_len);
    ASSERT_EQUALS(msg_len, s_msg_len, "expected the message lengths to be the same");
    // allocate memory for the stream message, should be size plus 1 for null terminator
    s_msg = calloc(s_msg_len + 1, sizeof(char));
    memcpy(s_msg, msg_stream + sizeof(uint32_t) * 3, s_msg_len);
    ASSERT_STRING_EQUALS(msg, s_msg, msg_len, "expected the messages to be the same");
    free_response(rsp);
    checkfree(msg_stream);
    checkfree(s_msg);
    TEST_PASSED(test_name);
}

/**
 * @brief Test if commands module can serialize a good Response struct that contains null bytes into a byte stream.
 * @return bool representing success or failure
 */
bool should_serialize_null_byte_response_message()
{
    char *test_name = "should_serialize_null_byte_response_message";
    TEST_START(test_name);
    Response *rsp;
    char* msg = "\x01\x00\x02\x00\x03\x00";
    uint32_t msg_len = 6;
    int ret_code = 1;
    char *msg_stream, *s_msg;
    uint32_t stream_len, s_msg_len, s_total_size;
    int s_ret_code;
    // create a new response struct
    rsp = alloc_response(ret_code, msg, msg_len);
    msg_stream = NULL;
    stream_len = serialize_response(rsp, &msg_stream);
    ASSERT_NOT_EQUALS(0, stream_len, "expected stream length to not be 0")
    // check that we actually get a stream back
    ASSERT_NOT_NULL(msg_stream, "msg_stream was returned as NULL");
    // check each component of the stream


    memcpy(&s_total_size, msg_stream, sizeof(uint32_t));
    // convert back to host byte order
    s_total_size = ntohl(s_total_size);
    ASSERT_EQUALS(stream_len, s_total_size, "expected the stream sizes to be the same");
    memcpy(&s_ret_code, msg_stream + sizeof(uint32_t), sizeof(uint32_t));
    // convert back to host byte order
    s_ret_code = ntohl(s_ret_code);
    ASSERT_EQUALS(ret_code, s_ret_code, "expected the return codes to be the same");
    memcpy(&s_msg_len, msg_stream + + sizeof(uint32_t) * 2, sizeof(uint32_t));
    // convert back to host byte order
    s_msg_len = ntohl(s_msg_len);
    ASSERT_EQUALS(msg_len, s_msg_len, "expected the message lengths to be the same");
    // allocate memory for the stream message, should be size plus 1 for null terminator
    s_msg = calloc(s_msg_len + 1, sizeof(char));
    memcpy(s_msg, msg_stream + + sizeof(uint32_t) * 3, s_msg_len);
    ASSERT_BYTES_EQUALS(msg, s_msg, msg_len, "expected the messages to be the same");
    free_response(rsp);
    checkfree(msg_stream);
    checkfree(s_msg);
    TEST_PASSED(test_name);
}
