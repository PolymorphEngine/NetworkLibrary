/*
** EPITECH PROJECT, 2020
** UnknownAuthorizationKey.hpp
** File description:
** header for UnknownAuthorizationKey.c
*/

#pragma once

#include <exception>
#include <string>

namespace polymorph::network::exceptions
{

    class UnauthorizedException : public std::exception
    {

            ////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            UnauthorizedException(std::string message) : _message(message) {};

            ~UnauthorizedException() override = default;

            const char *what() const noexcept {
                return _message.c_str();
            };

        private:
            std::string _message;
    };
}