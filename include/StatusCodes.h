#ifndef STATUS_CODES_H
#define STATUS_CODES_H

namespace Chat
{
    enum class StatusCode
    {
        SUCCESS,
        UNINITIALIZED,
        SOCKET_DESTROY_ERROR,
        ALREADY_INITIALIZED,
        CLIENT_CONNECTION_INITIALIZATION_ERROR,
        CLIENT_RECEIVE_ERROR,
        CLIENT_SEND_ERROR,
        SOCKET_INITIALIZATION_ERROR
    };
} // namespace Chat

#endif
