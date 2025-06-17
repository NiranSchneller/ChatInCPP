#ifndef STATUS_CODES_H
#define STATUS_CODES_H

namespace Chat
{
    /**
     * @brief Represents the various Error Codes that the programs functions can return
     */
    enum class ErrorCode
    {
        UNINITIALIZED,
        SOCKET_DESTROY_ERROR,
        ALREADY_INITIALIZED,
        CLIENT_CONNECTION_INITIALIZATION_ERROR,
        RECEIVE_ERROR,
        SEND_ERROR,
        INPUT_FROM_USER_ERROR,
        SOCKET_INITIALIZATION_ERROR,
        ARGUMENT_PARSE_FAILURE,
        CLIENT_NOT_CONNECTED,
        MESSAGE_RECEIVED_TOO_BIG,
        INVALID_ARGUMENT
    };
} // namespace Chat

#endif
