#pragma once

#include "net_common.h"

namespace olc {
    namespace net {
        // Message header is sent at start of all messages. The template allows us
        // to use "enum class" to ensure that the messages ar valid at compile time.
        template<typename T>
        struct message_header {
            T id{};
            uint32_t size = 0;
        };

        template<typename T>
        struct message {
            message_header<T> header{};
            std::vector<uint8_t> body;

            // returns size of entire message packet in bytes
            size_t size() const {
                return sizeof(message_header<T>) + body.size();
            }

            // Override for std::cout compatibility - produces friendly description of message
            friend std::ostream &operator<<(std::ostream &os, const message<T> &msg) {
                os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size;
                return os;
            }

            // pushes any POD-like data into the message buffer
            template<typename DataType>
            friend message<T> &operator<<(message<T> &msg, const DataType &data) {
                // check that the type of the data being pushed is trivially copyable
                static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");
                // cache current size of vector, as this will be the point we insert the data
                size_t i = msg.body.size();

                // resize the vector by the size of the data being pushed
                msg.body.resize(msg.body.size() + sizeof(DataType));

                // physically copy the data into newly allocated vector space
                std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

                // recalculate the message size
                msg.header.size = msg.size();
                return msg;
            }

            template<typename DataType>
            friend message<T> &operator>>(message<T> &msg, DataType &data) {

                // check that the type of the data being pushed is trivially copyable
                static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");
                // cache the location towards the end of the vector where the pulled data starts
                size_t i = msg.body.size() - sizeof(DataType);

                // physically copy the data from the vector into the user variable
                std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

                // shrink the vector to remove read bytes, and reset end position
                msg.body.resize(i);

                return msg;

            }
        };

        template<typename T>
        class connection;

        template<typename T>
        struct owned_message {
            std::shared_ptr<connection<T>> remote = nullptr;
            message<T> msg;

            friend std::ostream &operator<<(std::ostream &os, const owned_message<T> &msg) {
                os << msg.msg;
                return os;
            }
        };
    }
}