/*
** EPITECH PROJECT, 2020
** MessageDto.hpp
** File description:
** header for MessageDto.c
*/

#pragma once

#include <string>
#include "polymorph/network/SerializerTrait.hpp"

/**
 * @brief Message DTO that will be sent over the network
 */
struct MessageDto
{
    static constexpr polymorph::network::OpId opId = 3;
    std::string message;
};

/**
 * @brief SerializerTrait specialization for MessageDto because is it not a standard layout type
 */
namespace polymorph::network
{
    template<>
    struct SerializerTrait<MessageDto>
    {
        static std::vector<std::byte> serialize(const MessageDto &data)
        {
            std::vector<std::byte> buffer(data.message.size());

            std::memcpy(buffer.data(), data.message.data(), data.message.size());
            return buffer;
        }

        static MessageDto deserialize(const std::vector<std::byte> &buffer)
        {
            MessageDto dto;

            dto.message = std::string(reinterpret_cast<const char *>(buffer.data()), buffer.size());
            return dto;
        }
    };
}

