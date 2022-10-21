/*
** EPITECH PROJECT, 2020
** AuthorizationKey.hpp
** File description:
** header for AuthorizationKey.c
*/

#pragma once

#include "Polymorph/Network/types.hpp"

namespace polymorph::network
{
    namespace authorizationKey
    {
        /**
         * @brief Generate a random authorization key
         * @return The generated key
         */
        AuthorizationKey generate();

        /**
         * @brief Check if the given key is valid
         * @param key The key to check
         * @return True if the key is valid, false otherwise
         */
        bool areSame(const AuthorizationKey &key, const AuthorizationKey &other);
    }
};