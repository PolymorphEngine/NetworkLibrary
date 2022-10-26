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
#include "Polymorph/Network/exceptions/DeserializingException.hpp"

namespace polymorph::network
{

    /**
     * @brief SerializerTrait struct to serialize and deserialize a type before/after a transfer over the network
     * @tparam T The type to serialize/deserialize
     */
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

            if (buffer.size() < sizeof(T))
                throw exceptions::DeserializingException("Buffer size does not match the size of the type");

            std::memcpy(&dto, buffer.data(), sizeof(T));
            return dto;
        }

    };

    template<typename T>
    struct SerializerTrait<Packet<T>, true>
    {

        static std::vector<std::byte> serialize(const Packet<T> &packet)
        {
            std::vector<std::byte> buffer;
            auto payload = SerializerTrait<T>::serialize(packet.payload);
            PacketHeader header = packet.header;
            header.pSize = payload.size();
            auto serializedHeader = SerializerTrait<PacketHeader>::serialize(header);

            buffer.reserve(sizeof(PacketHeader) + payload.size());
            std::copy(serializedHeader.begin(), serializedHeader.end(), std::back_inserter(buffer));
            std::copy(payload.begin(), payload.end(), std::back_inserter(buffer));

            return buffer;
        }

        static Packet<T> deserialize(const std::vector<std::byte> &buffer)
        {
            Packet<T> packet = {};
            std::vector<std::byte> payloadBuffer;

            if (buffer.size() < sizeof(PacketHeader))
                throw exceptions::DeserializingException("Buffer size does not contains a packet header");

            payloadBuffer.reserve(buffer.size() - sizeof(PacketHeader));
            std::copy(buffer.begin() + sizeof(PacketHeader), buffer.end(), std::back_inserter(payloadBuffer));
            packet.header = SerializerTrait<PacketHeader>::deserialize(buffer);

            if (buffer.size() < sizeof(PacketHeader) + packet.header.pSize)
                throw exceptions::DeserializingException("Buffer size does not contains a packet");

            packet.payload = SerializerTrait<T>::deserialize(payloadBuffer);
            return packet;
        }

    };

    template<typename T>
    struct SerializerTrait<Packet<T>, false>
    {

            static std::vector<std::byte> serialize(const Packet<T> &packet)
            {
                std::vector<std::byte> buffer;
                auto payload = SerializerTrait<T>::serialize(packet.payload);
                PacketHeader header = packet.header;
                header.pSize = payload.size();
                auto serializedHeader = SerializerTrait<PacketHeader>::serialize(header);

                buffer.reserve(sizeof(PacketHeader) + payload.size());
                std::copy(serializedHeader.begin(), serializedHeader.end(), std::back_inserter(buffer));
                std::copy(payload.begin(), payload.end(), std::back_inserter(buffer));

                return buffer;
            }

            static Packet<T> deserialize(const std::vector<std::byte> &buffer)
            {
                Packet<T> packet = {};
                std::vector<std::byte> payloadBuffer;

                if (buffer.size() < sizeof(PacketHeader))
                    throw exceptions::DeserializingException("Buffer size does not contains a packet header");

                payloadBuffer.reserve(buffer.size() - sizeof(PacketHeader));
                std::copy(buffer.begin() + sizeof(PacketHeader), buffer.end(), std::back_inserter(payloadBuffer));
                packet.header = SerializerTrait<PacketHeader>::deserialize(buffer);

                if (buffer.size() < sizeof(PacketHeader) + packet.header.pSize)
                    throw exceptions::DeserializingException("Buffer size does not contains a packet");
                packet.payload = SerializerTrait<T>::deserialize(payloadBuffer);
                return packet;
            }

    };

}