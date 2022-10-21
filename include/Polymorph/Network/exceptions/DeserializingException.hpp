/*
** EPITECH PROJECT, 2020
** DeserializingException.hpp
** File description:
** header for DeserializingException.c
*/

#pragma once

#include <exception>
#include <string>
#include <utility>

namespace polymorph::network::exceptions
{

    /**
     * @brief DeserializingException class
     * @description Exception thrown when a deserialization failed
     */
    class DeserializingException : public std::exception
    {
        public:
            DeserializingException(std::string message) : _message(std::move(message)) {}

            const char *what() const noexcept override { return _message.c_str(); }

        private:
            std::string _message;
    };

}