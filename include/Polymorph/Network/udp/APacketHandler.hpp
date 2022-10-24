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
#include "Polymorph/Network/udp/IPacketHandler.hpp"
#include "Polymorph/Network/Packet.hpp"
#include "Polymorph/Network/SerializerTrait.hpp"

namespace polymorph::network::udp
{
    class Connector;

    class APacketHandler : public IPacketHandler
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            APacketHandler(asio::ip::udp::endpoint endpoint);

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

            /**
             * @property The link to a Connector instance to send packets
             */
            std::shared_ptr<Connector> _connector;

        private:
            /**
             * @property The thread used to run the io_context
             */
            std::thread _thread;

            /**
             * @property The callbacks to call when a packet is received
             */
            std::map<OpId, std::vector<std::function<void(const PacketHeader &, const std::vector<std::byte> &)>>> _receiveCallbacks;

            /**
             * @property The callbacks to call when a packet is sent
             */
            std::map<std::pair<asio::ip::udp::endpoint, PacketId>, std::function<void(const PacketHeader &, const std::vector<std::byte> &)>> _sentCallbacks;



//////////////////////--------------------------/////////////////////////



/////////////////////////////// METHODS /////////////////////////////////
        public:
            /**
             * @copydetails IPacketHandler::packetReceived
             */
            void packetReceived(asio::ip::udp::endpoint from, const std::vector<std::byte> &bytes) override final;

            /**
             * @copydetails IPacketHandler::getPreparedSocket
             */
            asio::ip::udp::socket &getPreparedSocket() override final;

            /**
             * @copydetails IPacketHandler::start
             */
            void start() override final;

            /**
             * @brief Add a callback to call when a packet is received with the passed opId
             * @tparam T The type of the packet to handle
             * @param opId The opId of the packet to handle
             * @param callback The callback to call when a packet is received
             */
            template<typename T>
            void registerReceiveHandler(OpId opId, std::function<void(const PacketHeader &, const T &)> callback)
            {
                _receiveCallbacks[opId].push_back([callback](const PacketHeader &header, const std::vector<std::byte> &bytes) {
                    Packet<T> packet = SerializerTrait<Packet<T>>::deserialize(bytes);
                    callback(header, packet.payload);
                });
            }

            /**
             * @brief Remove all callbacks registered for the passed opId
             * @param opId The opId of the packet to remove the callbacks from
             */
            void unregisterReceiveHandlers(OpId opId);

            /**
             * @brief Set the _connector property
             * @param connector The connector to link to
             */
            void setConnector(std::shared_ptr<Connector> connector);

        protected:
            /**
             * @brief add a callback to call when a packet with a specific packet id is sent
             */
            void _addSendCallback(const asio::ip::udp::endpoint& to, PacketId id, std::function<void(const PacketHeader &, const std::vector<std::byte> &)> callback);

            /**
             * @brief call a callback registered for the packet id and remove it
             */
            void _callAndPopSendCallback(asio::ip::udp::endpoint to, const PacketHeader &header, const std::vector<std::byte> &bytes);

        private:
            /**
             * @brief call all registered receive callbacks for the packet
             * @param header The header of the packet
             * @param bytes The packet in its serialized form
             */
            void _broadcastReceivedPacket(const PacketHeader &header, const std::vector<std::byte> &bytes);



//////////////////////--------------------------/////////////////////////

    };

}
