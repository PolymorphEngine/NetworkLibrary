/*
** EPITECH PROJECT, 2022
** Server.hpp
** File description:
** header for Server.c
*/


#pragma once

#include <map>
#include <memory>

#include "polymorph/network/udp/IPacketHandler.hpp"

namespace polymorph::network { class SessionStore; }

namespace polymorph::network::udp
{

    class Server : virtual public IPacketHandler
    {

            ////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            /**
             * @brief Construct a new Server
             * @param port The port to use by the server
             * @param safeties The safeties map to use to determine if a packet require ack or not
             */
            static std::unique_ptr<Server> create(std::uint16_t port, std::map<OpId, bool> safeties);

            ~Server() override = default;


            //////////////////////--------------------------/////////////////////////



            ///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:


            //////////////////////--------------------------/////////////////////////



            /////////////////////////////// METHODS /////////////////////////////////
        public:
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
                auto serialized = SerializerTrait<T>::serialize(data);
                _sendTo(opId, serialized, sessionId, [callback](const PacketHeader &header, const std::vector<std::byte> &bytes) {
                    if (callback) {
                        auto packet = SerializerTrait<Packet<T>>::deserialize(bytes);
                        callback(header, packet.payload);
                    }
                });
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
                auto serialized = SerializerTrait<T>::serialize(data);
                _send(opId, serialized);
            }


        private:
            virtual void _sendTo(OpId opId, const std::vector<std::byte> &data, SessionId sessionId, std::function<void(const PacketHeader &, const std::vector<std::byte> &)> callback) = 0;

            virtual void _send(OpId opId, const std::vector<std::byte> &data) = 0;

            //////////////////////--------------------------/////////////////////////

    };
}