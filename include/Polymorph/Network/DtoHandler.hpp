/*
** EPITECH PROJECT, 2020
** DtoHandler.hpp
** File description:
** header for DtoHandler.c
*/

#pragma once

#include <map>
#include <vector>
#include <functional>
#include "Polymorph/Network/PacketHeader.hpp"
#include "Polymorph/Network/SerializerTrait.hpp"

namespace Polymorph::Network
{

    template<typename T>
    struct DtoHandler
    {
        private:
            static std::map<int, std::vector<std::function<void(PacketHeader, T &)>>> _handlers;

        public:
            static void registerHandler(int id, std::function<void(PacketHeader, T &)> handler)
            {
                _handlers[id].push_back(handler);
            }

            /**
            static void handleReceivedPacket(const std::vector<std::byte> &bytes)
            {
                Packet<T> packet = SerializerTrait<Packet<T>>::deserialize(bytes);
                for (auto &handler: _handlers[packet.header.opId])
                    handler(packet.header, packet.payload);
            }

            static void unregisterHandler(int opId)
            {
                _handlers.erase(opId);
            }
    };
}