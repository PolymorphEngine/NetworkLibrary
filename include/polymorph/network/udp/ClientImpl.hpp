/*
** EPITECH PROJECT, 2020
** ClientImpl.hpp
** File description:
** header for ClientImpl.c
*/


#pragma once

#include "APacketHandler.hpp"
#include "polymorph/network/udp/PacketStore.hpp"
#include "polymorph/network/udp/AConnector.hpp"
#include "polymorph/network/dto/ACKDto.hpp"
#include "polymorph/network/udp/Client.hpp"

namespace polymorph::network::udp
{
    class PacketStore;

    class ClientImpl : virtual public Client, virtual public APacketHandler, public AConnector
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            ClientImpl(std::string host, std::uint16_t port, std::map<OpId, bool> safeties);

            ~ClientImpl() override = default;


//////////////////////--------------------------/////////////////////////



///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            asio::ip::udp::endpoint _serverEndpoint;
            PacketStore _packetStore;
            PacketId _currentPacketId = 0;
            SessionId _currentSession = 0;
            std::map<OpId, bool> _safeties;
            std::atomic<bool> _isConnected = false;
            std::atomic<bool> _isConnecting = true;


//////////////////////--------------------------/////////////////////////



/////////////////////////////// METHODS /////////////////////////////////
        public:
            void _ackReceived(const asio::ip::udp::endpoint& from, PacketId acknoledgedId) override;

            void _packetSent(const asio::ip::udp::endpoint& to, const PacketHeader &header, const std::vector<std::byte> &bytes) override;

            void connect(std::function<void(bool, SessionId session)> callback) override;

            void connectWithSession(SessionId session, AuthorizationKey authKey, std::function<void(bool, SessionId session)> callback) override;

        protected:
            void _onPacketReceived(const asio::ip::udp::endpoint &from, const PacketHeader &header, const std::vector<std::byte> &bytes) override;

        private:
            void _sendAckPacket(const asio::ip::udp::endpoint &from, const PacketHeader &header);

            void _send(OpId opId, const std::vector<std::byte> &payload, std::function<void(const PacketHeader &, const std::vector<std::byte> &)> callback) override;

//////////////////////--------------------------/////////////////////////

    };
}
