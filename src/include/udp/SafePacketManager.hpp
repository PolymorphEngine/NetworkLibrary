/*
** EPITECH PROJECT, 2020
** SafePacketManager.hpp
** File description:
** header for SafePacketManager.c
*/

#pragma once

#include <asio/steady_timer.hpp>
#include <vector>

namespace polymorph::network::udp
{
    class SafePacketManager
    {
        public:
            asio::steady_timer timer;
            PacketHeader header;
            std::vector<std::byte> sPacket;
            std::uint8_t remainingRetries;
            asio::ip::udp::endpoint endpoint;

            SafePacketManager(asio::steady_timer timer, PacketHeader header, std::vector<std::byte> bytes, uint8_t remainingRetries, asio::ip::udp::endpoint endpoint)
                    : timer(std::move(timer)), header(std::move(header)), sPacket(std::move(bytes)), remainingRetries(remainingRetries) , endpoint(std::move(endpoint))
            {};
    };
}