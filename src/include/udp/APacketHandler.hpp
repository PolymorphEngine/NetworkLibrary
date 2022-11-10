/*
** EPITECH PROJECT, 2020
** APacketHandler.hpp
** File description:
** header for APacketHandler.c
*/


#pragma once

#include <map>
#include <functional>
#include <asio/ip/udp.hpp>
#include <thread>
#include <utility>
#include "udp/IPacketReceiver.hpp"
#include "polymorph/network/Packet.hpp"
#include "polymorph/network/SerializerTrait.hpp"
#include "polymorph/network/udp/IPacketHandler.hpp"

namespace polymorph::network::udp
{
    class AConnector;

    class APacketHandler : virtual public IPacketReceiver, virtual public IPacketHandler
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            explicit APacketHandler(asio::ip::udp::endpoint endpoint);

            ~APacketHandler() override;


//////////////////////--------------------------/////////////////////////



///////////////////////////// PROPERTIES ////////////////////////////////
        protected:
            /**
             * @property The asio context used to create the socket
             */
            asio::io_context _context;

            /**
             * @property The socket to use to send and receive packets
             */
            asio::ip::udp::socket _socket;

        private:
            /**
             * @property The thread used to run the io_context
             */
            std::thread _thread;

            /**
             * @property The callbacks to call when a packet is received
             */
            std::map<OpId, std::vector<std::shared_ptr<std::function<int(const PacketHeader &, const std::vector<std::byte> &)>>>> _receiveCallbacks;

            /**
             * @property The callbacks to call when a packet is sent
             */
            std::map<std::pair<asio::ip::udp::endpoint, PacketId>, std::function<void(const PacketHeader &, const std::vector<std::byte> &)>> _sentCallbacks;



//////////////////////--------------------------/////////////////////////



/////////////////////////////// METHODS /////////////////////////////////
        public:
            /**
             * @copydetails IPacketHandler::_packetReceived
             */
            void _packetReceived(const asio::ip::udp::endpoint& from, const std::vector<std::byte> &bytes) final;

            /**
             * @brief Remove all callbacks registered for the passed opId
             * @param opId The opId of the packet to remove the callbacks from
             */
            void unregisterReceiveHandlers(OpId opId) final ;

            void _run();

        protected:
            virtual void _onPacketReceived(const asio::ip::udp::endpoint &from, const PacketHeader &header, const std::vector<std::byte> &bytes) = 0;

            /**
             * @brief add a callback to call when a packet with a specific packet id is sent
             */
            void _addSendCallback(const asio::ip::udp::endpoint& to, PacketId id, std::function<void(const PacketHeader &, const std::vector<std::byte> &)> callback)
            {
                _sentCallbacks[std::make_pair(to, id)] = std::move(callback);
            };

            /**
             * @brief call a callback registered for the packet id and remove it
             */
            void _callAndPopSendCallback(const asio::ip::udp::endpoint& to, const PacketHeader &header, const std::vector<std::byte> &bytes);

        private:
            /**
             * @brief call all registered receive callbacks for the packet
             * @param header The header of the packet
             * @param bytes The packet in its serialized form
             */
            void _broadcastReceivedPacket(const PacketHeader &header, const std::vector<std::byte> &bytes);

            void _registerReceiveHandler(polymorph::network::OpId opId, std::function<int(const PacketHeader &, const std::vector<std::byte> &)> handler) override;



//////////////////////--------------------------/////////////////////////

    };

}
