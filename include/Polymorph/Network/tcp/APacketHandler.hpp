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
#include "Polymorph/network/PacketHeader.hpp"
#include "Polymorph/Network/Packet.hpp"
#include "Polymorph/Network/tcp/IPacketHandler.hpp"
#include "Polymorph/Network/SerializerTrait.hpp"

namespace polymorph::network::tcp
{

    class APacketHandler : public IPacketHandler {

    ////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

    public:
        APacketHandler(asio::ip::tcp::endpoint endpoint);

        ~APacketHandler();


    //////////////////////--------------------------/////////////////////////



    ///////////////////////////// PROPERTIES ////////////////////////////////
    public:


    private:
        std::map<polymorph::network::OpId,
            std::vector<std::function<bool(const PacketHeader &header, const std::vector<std::byte> &bytes)>>> _receiveCallbacks;
        std::thread _thread;

    protected:
        asio::io_context _context;
        asio::ip::tcp::socket _socket;


    //////////////////////--------------------------/////////////////////////



    /////////////////////////////// METHODS /////////////////////////////////
    public:
        bool packetReceived(const PacketHeader &, const std::vector<std::byte> &bytes) final;

        template<typename T>
        void registerReceiveHandler(polymorph::network::OpId opId, std::function<bool(const polymorph::network::Packet<T> &)> handler)
        {
            _receiveCallbacks[opId].push_back([handler](const PacketHeader &header, const std::vector<std::byte> &bytes) {
                Packet<T> packet = SerializerTrait<Packet<T>>::deserialize(bytes);
                callback(header, packet.payload);
            });
        }
        void unregisterReceiveHandlers(polymorph::network::OpId opId);

    private:
        virtual void start() = 0;

    protected:
        virtual void _run() final;


    //////////////////////--------------------------/////////////////////////

    };

}