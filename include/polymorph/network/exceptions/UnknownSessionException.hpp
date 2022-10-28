/*
** EPITECH PROJECT, 2020
** UnknownEndpointException.hpp
** File description:
** header for UnknownEndpointException.c
*/

#pragma once

#include <exception>
#include <string>

namespace polymorph::network::exceptions
{

    class UnknownSessionException : public std::exception
    {

            ////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            UnknownSessionException(std::string message) : _message(message) {};

            ~UnknownSessionException() override = default;

            const char *what() const noexcept {
                return _message.c_str();
            };

        private:
            std::string _message;
    };
}