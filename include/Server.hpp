#ifndef SERVER_H
#define SERVER_H
#include <sys/socket.h>
#include "ErrorCodes.h"
#include <stdint.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <expected>
namespace Chat
{
    template <size_t MAX_MESSAGE_SIZE>
    class Server
    {
    public:
        Server();

        /**
         * @brief Initializes a socket based on params given
         * @param port The port the socket will bind to
         * @param listeningQueueAmount The amount of clients allowed to wait to connect to the server
         *
         * @returns Status Code
         */
        std::expected<void, ErrorCode> Initialize(uint16_t port, size_t listenQueueAmount);

        /**
         * @brief Handles a client and echoes messages until connection is closed
         *
         * @return Status code
         */
        std::expected<void, ErrorCode> HandleClient();

        ~Server();

    private:
        bool m_init;
        int m_socket;
        uint8_t m_buffer[MAX_MESSAGE_SIZE];
    };
} // namespace Chat

static constexpr int SOCKET_ERROR_RETURN_VALUE = -1;

template <size_t MAX_MESSAGE_SIZE>
Chat::Server<MAX_MESSAGE_SIZE>::Server() : m_init(false) {}

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
std::expected<void, Chat::ErrorCode> Chat::Server<MAX_MESSAGE_SIZE>::HandleClient()
{
    if (!m_init)
    {
        return std::unexpected(Chat::ErrorCode::UNINITIALIZED);
    }

    ssize_t sendRecvResult = 0;
    int clientSocket = accept(m_socket, nullptr, nullptr);
    if (clientSocket == SOCKET_ERROR_RETURN_VALUE)
    {
        return std::unexpected(Chat::ErrorCode::CLIENT_CONNECTION_INITIALIZATION_ERROR);
    }
    printf("Client connected!\n");
    while (true)
    {
        sendRecvResult = recv(clientSocket, m_buffer, MAX_MESSAGE_SIZE, 0);
        // amount received in 'sendRecvResult'
        if (sendRecvResult == SOCKET_ERROR_RETURN_VALUE || sendRecvResult == 0)
        {
            return std::unexpected(Chat::ErrorCode::RECEIVE_ERROR);
        }
        printf("Message received from client!\n");

        sendRecvResult = send(clientSocket, m_buffer, sendRecvResult, 0);
        if (sendRecvResult == SOCKET_ERROR_RETURN_VALUE)
        {
            return std::unexpected(Chat::ErrorCode::SEND_ERROR);
        }
        printf("Echoed to client!\n");
    }

    return {};
}

template <size_t MAX_MESSAGE_SIZE>
Chat::Server<MAX_MESSAGE_SIZE>::~Server()
{
    if (!m_init)
    {
        return; // Don't close unopened socket
    }
    close(m_socket);
    m_init = false;
}
#endif