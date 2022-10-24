/*
** EPITECH PROJECT, 2022
** ServerPacketManager.hpp
** File description:
** header for ServerPacketManager.c
*/

#pragma once

#include <asio/ip/udp.hpp>
#include <map>
#include <asio/steady_timer.hpp>
#include "Polymorph/Network/types.hpp"

namespace polymorph::network::udp
{
    class PacketStore;

    class ServerPacketManager {

        ////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            ServerPacketManager(asio::io_context& io_context, std::map<OpId, bool>& safeties, std::chrono::milliseconds& timeout,
                                uint8_t maxRetries, std::function<void(std::vector<std::byte>, asio::ip::udp::endpoint)> resendCallback)
                    : _io_context(io_context), _safeties(safeties), _timeout(timeout), _maxRetries(maxRetries), _resendCallback(std::move(resendCallback))
            {};

            ~ServerPacketManager() = default;


        //////////////////////--------------------------/////////////////////////



        ///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            std::map<asio::ip::udp::endpoint, std::unique_ptr<PacketStore>> _packetStores;
            std::map<asio::ip::udp::endpoint, PacketId> _currentPacketIds;

            std::mutex _clientsStoresMutex;
            std::mutex _packetIdsMutex;

            // ---

            asio::io_context& _io_context;
            std::map<OpId, bool>& _safeties;
            std::chrono::milliseconds& _timeout;
            std::function<void(std::vector<std::byte>, asio::ip::udp::endpoint)> _resendCallback;

            uint8_t _maxRetries;

            /**
             * @brief The endpoint to which the packet should be sent
             */
            asio::ip::udp::endpoint _endpoint;

        //////////////////////--------------------------/////////////////////////



        /////////////////////////////// METHODS /////////////////////////////////
        public:
            bool hasClient(const asio::ip::udp::endpoint& endpoint);
            bool registerClient(const asio::ip::udp::endpoint& endpoint);
            bool removeClient(const asio::ip::udp::endpoint& endpoint);
            PacketStore& storeOf(const asio::ip::udp::endpoint& endpoint);
            PacketId packetIdOf(const asio::ip::udp::endpoint& endpoint);


        private:


        //////////////////////--------------------------/////////////////////////

    };

}