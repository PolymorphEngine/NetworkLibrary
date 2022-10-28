/*
** EPITECH PROJECT, 2020
** ConnectionDto.hpp
** File description:
** header for ConnectionDto.c
*/

#pragma once

#include "Polymorph/Network/types.hpp"

namespace polymorph::network
{

#ifdef _WIN32
#pragma pack(push, 1)
#endif
    /**
     * @struct Data transfer object for responses to the incoming connection packet
     */
    struct ConnectionResponseDto
    {
        /**
         * @property The packet operation code
         */
        static constexpr OpId opId = 0;

        /**
         * @property The authentication status
         *
         * @return  true    if the connection was accepted and authenticated
         * @return  false   if the connection was refused or the authentication failed
         */
        bool authorized;
    }
#ifdef _WIN32
        ;
    #pragma pack(pop)
#else
        __attribute__((packed));
#endif

} // polymorph::network