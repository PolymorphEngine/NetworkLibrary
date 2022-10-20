/*
** EPITECH PROJECT, 2020
** types.hpp
** File description:
** header for types.c
*/

#pragma once

#include <cstdint>

namespace Polymorph::Network
{

    using OpId = std::uint16_t;

    using PacketId = std::uint32_t;

    using SessionId = std::uint16_t;

    using PayloadSize = std::uint16_t;

}
