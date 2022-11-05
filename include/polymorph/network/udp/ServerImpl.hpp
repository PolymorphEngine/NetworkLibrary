/*
** EPITECH PROJECT, 2022
** ServerImpl.hpp
** File description:
** header for ServerImpl.c
*/


#pragma once

#include "polymorph/network/udp/APacketHandler.hpp"
#include "polymorph/network/udp/ServerPacketManager.hpp"
#include "polymorph/network/udp/PacketStore.hpp"
#include "polymorph/network/udp/AConnector.hpp"
#include "polymorph/network/SessionStore.hpp"
#include "Server.hpp"

namespace polymorph::network::udp
{
    class ServerImpl : public Server, virtual public APacketHandler, virtual public AConnector
    {

            ////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            /**
             * @brief Construct a new ServerImpl
             * @param port The port to use by the ServerImpl
             * @param safeties The safeties map to use to determine if a packet require ack or not
             */
            ServerImpl(std::uint16_t port, std::map<OpId, bool> safeties);

            ~ServerImpl() override = default;


            //////////////////////--------------------------/////////////////////////



            ///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            /**
             * @property The packet manager used to manage the packets
             */
            ServerPacketManager _packetManager;

            /**
             * @property The session store used to manage the sessions
             */
            SessionStore _sessionStore;

            /**
             * @property The safeties map used to determine if a packet require ack or not
             */
            std::map<OpId, bool> _safeties;


            //////////////////////--------------------------/////////////////////////



            /////////////////////////////// METHODS /////////////////////////////////
        public:
            void start() override;

            /**
             * @brief get the port used by the ServerImpl
             * @return The port number
             */
            std::uint16_t getRunningPort() const;

            SessionStore *getSessionStore() override;

            void setSessionStore(SessionStore *sessionStore) override;

            void copyTcpSessionsFrom(SessionStore *other) override;

            void copyUdpSessionsFrom(SessionStore *other) override;

            void copyTcpAuthorizationKeysFrom(SessionStore *other) override;

            void copyUdpAuthorizationKeysFrom(SessionStore *other) override;

        private:
            void _sendTo(OpId opId, const std::vector<std::byte> &data, SessionId sessionId,
                         std::function<void(const PacketHeader &, const std::vector<std::byte> &)> callback) override;

            void _send(OpId opId, const std::vector<std::byte> &data) override;

        public:

            /**
             * @brief Method called when an ack packet is received, it confirm the reception and discard the auto re-send feature for the packet
             * @param from The recipient of the initial packet
             * @param acknowledgedId The acknowledged packet id
             */
            void _ackReceived(const asio::ip::udp::endpoint &from, PacketId acknowledgedId) override;

            /**
             * @brief Method called when a packet is sent, it confirms the sent and enable the auto re-send feature for safe packets
             * @param to The recipient of the packet
             * @param header the header of the packet
             * @param data The packet in its serialized form
             */
            void _packetSent(const asio::ip::udp::endpoint &to, const PacketHeader &header,
                             const std::vector<std::byte> &bytes) override;

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