/*
** EPITECH PROJECT, 2020
** PacketHeader.hpp
** File description:
** header for PacketHeader.c
*/


#pragma once

#include <cstring>
#include <vector>
#include "Polymorph/Network/types.hpp"

namespace polymorph::network
{
    /**
     * @brief Packet header struct
     */
#ifdef _WIN32
#pragma pack(push, 1)
#endif
    struct PacketHeader
    {
        OpId opId;
        PacketId pId;
        SessionId sId;
        PayloadSize pSize;
    }
#ifdef _WIN32
#pragma pack(pop)
#else
        __attribute__((packed))
#endif
    ;

}