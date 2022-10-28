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
        /**
         * @property The packet operation code, used to identify the packet action
         */
        OpId opId;

        /**
         * @property The packet identifier, used to uniquely identify a packet (in a short time)
         */
        PacketId pId;

        /**
         * @property The session identifier, used to identify the session
         */
        SessionId sId;

        /**
         * @property The packet payload size, used to know the size of the payload
         */
        PayloadSize pSize;
    }
#ifdef _WIN32
#pragma pack(pop)
#else
        __attribute__((packed))
#endif
    ;

}