/*
** EPITECH PROJECT, 2020
** Client.hpp
** File description:
** header for Client.c
*/


#pragma once

#include "APacketHandler.hpp"
#include "Polymorph/Network/udp/PacketStore.hpp"
#include "Polymorph/Network/udp/Connector.hpp"

namespace polymorph::network::udp
{
    class PacketStore;

    class Client : public APacketHandler
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            Client(std::string host, std::uint16_t port, std::map<OpId, bool> safeties);

            ~Client() override = default;


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


//////////////////////--------------------------/////////////////////////



/////////////////////////////// METHODS /////////////////////////////////
        public:
            template<typename T>
            void send(OpId opId, T &payload, std::function<void(const PacketHeader &, const T &)> callback = nullptr)
            {
                if (!_isConnected) {
                    std::cerr << "Trying to send a packet before client is connected" << std::endl;
                    return;
                }
                ++_currentPacketId;
                Packet<T> packet;
                packet.header.pId = _currentPacketId;
                packet.header.opId = opId;
                packet.header.sId = _currentSession;
                packet.payload = payload;
                std::vector<std::byte> sPacket = SerializerTrait<Packet<T>>::serialize(packet);
                _packetStore.addToSendList(packet.header, sPacket);
                if (callback)
                    _addSendCallback(_serverEndpoint, packet.header.pId, callback);
                _connector->send(_serverEndpoint, sPacket);
            }

            void ackReceived(asio::ip::udp::endpoint from, PacketId acknoledgedId) override;

            void
            packetSent(asio::ip::udp::endpoint to, PacketHeader &header, const std::vector<std::byte> &bytes) override;

            void connect(std::function<void(bool, SessionId session)> callback);

            void connectWithSession(SessionId session, AuthorizationKey authKey, std::function<void(bool, SessionId session)> callback);

        protected:
            void _onPacketReceived(const asio::ip::udp::endpoint &from, const PacketHeader &header,
                                   const std::vector<std::byte> &bytes) override;

        private:
            void _sendAckPacket(const asio::ip::udp::endpoint &from, const PacketHeader &header,
                                const std::vector<std::byte> &bytes);

//////////////////////--------------------------/////////////////////////

    };
}
