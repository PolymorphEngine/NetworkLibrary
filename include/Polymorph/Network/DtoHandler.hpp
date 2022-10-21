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

namespace polymorph::network
{
    /**
     * @brief DtoHandler class to store and retrieve a function to handle a specific received packet
     * @tparam T
     */
    template<typename T>
    struct DtoHandler
    {
        private:
            /**
             * @property _handlers Map of callback functions to handle a specific packet
             */
            static inline std::map<int, std::vector<std::function<void(PacketHeader, T &)>>> _handlers;

            /**
             * @property _mutex Mutex to lock the map
             */
            static inline  std::mutex _mutex;

        public:
            /**
             * @brief Register a callback function to handle a specific packet
             * @param opId The operation id of the packet
             * @param callback The callback function to handle the packet
             */
            static void registerHandler(int id, std::function<void(PacketHeader, T &)> handler)
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _handlers[id].push_back(handler);
            }

            /**
             * @brief Cast the binary format of the packet and call all the callback functions to handle it
             * @param bytes The packet still in bytes
             */
            static void handleReceivedPacket(const std::vector<std::byte> &bytes)
            {
                Packet<T> packet = SerializerTrait<Packet<T>>::deserialize(bytes);
                std::lock_guard<std::mutex> lock(_mutex);

                for (auto &handler: _handlers[packet.header.opId])
                    handler(packet.header, packet.payload);
            }

            static void unregisterHandler(int opId)
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _handlers.erase(opId);
            }
    };
}