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
    class IPacketHandler
    {
        public:
            virtual ~IPacketHandler() = default;

            /**
             * @brief Handle a received packet
             * @param from The endpoint of the client who sent the packet
             * @param bytes The packet received in its serialized form
             */
            virtual void packetReceived(const asio::ip::udp::endpoint& from, const std::vector<std::byte> &bytes) = 0;

            /**
             * @brief Handle a received ack packet
             * @param from The endpoint of the client who sent the packet
             * @param acknoledgedId The id of the packet that has been acknoledged
             */
            virtual void ackReceived(const asio::ip::udp::endpoint& from, PacketId acknoledgedId) = 0;

            /**
             * @brief Handle a sent packet
             * @param to The endpoint of the client who received the packet
             * @param bytes The packet sent in its serialized form
             */
            virtual void packetSent(const asio::ip::udp::endpoint& to, const PacketHeader &header, const std::vector<std::byte> &bytes) = 0;

            /**
             * @brief Get a reference to the socket tu use to send and receive packets
             * @return A reference to the socket
             */
            virtual asio::ip::udp::socket &getPreparedSocket() = 0;

            /**
             * @brief stat the receive and run the io_context
             */
            virtual void start() = 0;
    };
}