/*
** EPITECH PROJECT, 2020
** IClientConnection.hpp
** File description:
** header for IClientConnection.c
*/


#pragma once

#include <vector>
#include <functional>
#include "Polymorph/Network/PacketHeader.hpp"

namespace polymorph::network::tcp
{

    class IClientConnection
    {
        public:
            virtual ~IClientConnection() = default;

            virtual void start() = 0;

            virtual void send(std::vector<std::byte> data, std::function<void (const PacketHeader &, const std::vector<std::byte> &)> callback) = 0;

            virtual bool isConnected() = 0;

            virtual SessionId getSessionId() = 0;

            virtual PacketId getPacketId() = 0;
    };
}