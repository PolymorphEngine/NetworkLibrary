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
#include <utility>
#include "Polymorph/Network/types.hpp"

namespace polymorph::network::udp
{
    class PacketStore;
    class SafePacketManager;

    class ServerPacketManager {

        ////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            ServerPacketManager(asio::io_context& io_context, std::map<OpId, bool> safeties, std::function<void(std::shared_ptr<SafePacketManager>)> resendCallback)
                    : _io_context(io_context), _safeties(std::move(safeties)), _resendCallback(std::move(resendCallback))
            {};

            ~ServerPacketManager() = default;


        //////////////////////--------------------------/////////////////////////



        ///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            /**
             * @property Map of all the packets that are waiting for an operation to be completed for a specific endpoint
             */
            std::map<asio::ip::udp::endpoint, std::unique_ptr<PacketStore>> _packetStores;

            /**
             * @property Map of endpoint and their current packet id
             */
            std::map<asio::ip::udp::endpoint, PacketId> _currentPacketIds;

            /**
             * @property Mutex used to protect the _packetStores map
             */
            std::mutex _clientsStoresMutex;

            /**
             * @property Mutex used to protect the _currentPacketIds map
             */
            std::mutex _packetIdsMutex;

            /**
             * @property The io_context used to create the timer and sockets
             */
            asio::io_context& _io_context;

            /**
             * @property Map of OpId to bool, if the bool is true, the packet will be sent again if not acknowledged
             */
            std::map<OpId, bool> _safeties;

            /**
             * @property Callback called when a packet needs to be resent
             * @param data The data of the packet
             * @param endpoint The endpoint to which the packet needs to be resent
             */
            std::function<void(std::shared_ptr<SafePacketManager>)> _resendCallback;

            /**
             * @property The endpoint to which the packet should be sent
             */
            asio::ip::udp::endpoint _endpoint;

        //////////////////////--------------------------/////////////////////////



        /////////////////////////////// METHODS /////////////////////////////////
        public:
            /**
             * @brief Check if the client is already registered
             * @param endpoint The endpoint of the client to check
             * @return true if the client is registered, false otherwise
             */
            bool hasClient(const asio::ip::udp::endpoint& endpoint);

            /**
             * @brief Register a new client
             * @param endpoint The endpoint of the client to register
             * @return true if the client has been registered, false otherwise
             */
            bool registerClient(const asio::ip::udp::endpoint& endpoint);

            /**
             * @brief Remove a client
             * @param endpoint The endpoint of the client to remove
             * @return true if the client has been removed, false otherwise
             */
            bool removeClient(const asio::ip::udp::endpoint& endpoint);

            /**
             * @brief Get the PacketStore of a client
             * @param endpoint The endpoint of the client
             * @return The PacketStore of the client
             * @throw std::out_of_range if the client is not registered
             */
            PacketStore& storeOf(const asio::ip::udp::endpoint& endpoint);

            /**
             * @brief Get the current packet id of a client
             * @param endpoint The endpoint of the client
             * @return The current packet id of the client
             * @throw std::out_of_range if the client is not registered
             */
            PacketId packetIdOf(const asio::ip::udp::endpoint& endpoint);


        private:


        //////////////////////--------------------------/////////////////////////

    };

}