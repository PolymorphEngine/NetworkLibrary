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

    class AlreadyRegisteredException : public std::exception
    {

            ////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            AlreadyRegisteredException(std::string message) : _message(message) {};

            ~AlreadyRegisteredException() override = default;

            const char *what() const noexcept {
                return _message.c_str();
            };

        private:
            std::string _message;
    };
}