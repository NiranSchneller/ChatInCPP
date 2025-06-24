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
    template <size_t MAX_USERS, size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
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
        fd_t m_socket = 0;
        size_t m_currentClientIndex = 0;

        Poller<MAX_USERS> m_poller;
        UserManager<MAX_USERS, MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> m_userManager;
        uint8_t m_buffer[MAX_MESSAGE_SIZE];

        std::expected<void, ErrorCode> AddClientSocketToPoll(Chat::UserEntity<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> **clientSocketsUserEntities,
                                                             PollerData<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> **clientSocketsPollerDatas,
                                                             size_t currentIndex);
    };
} // namespace Chat

static constexpr int SOCKET_ERROR_RETURN_VALUE = -1;

template <size_t MAX_USERS, size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
Chat::Server<MAX_USERS, MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH>::Server() : m_init(), m_socket(), m_currentClientIndex() {}

template <size_t MAX_USERS, size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
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
template <size_t MAX_USERS, size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
std::expected<void, Chat::ErrorCode> Chat::Server<MAX_USERS, MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH>::Run()
{
    ssize_t sendRecvResult = 0;

    if (!m_init)
    {
        return std::unexpected(Chat::ErrorCode::UNINITIALIZED);
    }

    std::expected<size_t, Chat::ErrorCode> getActiveSocketsReturnCode;
    Chat::PollerData<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> serverSocketPollerData;
    Chat::UserEntity<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> serverUserEntity;

    memset(serverUserEntity.username, 0, MAX_USERNAME_LENGTH);
    serverUserEntity.usernameLength = 0;

    serverSocketPollerData.fileDescriptor = m_socket;
    printf("Server socket fd: %d\n", m_socket);
    serverSocketPollerData.userEntity = &serverUserEntity;

    std::expected<void, Chat::ErrorCode> pollerReturnCode = m_poller.AddToPoll(&serverSocketPollerData);
    if (!pollerReturnCode.has_value())
    {
        return pollerReturnCode;
    }

    Chat::UserEntity<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> *clientSocketsUserEntities[MAX_USERS];
    PollerData<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> *clientSocketsPollerDatas[MAX_USERS];

    std::expected<struct PollerData<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> *, ErrorCode> matchingUserForFD;
    size_t currentIndex = 0;

    PollerData<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> *activeSocketsPollerDatas[MAX_USERS];
    while (true)
    {
        getActiveSocketsReturnCode = m_poller.GetActive(activeSocketsPollerDatas, MAX_USERS);
        if (!getActiveSocketsReturnCode.has_value())
        {
            printf("Poller getActive Failed! Critical error!\n");
            return {};
        }

        // Iterate over all active sockets received, send
        for (size_t i = 0; i < getActiveSocketsReturnCode.value(); i++)
        {
            printf("Active FD: %d\n", activeSocketsPollerDatas[i]->fileDescriptor);

            if (activeSocketsPollerDatas[i]->fileDescriptor == m_socket)
            {
                if (currentIndex >= MAX_USERS)
                {
                    printf("Max Users reached! Can't accept new client!");
                    continue;
                }

                printf("test\n");
                if (!AddClientSocketToPoll(clientSocketsUserEntities, clientSocketsPollerDatas, currentIndex).has_value())
                {
                    printf("AddClientSocketToPoll failed!\n");
                    continue;
                }
                currentIndex++;
                m_currentClientIndex++;
                continue;
            }

            sendRecvResult = recv(activeSocketsPollerDatas[i]->fileDescriptor, m_buffer, MAX_MESSAGE_SIZE, 0);

            // amount received in 'sendRecvResult'
            if (sendRecvResult == SOCKET_ERROR_RETURN_VALUE || sendRecvResult == 0)
            {
                printf("Could not broadcast FD Message: %d\n", activeSocketsPollerDatas[i]->fileDescriptor);
                continue;
            }
            matchingUserForFD = m_poller.template GetPollerDataByFD<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH>(activeSocketsPollerDatas[i]->fileDescriptor);
            if (!matchingUserForFD.has_value())
            {
                printf("Poller couldnt find Matching data by FD!\n");
                continue;
            }

            if (!m_userManager.BroadcastMessage(matchingUserForFD.value()->userEntity->username, matchingUserForFD.value()->userEntity->usernameLength, m_buffer, sendRecvResult).has_value())
            {
                printf("Could not broadcast message!\n");
                continue;
            }
        }

        // Send all messages
        for (size_t i = 0; i < currentIndex; i++)
        {
            std::expected<Message<MAX_MESSAGE_SIZE>, ErrorCode> retCode = m_userManager.GetMessage(clientSocketsPollerDatas[i]->userEntity->username, clientSocketsPollerDatas[i]->userEntity->usernameLength);
            if (!retCode.has_value())
            {
                printf("Could not get message for FD: %d\n", clientSocketsPollerDatas[i]->fileDescriptor);
                continue;
            }

            if (send(clientSocketsPollerDatas[i]->fileDescriptor, retCode.value().message, retCode.value().messageSize, 0) == SOCKET_ERROR_RETURN_VALUE)
            {
                printf("Could not send message to FD: %d\n", clientSocketsPollerDatas[i]->fileDescriptor);
            }
        }
    }
    return {};
}

template <size_t MAX_USERS, size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
std::expected<void, Chat::ErrorCode> Chat::Server<MAX_USERS, MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH>::AddClientSocketToPoll(Chat::UserEntity<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> **clientSocketsUserEntities,
                                                                                                                                         PollerData<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> **clientSocketsPollerDatas,
                                                                                                                                         size_t currentIndex)
{
    fd_t clientSocket = accept(m_socket, NULL, NULL);
    if (clientSocket == SOCKET_ERROR_RETURN_VALUE)
    {
        return std::unexpected(Chat::ErrorCode::CLIENT_CONNECTION_INITIALIZATION_ERROR);
    }
    printf("Assigning FD!\n");
    clientSocketsPollerDatas[currentIndex]->fileDescriptor = clientSocket;

    printf("Setting username!\n");
    memset(clientSocketsUserEntities[currentIndex]->username, 0, MAX_USERNAME_LENGTH);
    clientSocketsUserEntities[currentIndex]->usernameLength = m_currentClientIndex;

    printf("Setting user entity!\n");
    clientSocketsPollerDatas[currentIndex]->userEntity = clientSocketsUserEntities[currentIndex];

    printf("Adding user!\n");
    return m_userManager.AddUser(clientSocketsPollerDatas[currentIndex]->userEntity);
}

template <size_t MAX_USERS, size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
Chat::Server<MAX_USERS, MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH>::~Server()
{
    if (!m_init)
    {
        return; // Don't close unopened socket
    }
    close(m_socket);

    m_init = false;
}
#endif