/*
** EPITECH PROJECT, 2020
** Server.hpp
** File description:
** header for Server.c
*/


#pragma once

#include <iostream>
#include <memory>
#include "polymorph/network/tcp/IPacketHandler.hpp"

namespace polymorph::network { class SessionStore; }

namespace polymorph::network::tcp
{

    /**
     * @class   TCP Server class
     * @note    TCP exclusive
     */
    class Server : virtual public IPacketHandler
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            static std::unique_ptr<Server> create(std::uint16_t port);

            ~Server() override = default;


//////////////////////--------------------------/////////////////////////



///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:


//////////////////////--------------------------/////////////////////////



/////////////////////////////// METHODS /////////////////////////////////
        public:
            /**
             * @brief Accept and start the network context
             */
            virtual void start() = 0;

            /**
             * @brief get the port used by the server
             * @return The port number
             */
            virtual std::uint16_t getRunningPort() const = 0;

            virtual SessionStore *getSessionStore() = 0;

            virtual void setSessionStore(SessionStore *sessionStore) = 0;

            virtual void copyTcpSessionsFrom(SessionStore *other) = 0;

            virtual void copyUdpSessionsFrom(SessionStore *other) = 0;

            virtual void copyTcpAuthorizationKeysFrom(SessionStore *other) = 0;

            virtual void copyUdpAuthorizationKeysFrom(SessionStore *other) = 0;

            /**
             * @brief Generate an authorization key for a specific sessionId to connect with UDP
             * @param sessionId The sessionId to generate the key for
             * @return The generated key
             */
            virtual AuthorizationKey generateUdpAuthorizationKey(SessionId sessionId) = 0;

            /**
             * @brief Generate an authorization key for a specific sessionId to connect with TCP
             * @param sessionId The sessionId to generate the key for
             * @return The generated key
             */
            virtual AuthorizationKey generateTcpAuthorizationKey(SessionId sessionId) = 0;

            /**
             * @brief   Send a payload to the server
             *
             * @tparam  T           The type of the payload to pack
             * @param   opId        The operation id of the Packet
             * @param   payload     The actual Packet data
             * @param   sessionId   The destination SessionId for the packet
             * @param   callback    Callback function used when the data has been (or not) delivered
             */
            template<typename T>
            void sendTo(OpId opId, T &data, SessionId sessionId, std::function<void(const PacketHeader &, const T &)> callback = nullptr)
            {
                auto serialized = SerializerTrait<T>::serialize(data);
                _sendTo(opId, serialized, sessionId, [callback](const PacketHeader &header, const std::vector<std::byte> &bytes) {
                    if (callback) {
                        T data = SerializerTrait<T>::deserialize(bytes);
                        callback(header, data);
                    }
                });
            }

            /**
            * @brief   Send a payload to the server
            *
            * @tparam  T           The type of the payload to pack
            * @param   opId        The operation id of the Packet
            * @param   payload     The actual Packet data
            */
            template<typename T>
            void send(OpId opId, T &data)
            {
                auto serialized = SerializerTrait<T>::serialize(data);
                _send(opId, serialized);
            }



        private:

            virtual void _sendTo(OpId opId, const std::vector<std::byte> &data, SessionId sessionId, std::function<void(const PacketHeader &header, const std::vector<std::byte> &data)> callback) = 0;

            virtual void _send(OpId opId, const std::vector<std::byte> &data) = 0;


//////////////////////--------------------------/////////////////////////

    };
}
