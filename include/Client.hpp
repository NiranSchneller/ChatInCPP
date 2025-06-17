#ifndef CHAT_CLIENT_HPP
#define CHAT_CLIENT_HPP

#include <stdint.h>
#include <stddef.h>
#include <expected>
#include "ErrorCodes.h"
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>

#define SOCKET_ERROR_RETURN_VALUE -1 // Not constexpr since it is included with server.hpp and it will conflict

namespace Chat
{
    template <size_t MAX_MESSAGE_SIZE>
    class Client
    {
    public:
        Client();
        ~Client();

        /**
         * @brief Initializes client object and attempts to connect to the desired server
         *
         * @param address The IP Address of the server
         * @param port The port of the server
         *
         * @returns Error Code
         */
        std::expected<void, ErrorCode> Initialize(uint32_t address, uint16_t port);

        /**
         * @brief Sends and receives messages from the server until connection is closed
         *
         * @returns Error Code
         */
        std::expected<void, ErrorCode> CommunicateWithServer();

    private:
        bool m_init;
        int m_socket;
        char m_buffer[MAX_MESSAGE_SIZE];
    };

} // namespace Chat

template <size_t MAX_MESSAGE_SIZE>
Chat::Client<MAX_MESSAGE_SIZE>::Client() : m_init(false) {}

template <size_t MAX_MESSAGE_SIZE>
std::expected<void, Chat::ErrorCode> Chat::Client<MAX_MESSAGE_SIZE>::Initialize(uint32_t address, uint16_t port)
{
    if (m_init)
    {
        return std::unexpected(Chat::ErrorCode::ALREADY_INITIALIZED);
    }
    memset(m_buffer, 0, MAX_MESSAGE_SIZE);

    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == SOCKET_ERROR_RETURN_VALUE)
    {
        return std::unexpected(Chat::ErrorCode::SOCKET_INITIALIZATION_ERROR);
    }
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = address;
    server_addr.sin_port = htons(port);
    if (connect(m_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR_RETURN_VALUE)
    {
        close(m_socket);
        return std::unexpected(Chat::ErrorCode::SOCKET_INITIALIZATION_ERROR);
    }

    m_init = true;
    return {};
}

template <size_t MAX_MESSAGE_SIZE>
std::expected<void, Chat::ErrorCode> Chat::Client<MAX_MESSAGE_SIZE>::CommunicateWithServer()
{
    ssize_t sendRecvResult = 0;
    if (!m_init)
    {
        return std::unexpected(Chat::ErrorCode::UNINITIALIZED);
    }

    while (true)
    {

        printf("Enter message: ");
        if (fgets(m_buffer, sizeof(m_buffer), stdin) == NULL)
        {
            return std::unexpected(Chat::ErrorCode::INPUT_FROM_USER_ERROR);
        }

        sendRecvResult = send(m_socket, m_buffer, strnlen(m_buffer, MAX_MESSAGE_SIZE), 0);
        if (sendRecvResult == SOCKET_ERROR_RETURN_VALUE)
        {
            return std::unexpected(Chat::ErrorCode::SEND_ERROR);
        }

        printf("Sent to server!\n");
        sendRecvResult = recv(m_socket, m_buffer, MAX_MESSAGE_SIZE, 0);
        // amount received in 'sendRecvResult'
        if (sendRecvResult == SOCKET_ERROR_RETURN_VALUE || sendRecvResult == 0)
        {
            return std::unexpected(Chat::ErrorCode::RECEIVE_ERROR);
        }
        printf("Message received from server: %s\n", m_buffer);
    }

    return {};
}

template <size_t MAX_MESSAGE_SIZE>
Chat::Client<MAX_MESSAGE_SIZE>::~Client()
{
    if (!m_init)
    {
        return; // Don't close unopened socket
    }
    close(m_socket);
    m_init = false;
}
#endif