#include "ArgumentParser.h"
#include "StatusCodes.h"
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>

#define CLIENT_PROGRAM_TYPE "client"
#define SERVER_PROGRAM_TYPE "server"
static constexpr size_t PROGRAM_TYPE_ARGUMENT_INDEX = 1;
static constexpr size_t ADDRESS_ARGUMENT_INDEX = 2;
static constexpr size_t PORT_ARGUMENT_INDEX_CLIENT = 3;
static constexpr size_t PORT_ARGUMENT_INDEX_SERVER = 2;

static constexpr size_t STRCMP_EQUAL_VALUE = 0;
static constexpr size_t INET_SUCCESS_VALUE = 1;
static constexpr size_t ERRNO_SUCCESS_VALUE = 0;

std::expected<Chat::ProgramType, Chat::ErrorCode> Chat::ArgumentParser::ParseProgramType(int argc, char **argv)
{
    if (argc < 2) // Need atleast one more for program type
    {
        return std::unexpected(Chat::ErrorCode::ARGUMENT_PARSE_FAILURE);
    }

    if (strncmp(CLIENT_PROGRAM_TYPE, argv[PROGRAM_TYPE_ARGUMENT_INDEX], sizeof(CLIENT_PROGRAM_TYPE)) == STRCMP_EQUAL_VALUE)
    {
        return Chat::ProgramType::CLIENT;
    }
    if (strncmp(SERVER_PROGRAM_TYPE, argv[PROGRAM_TYPE_ARGUMENT_INDEX], sizeof(SERVER_PROGRAM_TYPE)) == STRCMP_EQUAL_VALUE)
    {
        return Chat::ProgramType::SERVER;
    }
    return std::unexpected(Chat::ErrorCode::ARGUMENT_PARSE_FAILURE);
}

std::expected<Chat::ClientInfo, Chat::ErrorCode> Chat::ArgumentParser::ParseAddressPort(int argc, char **argv)
{
    Chat::ClientInfo info = {0, 0};
    if (argc != 2 + 1 + 1) // Type + addr + port
    {
        return std::unexpected(Chat::ErrorCode::ARGUMENT_PARSE_FAILURE);
    }

    struct in_addr ip_addr;
    int result = inet_pton(AF_INET, argv[ADDRESS_ARGUMENT_INDEX], &ip_addr);

    if (result != INET_SUCCESS_VALUE)
    {
        return std::unexpected(Chat::ErrorCode::ARGUMENT_PARSE_FAILURE);
    }
    info.address = ip_addr.s_addr;

    errno = ERRNO_SUCCESS_VALUE;
    uint16_t port = static_cast<uint16_t>(strtoul(argv[PORT_ARGUMENT_INDEX_CLIENT], NULL, 0));
    if (errno != ERRNO_SUCCESS_VALUE)
    {
        return std::unexpected(Chat::ErrorCode::ARGUMENT_PARSE_FAILURE);
    }

    info.port = port;
    return info;
}

std::expected<uint16_t, Chat::ErrorCode> Chat::ArgumentParser::ParsePort(int argc, char **argv)
{
    if (argc != 2 + 1) // Type +  port
    {
        return std::unexpected(Chat::ErrorCode::ARGUMENT_PARSE_FAILURE);
    }

    errno = ERRNO_SUCCESS_VALUE;
    uint16_t port = static_cast<uint16_t>(strtoul(argv[PORT_ARGUMENT_INDEX_SERVER], NULL, 0));
    if (errno != ERRNO_SUCCESS_VALUE)
    {
        return std::unexpected(Chat::ErrorCode::ARGUMENT_PARSE_FAILURE);
    }

    return port;
}