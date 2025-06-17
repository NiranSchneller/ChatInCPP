#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <stdint.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <expected>

#include "ErrorCodes.h"

namespace Chat
{
    /**
     * @brief Represents a server which can sequentially accept incoming client connections
     */
    using fd_t = int;
    template <size_t MAX_MESSAGE_SIZE>
    class Server
    {
    public:
        Server();

        ~Server();

        /**
         * @brief Initializes a socket based on params given
         * @param port The port the socket will bind to
         * @param listeningQueueAmount The amount of clients allowed to wait to connect to the server
         *
         * @returns Status Code
         */
        std::expected<void, ErrorCode> Initialize(uint16_t port, size_t listenQueueAmount);

        /**
         * @brief Accepts an incoming client
         *
         * @returns Error code
         */
        std::expected<void, ErrorCode> AcceptClient();

        /**
         * @brief Handles a client and echoes messages until connection is closed
         *
         * @return Status code
         */
        std::expected<void, ErrorCode> HandleClient();

    private:
        bool m_init = false;
        bool m_clientConnected = false;
        fd_t m_socket = 0;
        fd_t m_clientSocket = 0;
        uint8_t m_buffer[MAX_MESSAGE_SIZE];
    };
} // namespace Chat

static constexpr int SOCKET_ERROR_RETURN_VALUE = -1;

template <size_t MAX_MESSAGE_SIZE>
Chat::Server<MAX_MESSAGE_SIZE>::Server() : m_init(false), m_clientConnected(false), m_socket(0), m_clientSocket(0) {}

template <size_t MAX_MESSAGE_SIZE>
std::expected<void, Chat::ErrorCode> Chat::Server<MAX_MESSAGE_SIZE>::Initialize(uint16_t port, size_t listenQueueAmount)
{
    if (m_init)
    {
        return std::unexpected(Chat::ErrorCode::ALREADY_INITIALIZED);
    }

    m_init = false;

    memset(m_buffer, 0, MAX_MESSAGE_SIZE);
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == SOCKET_ERROR_RETURN_VALUE)
    {
        return std::unexpected(Chat::ErrorCode::SOCKET_INITIALIZATION_ERROR);
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int result = bind(m_socket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (result == SOCKET_ERROR_RETURN_VALUE)
    {
        return std::unexpected(Chat::ErrorCode::SOCKET_INITIALIZATION_ERROR);
    }

    result = listen(m_socket, listenQueueAmount);
    if (result == SOCKET_ERROR_RETURN_VALUE)
    {
        return std::unexpected(Chat::ErrorCode::SOCKET_INITIALIZATION_ERROR);
    }

    m_init = true;
    return {};
}
template <size_t MAX_MESSAGE_SIZE>
std::expected<void, Chat::ErrorCode> Chat::Server<MAX_MESSAGE_SIZE>::AcceptClient()
{
    if (!m_init)
    {
        return std::unexpected(Chat::ErrorCode::UNINITIALIZED);
    }

    m_clientSocket = accept(m_socket, nullptr, nullptr);
    if (m_clientSocket == SOCKET_ERROR_RETURN_VALUE)
    {
        return std::unexpected(Chat::ErrorCode::CLIENT_CONNECTION_INITIALIZATION_ERROR);
    }

    printf("Client connected!\n");
    m_clientConnected = true;
    return {};
}

template <size_t MAX_MESSAGE_SIZE>
std::expected<void, Chat::ErrorCode> Chat::Server<MAX_MESSAGE_SIZE>::HandleClient()
{
    std::unexpected<Chat::ErrorCode> errorCode = std::unexpected(Chat::ErrorCode::UNINITIALIZED);
    if (!m_init)
    {
        return std::unexpected(Chat::ErrorCode::UNINITIALIZED);
    }

    if (!m_clientConnected)
    {
        return std::unexpected(Chat::ErrorCode::CLIENT_NOT_CONNECTED);
    }

    ssize_t sendRecvResult = 0;

    while (true)
    {
        sendRecvResult = recv(m_clientSocket, m_buffer, MAX_MESSAGE_SIZE, 0);

        // amount received in 'sendRecvResult'
        if (sendRecvResult == SOCKET_ERROR_RETURN_VALUE || sendRecvResult == 0)
        {
            errorCode = std::unexpected(Chat::ErrorCode::RECEIVE_ERROR);
            break;
        }

        if (static_cast<size_t>(sendRecvResult) > MAX_MESSAGE_SIZE)
        {
            errorCode = std::unexpected(Chat::ErrorCode::MESSAGE_RECEIVED_TOO_BIG);
            break;
        }

        printf("Message received from client!\n");

        sendRecvResult = send(m_clientSocket, m_buffer, sendRecvResult, 0);
        if (sendRecvResult == SOCKET_ERROR_RETURN_VALUE)
        {
            errorCode = std::unexpected(Chat::ErrorCode::SEND_ERROR);
            break;
        }
        printf("Echoed to client!\n");
    }

    m_clientConnected = false;
    close(m_clientSocket);
    return errorCode;
}

template <size_t MAX_MESSAGE_SIZE>
Chat::Server<MAX_MESSAGE_SIZE>::~Server()
{
    if (!m_init)
    {
        return; // Don't close unopened socket
    }
    close(m_socket);
    if (m_clientConnected)
    {
        close(m_clientSocket);
    }

    m_init = false;
}
#endif