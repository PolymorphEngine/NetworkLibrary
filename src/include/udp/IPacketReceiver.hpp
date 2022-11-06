/*
** EPITECH PROJECT, 2020
** IPacketHandler.hpp
** File description:
** header for IPacketHandler.c
*/

#pragma once

#include <asio/ip/udp.hpp>
#include "polymorph/network/types.hpp"
#include "polymorph/network/PacketHeader.hpp"

namespace polymorph::network::udp
{
    /**
     * @brief Interface to handle packets
     */
    class IPacketReceiver
    {
        public:
            virtual ~IPacketReceiver() = default;

        protected:

            /**
             * @brief Handle a received packet
             * @param from The endpoint of the client who sent the packet
             * @param bytes The packet received in its serialized form
             */
            virtual void _packetReceived(const asio::ip::udp::endpoint& from, const std::vector<std::byte> &bytes) = 0;

            /**
             * @brief Handle a received ack packet
             * @param from The endpoint of the client who sent the packet
             * @param acknoledgedId The id of the packet that has been acknoledged
             */
            virtual void _ackReceived(const asio::ip::udp::endpoint& from, PacketId acknoledgedId) = 0;

            /**
             * @brief Handle a sent packet
             * @param to The endpoint of the client who received the packet
             * @param bytes The packet sent in its serialized form
             */
            virtual void _packetSent(const asio::ip::udp::endpoint& to, const PacketHeader &header, const std::vector<std::byte> &bytes) = 0;
    };
}