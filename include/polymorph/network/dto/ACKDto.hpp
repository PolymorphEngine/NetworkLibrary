/*
** EPITECH PROJECT, 2020
** ConnectionDto.hpp
** File description:
** header for ConnectionDto.c
*/

#pragma once

#include "polymorph/network/types.hpp"

namespace polymorph::network
{

#ifdef _WIN32
#pragma pack(push, 1)
#endif
    /**
     * @struct Data transfer object for the ACK packet
     */
    struct ACKDto
    {
        /**
         * @property The packet operation code
         */
        static constexpr OpId opId = 1;

        /**
         * @property The packet identifier
         */
        PacketId id;
    }
#ifdef _WIN32
        ;
    #pragma pack(pop)
#else
        __attribute__((packed));
#endif

} // polymorph::network