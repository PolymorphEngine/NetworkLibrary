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

    class APacketHandler : public IPacketHandler {

    ////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

    public:
        APacketHandler() = default;

        ~APacketHandler() override;


    //////////////////////--------------------------/////////////////////////



    ///////////////////////////// PROPERTIES ////////////////////////////////
    public:


    private:
        std::map<polymorph::network::OpId,
            std::vector<std::function<bool(const PacketHeader &header, const std::vector<std::byte> &bytes)>>> _receiveCallbacks;
        std::thread _thread;

    protected:
        asio::io_context _context;


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

        virtual void start() = 0;

    protected:
        virtual void _run() final;


    //////////////////////--------------------------/////////////////////////

    };

}