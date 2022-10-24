/*
** EPITECH PROJECT, 2020
** types.hpp
** File description:
** header for types.c
*/

#pragma once

#include <chrono>
#include <cstdint>
#include <array>

namespace polymorph::network
{

    using OpId = std::uint16_t;

    using PacketId = std::uint32_t;

    using SessionId = std::uint16_t;

    using PayloadSize = std::uint16_t;

    using AuthorizationKey = std::array<std::uint8_t, 16>;

    constexpr std::chrono::milliseconds DEFAULT_UDP_TIMEOUT = std::chrono::milliseconds(300);

    constexpr std::uint8_t DEFAULT_UDP_RETRIES = 10;

}
