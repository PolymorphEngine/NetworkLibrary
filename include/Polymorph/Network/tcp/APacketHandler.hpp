/*
** EPITECH PROJECT, 2020
** APacketHandler.hpp
** File description:
** header for APacketHandler.c
*/

#pragma once

#include <map>
#include <thread>
#include <functional>
#include <asio.hpp>
#include "Polymorph/Network/PacketHeader.hpp"
#include "Polymorph/Network/Packet.hpp"
#include "Polymorph/Network/tcp/IPacketHandler.hpp"
#include "Polymorph/Network/SerializerTrait.hpp"

namespace polymorph::network::tcp
{

    /**
     * @class   Base class for packet handlers
     * @note    TCP exclusive
     *
     * @inherit IPacketHandler   The interface for packet handlers
     */
    class APacketHandler : public IPacketHandler {

    ////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

    public:
        APacketHandler() = default;

        ~APacketHandler() override;


    //////////////////////--------------------------/////////////////////////



    ///////////////////////////// PROPERTIES ////////////////////////////////
    public:


    private:
        /**
         * @property Map of the packet handlers, indexed by their operation code
         *
         * @return  std::map<OpId, std::function<void(const Packet&)>>    the map of the packet handlers
         * @brief   The packet handlers are functions that will be called when a packet with the corresponding operation code is received
         */
        std::map<polymorph::network::OpId,
            std::vector<std::function<bool(const PacketHeader &header, const std::vector<std::byte> &bytes)>>> _receiveCallbacks;
        /**
         * @property Thread that will handle the network operations (asio context)
         */
        std::thread _thread;

    protected:
        /**
         * @property The asio context for the PacketHandler
         */
        asio::io_context _context;


    //////////////////////--------------------------/////////////////////////



    /////////////////////////////// METHODS /////////////////////////////////
    public:
        bool packetReceived(const PacketHeader &, const std::vector<std::byte> &bytes) final;

        /**
         * @brief   Register the receive handler for the given operation code
         *
         * @param   opId    The operation code that will be associated with the receive handler function
         * @param   handler The receive handler function
         */
        template<typename T>
        void registerReceiveHandler(polymorph::network::OpId opId, std::function<bool(const PacketHeader &, const T &)> handler)
        {
            _receiveCallbacks[opId].push_back([handler](const PacketHeader &header, const std::vector<std::byte> &bytes) {
                Packet<T> packet = SerializerTrait<Packet<T>>::deserialize(bytes);
                return handler(header, packet.payload);
            });
        }

        /**
         * @brief   Unregister all the receive handlers for the given operation code
         *
         * @param   opId    The operation code that will be unregistered from the receive handlers
         */
        void unregisterReceiveHandlers(polymorph::network::OpId opId);

    protected:
        /**
         * @brief   Run the asio context in a thread
         */
        virtual void _run() final;


    //////////////////////--------------------------/////////////////////////

    };

}