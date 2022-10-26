/*
** EPITECH PROJECT, 2020
** Server.hpp
** File description:
** header for Server.c
*/


#pragma once

#include <iostream>
#include "Polymorph/Network/tcp/APacketHandler.hpp"
#include "Polymorph/Network/SessionStore.hpp"
#include "ConnectionPool.hpp"

namespace polymorph::network::tcp
{

    class Server : public APacketHandler
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            Server(std::uint16_t port, SessionStore &sessionStore);

            ~Server() override;


//////////////////////--------------------------/////////////////////////



///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            /**
             * @property The session store to assign session ids to new clients
             */
            SessionStore &_sessionStore;

            ConnectionPool _connectionPool;

            asio::ip::tcp::acceptor _acceptor;


//////////////////////--------------------------/////////////////////////



/////////////////////////////// METHODS /////////////////////////////////
        public:
            void start();

            template<typename T>
            void sendTo(OpId opId, T &data, SessionId sessionId, std::function<void(const PacketHeader &, const T &)> callback = nullptr)
            {
                auto connection = _connectionPool.getConnectionBySessionId(sessionId);

                if (connection == nullptr) {
                    std::cerr << "Trying to send a packet to a non existing session" << std::endl;
                    return;
                }
                auto pId = connection->getPacketId();
                Packet<T> packet = {};
                packet.header.opId = opId;
                packet.header.pId = pId;
                packet.header.sId = sessionId;
                packet.payload = data;
                auto serialized = SerializerTrait<Packet<T>>::serialize(packet);
                connection->send(serialized, [callback](const PacketHeader &header, const std::vector<std::byte> &data) {
                    if (callback != nullptr) {
                        auto deserialized = SerializerTrait<Packet<T>>::deserialize(data);
                        callback(header, deserialized.payload);
                    }
                });
            }

            template<typename T>
            void send(OpId opId, T &data)
            {
                auto connections = _connectionPool.getConnections();

                for (auto &connection : connections) {
                    auto pId = connection->getPacketId();
                    Packet<T> packet = {};
                    packet.header.opId = opId;
                    packet.header.pId = pId;
                    packet.header.sId = connection->getSessionId();
                    packet.payload = data;
                    auto serialized = SerializerTrait<Packet<T>>::serialize(packet);
                    connection->send(serialized, std::function<void (const PacketHeader &, const std::vector<std::byte> &)>());
                }
            }



        private:
            void _doAccept();


//////////////////////--------------------------/////////////////////////

    };
}
