#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <stdint.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <expected>

#include "ErrorCodes.h"
#include "UserManager.hpp"
#include "Poller.hpp"
#include "PollerData.hpp"

namespace Chat
{
    /**
     * @brief Represents a server which can sequentially accept incoming client connections
     */
    using fd_t = int;
    template <size_t MAX_USERS, size_t MAX_MESSAGES_PER_USER, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
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

        std::expected<void, ErrorCode> Run();

    private:
        bool m_init = false;
        bool m_clientConnected = false;
        fd_t m_socket = 0;
        fd_t m_clientSocket = 0;

        Poller<MAX_USERS> m_poller;
        UserManager<MAX_USERS, MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> m_userManager;
        uint8_t m_buffer[MAX_MESSAGE_SIZE];
    };
} // namespace Chat

static constexpr int SOCKET_ERROR_RETURN_VALUE = -1;

template <size_t MAX_USERS, size_t MAX_MESSAGES_PER_USER, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
Chat::Server<MAX_USERS, MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH>::Server() : m_init(), m_clientConnected(), m_socket(), m_clientSocket() {}

template <size_t MAX_USERS, size_t MAX_MESSAGES_PER_USER, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
std::expected<void, Chat::ErrorCode> Chat::Server<MAX_USERS, MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH>::Initialize(uint16_t port, size_t listenQueueAmount)
{
    std::expected<void, Chat::ErrorCode> returnCode;
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

    returnCode = m_poller.Initialize();
    if (!returnCode.has_value())
    {
        return returnCode;
    }

    m_init = true;
    return {};
}
template <size_t MAX_USERS, size_t MAX_MESSAGES_PER_USER, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
std::expected<void, Chat::ErrorCode> Chat::Server<MAX_USERS, MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH>::Run()
{
    std::expected<void, Chat::ErrorCode> returnCode;
    Chat::PollerData<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> serverSocketPollerData;
    Chat::UserEntity<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> serverUserEntity;
    serverUserEntity.username = "";
    serverUserEntity.usernameLength = 0;

    serverSocketPollerData.fileDescriptor = m_socket;
    serverSocketPollerData.userEntity = &serverUserEntity;

    returnCode = m_poller.AddToPoll(&serverSocketPollerData);
    if (!returnCode.has_value())
    {
        return returnCode;
    }

    PollerData<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> *socketPollerDatas[MAX_USERS] = {0};
    while (true)
    {
        if (!m_poller.GetActive(socketPollerDatas, MAX_USERS).has_value())
        {
            printf("Poller getActive Failed! Critical error!\n");
        }
    }
}

template <size_t MAX_USERS, size_t MAX_MESSAGES_PER_USER, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
Chat::Server<MAX_USERS, MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH>::~Server()
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