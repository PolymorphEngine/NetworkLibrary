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

    class UnknownEndpointException : public std::exception
    {

            ////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            UnknownEndpointException(std::string message) : _message(message) {};

            ~UnknownEndpointException() override = default

            const char *what() const noexcept {
                return _message.c_str();
            };

        private:
            std::string _message;
    };
}