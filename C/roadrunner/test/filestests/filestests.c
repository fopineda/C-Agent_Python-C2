/**
 * filestests.c contains all of the tests for the files module.
 * @file filestests.c
 */

#include <tests.h>
#include <filestests/filestests.h>
#include <files.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <utils.h>

static uint32_t generate_upload_args(char *file_name, uint32_t file_name_length, char *content, uint32_t content_length, 
                                    char **buffer_out);
/**
 * @brief Test if the files module can read a valid file.
 * @return bool representing success or failure
 */
bool should_read_good_file()
{
    char *test_name = "should_read_good_file";
    TEST_START(test_name)
    char *file_name = "./test/data/test_file.txt";
    char *expected_contents = "I AM A TEST FILE";
    char *received_contents = NULL;
    uint32_t expected_size = 16;
    uint32_t received_size;
    received_size = read_file(file_name, &received_contents);
    ASSERT_EQUALS(expected_size, received_size, "expected the sizes to be the same");
    ASSERT_NOT_NULL(received_contents, "expected the contents to be NULL");
    ASSERT_STRING_EQUALS(expected_contents, received_contents, expected_size, "expected the contents to be the same");
    free(received_contents);
    TEST_PASSED(test_name)
}

/**
 * @brief Test if the files module can gracefully handle reading a bad file.
 * @return bool representing success or failure
 */
bool should_error_read_on_bad_file()
{
    char *test_name = "should_error_read_on_bad_file";
    TEST_START(test_name)
    char *file_name = "BAD FILE";
    char *received_contents = NULL;
    uint32_t expected_size = 0;
    uint32_t received_size;
    received_size = read_file(file_name, &received_contents);
    ASSERT_EQUALS(expected_size, received_size, "expected the sizes to be the same");
    ASSERT_NULL(received_contents, "expected the contents to be NULL");
    if (received_contents != NULL)
    {
        free(received_contents);
    }
    TEST_PASSED(test_name)
}

/**
 * @brief Test if the files module can write a file.
 * @return bool representing success or failure
 */
bool should_write_file()
{
    char *test_name = "should_write_file";
    TEST_START(test_name)
    char *file_name = "./test/data/test_write_file.txt";
    char *write_contents = "I AM A TEST FILE";
    uint32_t expected_size = 16;
    uint32_t received_size;
    received_size = write_file(file_name, write_contents, expected_size);
    ASSERT_EQUALS(expected_size, received_size, "expected the sizes to be the same");
    ASSERT_EQUALS(0, remove(file_name), "expected file remove to return 0");
    TEST_PASSED(test_name)
}

/**
 * @brief Test if the files module can gracefully handle receiving NULL for contents.
 * @return bool representing success or failure
 */
bool should_error_on_write_null()
{
    char *test_name = "should_error_on_write_null";
    TEST_START(test_name)
    char *file_name = "./test/data/test_write_file.txt";
    uint32_t expected_size = 0;
    uint32_t received_size;
    received_size = write_file(file_name, NULL, expected_size);
    ASSERT_EQUALS(expected_size, received_size, "expected the sizes to be the same");
    ASSERT_NOT_EQUALS(0, remove(file_name), "expected file remove to not return 0");
    TEST_PASSED(test_name)
}

/**
 * @brief Test if the files module can handle a valid download command.
 * @return bool representing success or failure
 */
bool should_provide_download_response()
{
    char *test_name = "should_provide_download_response";
    TEST_START(test_name)
    Command *cmd = NULL;
    Response *rsp = NULL;
    int good_ret_code = 0;
    char *command = "download";
    uint32_t  command_len, args_len;
    char *download_msg = "I AM A TEST FILE";
    char *args = "./test/data/test_file.txt";
    command_len = strlen(command);
    args_len = strlen(args);
    cmd = alloc_command(command, command_len, args, args_len);
    rsp = download_file_command(cmd);
    ASSERT_NOT_NULL(rsp, "expected response to not be NULL");
    ASSERT_EQUALS(good_ret_code, rsp->ret_code, "expected a different return code");
    ASSERT_STRING_EQUALS(download_msg, rsp->msg, strlen(download_msg), "expected the contents of the test file")
    free_command(cmd);
    cmd = NULL;
    free_response(rsp);
    rsp = NULL;
    TEST_PASSED(test_name)
}

/**
 * @brief Test if the files module can gracefully handle a download command with a bad file name or path.
 * @return bool representing success or failure
 */
bool should_error_on_bad_download_file_name()
{
    char *test_name = "should_error_on_bad_download_file_name";
    TEST_START(test_name)
    Command *cmd = NULL;
    Response *rsp = NULL;
    int error_ret_code = 1;
    uint32_t command_len, args_len;
    char *command = "download";
    char *error_msg = "error downloading file";
    char *args = "./test/data/not_a_test_file.txt";
    command_len = strlen(command);
    args_len = strlen(args);
    cmd = alloc_command(command, command_len, args, args_len);
    rsp = download_file_command(cmd);
    ASSERT_NOT_NULL(rsp, "expected response to not be NULL");
    ASSERT_EQUALS(error_ret_code, rsp->ret_code, "expected a different return code");
    ASSERT_STRING_EQUALS(error_msg, rsp->msg, strlen(error_msg), "expected a download error message")
    free_command(cmd);
    cmd = NULL;
    free_response(rsp);
    rsp = NULL;
    TEST_PASSED(test_name)
}

/**
 * @brief A helper function to generate the upload arguments.
 * @param file_name the name or path of the file
 * @param file_name_length the length of file_name
 * @param content the content of the file
 * @param content_length the length of content
 * @param buffer_out a pointer to pass out the arguments buffer
 * @return the size of buffer_out
 */
static uint32_t generate_upload_args(char *file_name, uint32_t file_name_length, char *content, uint32_t content_length, 
                                    char **buffer_out)
{
    uint32_t net_file_name_length, net_content_length, mem_offset, args_size;
    net_file_name_length = htonl(file_name_length);
    net_content_length = htonl(content_length);
    args_size = sizeof(uint32_t) * 2 + file_name_length + content_length;
    *buffer_out = calloc(args_size, sizeof(char));
    memcpy(*buffer_out, &net_file_name_length, sizeof(uint32_t));
    mem_offset = sizeof(uint32_t);
    memcpy(*buffer_out + mem_offset, file_name, file_name_length);
    mem_offset += file_name_length;
    memcpy(*buffer_out + mem_offset, &net_content_length, sizeof(uint32_t));
    mem_offset += sizeof(uint32_t);
    memcpy(*buffer_out + mem_offset, content, content_length);
    return args_size;
}

/**
 * @brief Test if the files module can handle a valid upload command and provide a good response.
 * @return bool representing success or failure
 */
bool should_provide_upload_response()
{
    char *test_name = "should_provide_upload_response";
    TEST_START(test_name)
    Command *cmd = NULL;
    Response *rsp = NULL;
    int good_ret_code = 0;
    uint32_t command_len, args_len, path_size, content_size;
    char *command = "upload";
    char *args = NULL;
    char *upload_msg = "upload successful";
    char *file_path = "./test/data/upload_test_file.txt";
    char *file_contents = "UPLOAD FILE CONTENTS";
    path_size = strlen(file_path);
    content_size = strlen(file_contents);
    args_len = generate_upload_args(file_path, path_size, file_contents, content_size, &args);
    command_len = strlen(command);
    cmd = alloc_command(command, command_len, args, args_len);
    rsp = upload_file_command(cmd);
    ASSERT_NOT_NULL(rsp, "expected response to not be NULL");
    ASSERT_EQUALS(good_ret_code, rsp->ret_code, "expected a different return code");
    ASSERT_STRING_EQUALS(upload_msg, rsp->msg, strlen(upload_msg), "expected a download error message")
    ASSERT_EQUALS(0, remove(file_path), "expected remove to return success");
    checkfree(args);
    free_command(cmd);
    cmd = NULL;
    free_response(rsp);
    rsp = NULL;
    TEST_PASSED(test_name)
}

/**
 * @brief Test if the files module can gracefully handle bad file_name lengths being passed.
 * @return bool representing success or failure
 */
bool should_error_on_bad_upload_file_name_length()
{
    char *test_name = "should_error_on_bad_upload_file_name_length";
    TEST_START(test_name)
    Command *cmd = NULL;
    Response *rsp = NULL;
    int error_ret_code = 1;
    uint32_t command_len, args_len, path_size, content_size;
    char *command = "upload";
    char *error_msg = "error with upload file path";
    char *file_path = "";
    char *args = NULL;
    path_size = 0;
    char *file_contents = "UPLOAD FILE CONTENTS";
    content_size = strlen(file_contents);
    generate_upload_args(file_path, path_size, file_contents, content_size, &args);
    command_len = strlen(command);
    args_len = path_size + content_size + sizeof(uint32_t) * 2;
    cmd = alloc_command(command, command_len, args, args_len);
    rsp = upload_file_command(cmd);
    ASSERT_NOT_NULL(rsp, "expected response to not be NULL");
    ASSERT_EQUALS(error_ret_code, rsp->ret_code, "expected a different return code");
    ASSERT_STRING_EQUALS(error_msg, rsp->msg, strlen(error_msg), "expected a download error message")
    ASSERT_EQUALS(-1, remove(file_path), "expected remove to return error");
    checkfree(args);
    free_command(cmd);
    cmd = NULL;
    free_response(rsp);
    rsp = NULL;
    TEST_PASSED(test_name)
}

/**
 * @brief Test if the files module can gracefully handle bad content lengths being passed.
 * @return bool representing success or failure
 */
bool should_error_on_bad_upload_content_length()
{
    char *test_name = "should_error_on_bad_upload_content_length";
    TEST_START(test_name)
    Command *cmd = NULL;
    Response *rsp = NULL;
    int error_ret_code = 1;
    uint32_t command_len, args_len, path_size, content_size;
    char *command = "upload";
    char *error_msg = "error with upload file contents";
    char *file_path = "./test/data/upload_test_file.txt";
    char *args = NULL;
    path_size = strlen(file_path);
    char *file_contents = "";
    content_size = 0;
    generate_upload_args(file_path, path_size, file_contents, content_size, &args);
    command_len = strlen(command);
    args_len = path_size + content_size + sizeof(uint32_t) * 2;
    cmd = alloc_command(command, command_len, args, args_len);
    rsp = upload_file_command(cmd);
    ASSERT_NOT_NULL(rsp, "expected response to not be NULL");
    ASSERT_EQUALS(error_ret_code, rsp->ret_code, "expected a different return code");
    ASSERT_STRING_EQUALS(error_msg, rsp->msg, strlen(error_msg), "expected a upload error message")
    ASSERT_EQUALS(-1, remove(file_path), "expected remove to return error");
    checkfree(args);
    free_command(cmd);
    cmd = NULL;
    free_response(rsp);
    rsp = NULL;
    TEST_PASSED(test_name)
}
