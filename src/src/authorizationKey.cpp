/*
** EPITECH PROJECT, 2022
** authorizationKey.cpp
** File description:
** authorizationKey.cpp
*/


#include <cstring>
#include <random>
#include "authorizationKey.hpp"

namespace polymorph::network::authorizationKey
{
    AuthorizationKey generate()
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, 255);
        AuthorizationKey key{0};

        while (areSame(key, zero)) {
            for (auto &byte: key)
                byte = dis(gen);
        }
        return key;
    }

    bool areSame(const AuthorizationKey &key, const AuthorizationKey &other)
    {
        return std::memcmp(key.data(), other.data(), key.size()) == 0;
    }
}