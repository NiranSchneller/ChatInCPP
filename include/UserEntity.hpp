#ifndef USER_ENTITY_HPP
#define USER_ENTITY_HPP

#include <stddef.h>

#include "MessageQueue.hpp"

namespace Chat
{
    template <size_t MAX_MESSAGES_PER_USER, size_t MAX_MESSAGE_SIZE, size_t MAX_USERNAME_LENGTH>
    struct UserEntity
    {
        uint8_t username[MAX_USERNAME_LENGTH];
        size_t usernameLength;
        MessageQueue<MAX_MESSAGES, MAX_MESSAGE_SIZE> userMessageQueue;
    };

} // namespace Chat

#endif