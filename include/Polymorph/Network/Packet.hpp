/*
** EPITECH PROJECT, 2020
** Packet.hpp
** File description:
** header for Packet.c
*/

#pragma once

#include "Polymorph/Network/PacketHeader.hpp"

namespace polymorph::network
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

        /**
         * @property The packet payload, contains the data to send
         * @tparam T The type of the payload
         */
        T payload;
    }
#ifdef _WIN32
    ;
    #pragma pack(pop)
#else
    __attribute__((packed));
#endif

}