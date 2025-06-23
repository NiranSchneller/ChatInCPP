#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP

#include <stddef.h>
#include <expected>
#include <stdint.h>
#include <string.h>

#include "Message.hpp"
#include "ErrorCodes.h"

namespace Chat
{
    template <size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE>
    class MessageQueue
    {
    public:
        MessageQueue();
        ~MessageQueue();

        /**
         * @brief Push a message into the queue. If the queue is full, return an error.
         *
         * @param message The message to push
         * @param messageLength The length of message
         *
         * @returns ErrorCode
         */
        std::expected<void, ErrorCode> Push(uint8_t *message, size_t messageLength);

        /**
         * @brief Pop a message from the queue. If queue is empty return an error
         *
         * @returns Message or error code
         */
        std::expected<Message<MAX_MESSAGE_SIZE>, ErrorCode> Pop();

        /**
         * @returns The size of the queue
         */
        size_t GetQueueSize();

    private:
        size_t m_queueSize = 0;
        size_t m_firstVacantSlot = 0;
        Message<MAX_MESSAGE_SIZE> m_messagesBuffer[MAX_MESSAGES] = {};
    };

    template <size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE>
    MessageQueue<MAX_MESSAGES, MAX_MESSAGE_SIZE>::MessageQueue() : m_queueSize(0), m_firstVacantSlot(0) {}

    template <size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE>
    std::expected<void, ErrorCode> MessageQueue<MAX_MESSAGES, MAX_MESSAGE_SIZE>::Push(uint8_t *message, size_t messageLength)
    {
        if (m_queueSize == MAX_MESSAGES)
        {
            return std::unexpected(ErrorCode::DATA_STRUCTURE_FULL);
        }

        if (messageLength > MAX_MESSAGE_SIZE || message == nullptr)
        {
            return std::unexpected(ErrorCode::INVALID_ARGUMENT);
        }

        m_messagesBuffer[m_firstVacantSlot] = Message<MAX_MESSAGE_SIZE>();
        memcpy(m_messagesBuffer[m_firstVacantSlot].message, message, messageLength);
        m_messagesBuffer[m_firstVacantSlot].messageSize = messageLength;

        m_queueSize++;
        m_firstVacantSlot++;

        return {};
    }

    template <size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE>
    std::expected<Message<MAX_MESSAGE_SIZE>, ErrorCode> MessageQueue<MAX_MESSAGES, MAX_MESSAGE_SIZE>::Pop()
    {
        if (m_queueSize == 0)
        {
            return std::unexpected(ErrorCode::DATA_STRUCTURE_EMPTY);
        }

        m_queueSize--;
        return m_messagesBuffer[--m_firstVacantSlot];
    }

    template <size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE>
    size_t MessageQueue<MAX_MESSAGES, MAX_MESSAGE_SIZE>::GetQueueSize()
    {
        return m_queueSize;
    }

    template <size_t MAX_MESSAGES, size_t MAX_MESSAGE_SIZE>
    MessageQueue<MAX_MESSAGES, MAX_MESSAGE_SIZE>::~MessageQueue()
    {
        m_queueSize = 0;
        m_firstVacantSlot = 0;
    }

} // namespace Chat

#endif