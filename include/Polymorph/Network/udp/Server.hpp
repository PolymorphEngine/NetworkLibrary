/*
** EPITECH PROJECT, 2022
** Server.hpp
** File description:
** header for Server.c
*/


#pragma once

#include "Polymorph/Network/udp/APacketHandler.hpp"
#include "Polymorph/Network/udp/ServerPacketManager.hpp"
#include "Polymorph/Network/udp/PacketStore.hpp"
#include "Polymorph/Network/udp/Connector.hpp"
#include "Polymorph/Network/SessionStore.hpp"

namespace polymorph::network::udp
{
    class Server : public APacketHandler {

        ////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            /**
             * @brief Construct a new Server
             * @param port The port to use by the server
             * @param safeties The safeties map to use to determine if a packet require ack or not
             */
            Server(std::uint16_t port, std::map<OpId, bool> safeties, SessionStore &sessionStore);

            ~Server() override = default;


        //////////////////////--------------------------/////////////////////////



        ///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            /**
             * @property The packet manager used to manage the packets
             */
            ServerPacketManager _packetManager;

            /**
             * @brief The session store used to manage the sessions
             */
            SessionStore &_sessionStore;

            std::map<OpId, bool> _safeties;


        //////////////////////--------------------------/////////////////////////



        /////////////////////////////// METHODS /////////////////////////////////
        public:
        /**
         * @brief get the port used by the server
         * @return The port number
         */
        std::uint16_t getRunningPort() const;

        /**
         * @brief Method called when an ack packet is received, it confirm the reception and discard the auto re-send feature for the packet
         * @param from The recipient of the initial packet
         * @param acknowledgedId The acknowledged packet id
         */
        void ackReceived(const asio::ip::udp::endpoint& from, PacketId acknowledgedId);

        /**
         * @brief Method called when a packet is sent, it confirms the sent and enable the auto re-send feature for safe packets
         * @param to The recipient of the packet
         * @param header the header of the packet
         * @param data The packet in its serialized form
         */
        void packetSent(const asio::ip::udp::endpoint& to, const PacketHeader &header, const std::vector<std::byte> &bytes) override;

        /**
         * @brief Send a packet to the client with the specified session id
         * @tparam T The type of the DTO to transfer
         * @param opId Operation id of the packet to send
         * @param data payload of the packet to send
         * @param sessionId Session id of the recipeint to send the packet to
         * @param callback callback triggered when the packet is fully sent (and surely received if it was safe)
         */
        template<typename T>
        void sendTo(OpId opId, T &data, SessionId sessionId, std::function<void(const PacketHeader &, const T &)> callback = nullptr)
        {
            auto endpoint = _sessionStore.udpEndpointOf(sessionId);
            PacketId id = _packetManager.packetIdOf(endpoint);
            Packet<T> packet = {};
            packet.header.pId = id;
            packet.header.opId = opId;
            packet.header.sId = sessionId;
            packet.payload = data;
            std::vector<std::byte> sPacket = SerializerTrait<Packet<T>>::serialize(packet);
            _packetManager.storeOf(endpoint).addToSendList(packet.header, sPacket);
            if (callback)
                _addSendCallback(endpoint, packet.header.pId, callback);
            _connector->send(endpoint, sPacket);
        }

        /**
         * @brief Send a packet to all clients
         * @tparam T
         * @param opId Operation id of the data T to send
         * @param data The data to send of type T passed by reference
         */
        // TODO: implement callbacks per session AND for all sessions (= when all done)
        template<typename T>
        void send(OpId opId, T &data)
        {
            auto sessions = _sessionStore.allUdpSessions();

            for (auto session: sessions) {
                sendTo(opId, data, session);
            }
        }

            void _onPacketReceived(const asio::ip::udp::endpoint &from, const PacketHeader &header,
                                   const std::vector<std::byte> &bytes) override;


        private:
            void _handleConnectionHandshake(const asio::ip::udp::endpoint &from, const PacketHeader &header,
                                            const std::vector<std::byte> &bytes);

            void _handleSessionTransfer(const asio::ip::udp::endpoint &from, const PacketHeader &header,
                                        const std::vector<std::byte> &bytes);

            void _sendAckPacket(const asio::ip::udp::endpoint &from, const PacketHeader &header);

        //////////////////////--------------------------/////////////////////////

    };
}