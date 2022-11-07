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
     * @struct Data transfer object for the incoming connection packet
     */
    struct DisconnectionDto
    {
        /**
         * @property The packet operation code
         */
        static constexpr OpId opId = 4;
    }
#ifdef _WIN32
        ;
    #pragma pack(pop)
#else
        __attribute__((packed));
#endif

} // polymorph::network