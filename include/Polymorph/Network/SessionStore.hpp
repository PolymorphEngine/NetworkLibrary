/*
** EPITECH PROJECT, 2020
** SessionStore.hpp
** File description:
** header for SessionStore.c
*/


#pragma once

#include <map>
#include <unordered_map>
#include <mutex>
#include "asio/ip/tcp.hpp"
#include "asio/ip/udp.hpp"
#include "Polymorph/Network/types.hpp"

namespace polymorph::network
{

    class SessionStore
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:


//////////////////////--------------------------/////////////////////////



///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            /**
            * @property Mapping between endpoints of udp client and generated sessionId
            */
            std::unordered_map<asio::ip::udp::endpoint, SessionId> _udpSessions;
            /**
             * @property Mapping between endpoints of tcp client and generated sessionId
             */
            std::unordered_map<asio::ip::tcp::endpoint, SessionId> _tcpSessions;

            /**
             * @property Emitted authorization keys to recover sessionId on udp connection
             */
            std::map<SessionId, AuthorizationKey> _udpSessionsAuthorizationKeys;

            /**
             * @property Emitted authorization keys to recover sessionId on tcp connection
             */
            std::map<SessionId, AuthorizationKey> _tcpSessionsAuthorizationKeys;

            /**
             * @property Mutex to lock _sessionsTcp
             */
            std::mutex _tcpSessionsMutex;
            /**
             * @property Mutex to lock _sessionsUdp
             */
            std::mutex _udpSessionsMutex;

            /**
             * @property Mutex to lock _udpSessionsAuthorizationKeys
             */
            std::mutex _udpSessionsAuthorizationKeysMutex;

            /**
             * @property Mutex to lock _tcpSessionsAuthorizationKeys
             */
            std::mutex _tcpSessionsAuthorizationKeysMutex;

//////////////////////--------------------------/////////////////////////



/////////////////////////////// METHODS /////////////////////////////////
        public:
            /**
             * @brief Register a client with a specific sessionId from an endpoint (UDP)
             * @throws UnauthorizedException if the authorization key is not valid
             * @throws UnknownAuthorizationKeyException if the authorization key is unknown
             */
            void registerAuthoredClient(asio::ip::udp::endpoint endpoint, SessionId sessionId, AuthorizationKey key);

            /**
             * @brief Register a client and generate a unique SessionId
             */
            SessionId registerClient(asio::ip::udp::endpoint endpoint);

            /**
             * @brief Register a client with a specific sessionId from an endpoint (TCP)
             * @throws UnauthorizedException if the authorization key is not valid
             * @throws UnknownAuthorizationKeyException if the authorization key is unknown
             */
            void registerAuthoredClient(asio::ip::tcp::endpoint endpoint, SessionId sessionId, AuthorizationKey key);

            /**
             * @brief Register a client and generate a unique SessionId
             */
            SessionId registerClient(asio::ip::tcp::endpoint endpoint);

            /**
             * @brief Remove a client from the store from its endpoint (UDP)
             */
            void removeClient(asio::ip::udp::endpoint endpoint);

            /**
             * @brief Remove a client from the store from its endpoint (TCP)
             */
            void removeClient(asio::ip::tcp::endpoint endpoint);

            /**
             * @brief Remove a client from the store from its sessionId
             */
            bool removeClient(SessionId sessionid);

            /**
             * @brief Generate an authorization key for a specific sessionId to connect with UDP
             * @param sessionId The sessionId to generate the key for
             * @return The generated key
             */
            AuthorizationKey generateUdpAuthorizationKey(SessionId sessionId);

            /**
             * @brief Generate an authorization key for a specific sessionId to connect with TCP
             * @param sessionId The sessionId to generate the key for
             * @return The generated key
             */
            AuthorizationKey generateTcpAuthorizationKey(SessionId sessionId);

            /**
             * @brief Get the sessionId of a client from an endpoint (UDP)
             * @throws UnknownSessionException if the client is not registered
             */
            SessionId sessionOf(asio::ip::udp::endpoint endpoint);

            /**
             * @brief Get the sessionId of a client from an endpoint (TCP)
             * @throws UnknownSessionException if the client is not registered
             */
            SessionId sessionOf(asio::ip::tcp::endpoint endpoint);

            /**
             * @brief Get the endpoint (UDP) of a client from its sessionId
             * @throws UnknownSessionException if the session does not exists
             */
            asio::ip::udp::endpoint udpEndpointOf(SessionId session);

            /**
             * @brief Get all endpoint of all client except the one from the sessionId (UDP)
             */
            std::vector<asio::ip::udp::endpoint> udpEndpointsOfExcept(SessionId except);

            /**
             * @brief Get all sessions of all client (UDP)
             */
            std::vector<SessionId> allUdpSessions();


        private:
            /**
             * @brief find the smallest available sessionId in the tcp sessions
             */
            SessionId _findAvailableTcpSessionId();

            /**
             * @brief find the smallest available sessionId in the udp sessions
             */
            SessionId _findAvailableUdpSessionId();


//////////////////////--------------------------/////////////////////////

    };
}

