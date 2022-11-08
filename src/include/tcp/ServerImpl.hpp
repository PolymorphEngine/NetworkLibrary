/*
** EPITECH PROJECT, 2020
** ServerImpl.hpp
** File description:
** header for ServerImpl.c
*/


#pragma once

#include <iostream>
#include "tcp/PacketHandler.hpp"
#include "SessionStore.hpp"
#include "ConnectionPool.hpp"
#include "polymorph/network/tcp/Server.hpp"

namespace polymorph::network::tcp
{

    /**
     * @class   TCP ServerImpl class
     * @note    TCP exclusive
     */
    class ServerImpl : public Server, virtual public APacketHandler
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            ServerImpl(std::uint16_t port);

            ~ServerImpl() override;


//////////////////////--------------------------/////////////////////////



///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            /**
             * @property    The session store to assign session ids to new clients
             */
            SessionStore _sessionStore;

            /**
             * @property    Connection Pool, responsible of the good behaviour of TCP sockets
             */
            std::shared_ptr<ConnectionPool> _connectionPool;

            /**
             * @property    Asio TCP acceptor.
             */
            asio::ip::tcp::acceptor _acceptor;

            std::atomic<int> _sendingCount = 0;

            std::atomic<int> _receivingCount = 0;


//////////////////////--------------------------/////////////////////////



/////////////////////////////// METHODS /////////////////////////////////
        public:
            void start() override;

            uint16_t getRunningPort() const override;

            SessionStore *getSessionStore() override;

            void setSessionStore(SessionStore *sessionStore) override;

            void copyTcpSessionsFrom(SessionStore *other) override;

            void copyUdpSessionsFrom(SessionStore *other) override;

            void copyTcpAuthorizationKeysFrom(SessionStore *other) override;

            void copyUdpAuthorizationKeysFrom(SessionStore *other) override;

            AuthorizationKey generateUdpAuthorizationKey(SessionId sessionId) override;

            AuthorizationKey generateTcpAuthorizationKey(SessionId sessionId) override;

            void declareSending();

            void declareSendingDone();

            bool isSending();

            void declareReceiving();

            void declareReceivingDone();

            bool isReceiving();



        private:
            /**
             * @brief Accept clients to connect to the ServerImpl
             */
            void _doAccept();

            void _sendTo(OpId opId, const std::vector<std::byte> &data, SessionId sessionId, std::function<void(const PacketHeader &, const std::vector<std::byte> &)> callback) override;

            void _send(OpId opId, const std::vector<std::byte> &data) override;


//////////////////////--------------------------/////////////////////////

    };
}
