#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include "ErrorCodes.h"
#include <stdint.h>
#include <expected>
namespace Chat
{
    enum class ProgramType
    {
        CLIENT,
        SERVER
    };

    typedef struct
    {
        uint32_t address;
        uint16_t port;
    } ClientInfo;

    namespace ArgumentParser
    {
        std::expected<ProgramType, ErrorCode> ParseProgramType(int argc, char **argv);
        std::expected<ClientInfo, ErrorCode> ParseAddressPort(int argc, char **argv);
        std::expected<uint16_t, ErrorCode> ParsePort(int argc, char **argv);
    } // namespace ArgumentParser

} // namespace Chat

#endif