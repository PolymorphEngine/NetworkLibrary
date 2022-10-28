/*
** EPITECH PROJECT, 2020
** IClientConnection.hpp
** File description:
** header for IClientConnection.c
*/


#pragma once

#include <vector>
#include <functional>
#include "polymorph/network/PacketHeader.hpp"

namespace polymorph::network::tcp
{

    class IClientConnection
    {
        public:
            virtual ~IClientConnection() = default;

            /**
             * @brief Start accepting connection
             */
            virtual void start() = 0;

            /**
             * @brief Send a packet to the client
             * @tparam T The type of the payload
             * @param payload The payload to send
             * @param callback The callback to call when the packet is sent
             */
            virtual void send(std::vector<std::byte> data, std::function<void (const PacketHeader &, const std::vector<std::byte> &)> callback) = 0;

            /**
             * @brief Check if the client is connected
             */
            virtual bool isConnected() = 0;

            /**
             * @brief Get the current client session id
             */
            virtual SessionId getSessionId() = 0;

            /**
             * @brief Get the current last packet id
             */
            virtual PacketId getPacketId() = 0;
    };
}