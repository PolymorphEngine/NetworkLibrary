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
#include "polymorph/network/PacketHeader.hpp"
#include "polymorph/network/Packet.hpp"
#include "tcp/IPacketReceiver.hpp"
#include "polymorph/network/SerializerTrait.hpp"
#include "polymorph/network/tcp/IPacketHandler.hpp"

namespace polymorph::network::tcp
{

    /**
     * @class   Base class for packet handlers
     * @note    TCP exclusive
     *
     * @inherit IPacketHandler   The interface for packet handlers
     */
    class APacketHandler : public IPacketReceiver, virtual public IPacketHandler {

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
            std::vector<std::shared_ptr<std::function<int(const PacketHeader &header, const std::vector<std::byte> &bytes)>>>> _receiveCallbacks;
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

        void unregisterReceiveHandlers(polymorph::network::OpId opId) override;

    protected:
        /**
         * @brief   Run the asio context in a thread
         */
        virtual void _run() final;


    private:
        void _registerReceiveHandler(polymorph::network::OpId opId, std::function<int(const PacketHeader &, const std::vector<std::byte> &)> handler) override;




            //////////////////////--------------------------/////////////////////////

    };

}