#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include <stdint.h>
#include <expected>

#include "ErrorCodes.h"

namespace Chat
{
    /**
     * @brief Enum to represent the type of program being run (server or client)
     */
    enum class ProgramType
    {
        CLIENT,
        SERVER
    };

    /**
     * @brief Information that the client needs to connect to a server
     */
    typedef struct
    {
        uint32_t ip;
        uint16_t port;
    } Address;

    namespace ArgumentParser
    {
        std::expected<ProgramType, ErrorCode> ParseProgramType(int argc, char **argv);
        std::expected<Address, ErrorCode> ParseAddressPort(int argc, char **argv);
        std::expected<uint16_t, ErrorCode> ParsePort(int argc, char **argv);
    } // namespace ArgumentParser

} // namespace Chat

#endif