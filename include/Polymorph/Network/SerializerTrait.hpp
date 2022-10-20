/*
** EPITECH PROJECT, 2020
** SerializerTrait.hpp
** File description:
** header for SerializerTrait.c
*/

#pragma once

#include <cstring>
#include <vector>
#include "Polymorph/Network/Packet.hpp"

namespace Polymorph::Network
{

    template<typename T, bool = std::is_standard_layout<T>::value && std::is_trivial<T>::value>
    struct SerializerTrait
    {
    };

    template<typename T>
    struct SerializerTrait<T, true>
    {

        static std::vector<std::byte> serialize(const T &data)
        {
            std::vector<std::byte> buffer(sizeof(T));

            std::memcpy(buffer.data(), &data, sizeof(T));
            return buffer;
        }

        static T deserialize(const std::vector<std::byte> &buffer)
        {
            T dto;

            std::memcpy(&dto, buffer.data(), sizeof(T));
            return dto;
        }

    };

    template<typename T>
    struct SerializerTrait<Packet<T>, false>
    {

            static std::vector<std::byte> serialize(const Packet<T> &packet)
            {
                std::vector<std::byte> buffer;
                auto header = SerializerTrait<PacketHeader>::serialize(packet.header);
                auto payload = SerializerTrait<T>::serialize(packet.payload);

                buffer.reserve(sizeof(PacketHeader) + sizeof(T));
                std::copy(header.begin(), header.end(), buffer.begin());
                std::copy(payload.begin(), payload.end(), std::back_inserter(buffer));

                return buffer;
            }

            static Packet<T> deserialize(const std::vector<std::byte> &buffer)
            {
                Packet<T> packet;
                std::vector<std::byte> payloadBuffer(sizeof(T));

                std::copy(buffer.begin() + sizeof(PacketHeader), buffer.end(), std::back_inserter(payloadBuffer));
                packet.header = SerializerTrait<PacketHeader>::deserialize(buffer);
                packet.payload = SerializerTrait<T>::deserialize(payloadBuffer);
                return packet;
            }

    };

}