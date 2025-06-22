#ifndef USER_MANAGER_HPP
#define USER_MANAGER_HPP

#include "UserEntity.hpp"
#include "ErrorCodes.h"
#include "Message.hpp"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <expected>
#include <stdint.h>

static constexpr size_t MEMCMP_EQUAL_VALUE = 0;

namespace Chat
{
    template <size_t MAX_USERS, size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
    class UserManager
    {
    public:
        UserManager();
        ~UserManager();

        /**
         * @brief Adds a user to the Manager
         *
         * @param usernameToAdd
         *
         * @returns ErrorCode on error
         */
        std::expected<void, ErrorCode> AddUser(UserEntity<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> usernameToAdd);

        /**
         * @brief Get a message for the said username. In case of no message, size will be 0.
         *
         * @param username
         * @param usernameLength
         *
         * @returns The message designated for said user, or ErrorCode
         *
         */
        std::expected<Message<MAX_MESSAGE_SIZE>, ErrorCode> GetMessage(uint8_t *username, size_t usernameLength);

        /**
         * @brief Broadcasts a message from provided user to all other users
         *
         * @param username The source username
         * @param usernameLength
         *
         * @param message The message to broadcast
         * @param messageLength
         *
         * @returns ErrorCode on error.
         */
        std::expected<void, ErrorCode> BroadcastMessage(uint8_t *username, size_t usernameLength, uint8_t *message, size_t messageLength);

    private:
        size_t m_dictionarySize = 0;
        UserEntity<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> m_userToMessageQueue[MAX_USERS];
    };

    template <size_t MAX_USERS, size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
    UserManager<MAX_USERS, MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH>::UserManager() : m_dictionarySize(0)
    {
        memset(m_userToMessageQueue, 0, MAX_USERS * sizeof(UserEntity<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH>));
    }

    template <size_t MAX_USERS, size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
    std::expected<void, ErrorCode> UserManager<MAX_USERS, MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH>::AddUser(UserEntity<MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH> usernameToAdd)
    {
        if (m_dictionarySize == MAX_USERS)
        {
            return std::unexpected(ErrorCode::DATA_STRUCTURE_FULL);
        }

        m_userToMessageQueue[m_dictionarySize++] = usernameToAdd;
        return {};
    }

    template <size_t MAX_USERS, size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
    std::expected<Message<MAX_MESSAGE_SIZE>, ErrorCode> UserManager<MAX_USERS, MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH>::GetMessage(uint8_t *username, size_t usernameLength)
    {
        if (username == nullptr || usernameLength > MAX_USERNAME_LENGTH)
        {
            return std::unexpected(ErrorCode::INVALID_ARGUMENT);
        }

        for (size_t i = 0; i < m_dictionarySize; i++)
        {
            if (m_userToMessageQueue[i].usernameLength == usernameLength && memcmp(m_userToMessageQueue[i].username, username, usernameLength) == MEMCMP_EQUAL_VALUE)
            {
                return m_userToMessageQueue[i]->userMessageQueue.Pop();
            }
        }

        return std::unexpected(ErrorCode::NOT_FOUND);
    }

    template <size_t MAX_USERS, size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
    std::expected<void, ErrorCode> UserManager<MAX_USERS, MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH>::BroadcastMessage(uint8_t *username, size_t usernameLength, uint8_t *message, size_t messageLength)
    {
        if (username == nullptr || message == nullptr || usernameLength > MAX_USERNAME_LENGTH || messageLength > MAX_MESSAGE_SIZE)
        {
            return std::unexpected(ErrorCode::INVALID_ARGUMENT);
        }

        for (size_t i = 0; i < m_dictionarySize; i++)
        {
            if (m_userToMessageQueue[i].usernameLength == usernameLength && memcmp(m_userToMessageQueue[i].username, username, usernameLength) == MEMCMP_EQUAL_VALUE)
            {
                continue; // Don't broadcast to self
            }

            m_userToMessageQueue[i]->userMessageQueue.Push(message, messageLength);
        }

        return {};
    }

    UserManager<MAX_USERS, MAX_MESSAGES, MAX_MESSAGE_SIZE, MAX_USERNAME_LENGTH>::~UserManager()
    {
        m_dictionarySize = 0;
    }

} // namespace Chat

#endif