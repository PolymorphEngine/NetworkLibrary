/*
** EPITECH PROJECT, 2020
** Packet.hpp
** File description:
** header for Packet.c
*/

#pragma once

#include "Polymorph/Network/PacketHeader.hpp"

namespace Polymorph::Network
{

#ifdef _WIN32
#pragma pack(push, 1)
#endif
    /**
     * @brief Packet struct to store header and payload
     * @tparam T
     */
    template<typename T>
    struct Packet
    {
        PacketHeader header;
        T payload;
    }
#ifdef _WIN32
    ;
    #pragma pack(pop)
#else
    __attribute__((packed));
#endif

}