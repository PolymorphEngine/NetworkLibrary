/*
** EPITECH PROJECT, 2020
** IPacketHandler.hpp
** File description:
** header for IPacketHandler.c
*/

#pragma once

#include "Polymorph/Network/PacketHeader.hpp"
#include <vector>

namespace polymorph::network::tcp {

    class IPacketHandler {

        public:
            virtual ~IPacketHandler() = default;

            virtual bool packetReceived(const PacketHeader &, const std::vector<std::byte> &bytes) = 0;

    };
}