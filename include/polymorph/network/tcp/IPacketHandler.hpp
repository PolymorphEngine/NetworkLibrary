/*
** EPITECH PROJECT, 2020
** IPacketHandler.hpp
** File description:
** header for IPacketHandler.c
*/

#pragma once

#include <functional>
#include "polymorph/network/SerializerTrait.hpp"

namespace polymorph::network::tcp {

    /**
     * @class   Interface for packet handlers
     * @note    TCP exclusive
     */
    class IPacketHandler {

        public:
            virtual ~IPacketHandler() = default;

            /**
             * @brief   Register the receive handler for the given operation code
             *
             * @param   opId    The operation code that will be associated with the receive handler function
             * @param   handler The receive handler function
             */
            template<typename T>
            void registerReceiveHandler(polymorph::network::OpId opId, std::function<int(const PacketHeader &, const T &)> handler)
            {
                _registerReceiveHandler(opId, [handler](const PacketHeader &header, const std::vector<std::byte> &bytes) {
                    Packet<T> packet = SerializerTrait<Packet<T>>::deserialize(bytes);
                    return handler(header, packet.payload);
                });
            }

            /**
             * @brief   Unregister all the receive handlers for the given operation code
             *
             * @param   opId    The operation code that will be unregistered from the receive handlers
             */
            virtual void unregisterReceiveHandlers(polymorph::network::OpId opId) = 0;


        private:
            virtual void _registerReceiveHandler(polymorph::network::OpId opId, std::function<int(const PacketHeader &, const std::vector<std::byte> &)> handler) = 0;
    };
}