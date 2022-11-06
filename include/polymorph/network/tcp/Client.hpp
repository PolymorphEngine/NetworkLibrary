/*
** EPITECH PROJECT, 2020
** Client.hpp
** File description:
** header for Client.c
*/


#pragma once

#include <functional>
#include "polymorph/network/Packet.hpp"
#include "polymorph/network/SerializerTrait.hpp"
#include "IPacketHandler.hpp"

namespace polymorph::network::tcp
{

    /**
     * @class   TCP Client class
     * @note    TCP exclusive
     */
    class Client : virtual public IPacketHandler
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            /**
             * @brief   Constructor of the TCP Client class
             *
             * @param   host    The server host ip address
             * @param   port    The server port
             */
            static std::unique_ptr<Client> create(std::string host, std::uint16_t port);

            ~Client() override = default;


//////////////////////--------------------------/////////////////////////



///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:



//////////////////////--------------------------/////////////////////////

/////////////////////////////// METHODS /////////////////////////////////
        public:
            /**
             * @brief   Send a payload to the server
             *
             * @tparam  T           The type of the payload to pack
             * @param   opId        The operation id of the Packet
             * @param   payload     The actual Packet data
             * @param   callback    Callback function used when the data has been (or not) delivered
             */
            template<typename T>
            void send(OpId opId, T &payload, std::function<void(const PacketHeader &, const T &)> callback = nullptr)
            {
                auto bytes = SerializerTrait<T>::serialize(payload);

                _send(opId, bytes, [callback](const PacketHeader &header, const std::vector<std::byte> &bytes) {
                    if (callback) {
                        Packet<T> packet = SerializerTrait<Packet<T>>::deserialize(bytes);
                        callback(header, packet.payload);
                    }
                });
            }

            /**
             * @brief   Connect to server and execute a callback when done
             * @param   callback    Callback executed when the connection is done (or not)
             * @note    bool of std::function correspond to the auth status (true = success, false otherwise)
             */
            virtual void connect(std::function<void(bool, SessionId)> callback) = 0;

            /**
             * @brief   Connect from a SessionId
             * @note    This method is used to communicate with the server with a authenticated session
             */
            virtual void connectWithSession(SessionId session, AuthorizationKey authKey, std::function<void(bool, SessionId)> callback) = 0;


        private:
            virtual void _send(OpId opId, const std::vector<std::byte> &data, std::function<void(const PacketHeader &, const std::vector<std::byte> &)> callback) = 0;

//////////////////////--------------------------/////////////////////////

    };
}