/*
** EPITECH PROJECT, 2020
** IPacketHandler.hpp
** File description:
** header for IPacketHandler.c
*/

#pragma once

#include "polymorph/network/PacketHeader.hpp"
#include <vector>

namespace polymorph::network::tcp {

    /**
     * @class   Interface for packet handlers
     * @note    TCP exclusive
     */
    class IPacketHandler {

        public:
            virtual ~IPacketHandler() = default;

            /**
            * @brief   Called when a packet is received
            *
            * @param   header  The packet header
            * @param   bytes   The packet bytes as a vector of bytes (with the header)
            *
            * @return  bool    True if the packet was handled correctly, false otherwise
            */
            virtual bool packetReceived(const PacketHeader &, const std::vector<std::byte> &bytes) = 0;

    };
}