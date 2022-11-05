/*
** EPITECH PROJECT, 2020
** Client.hpp
** File description:
** header for Client.c
*/


#pragma once

#include "APacketHandler.hpp"
#include "polymorph/network/udp/PacketStore.hpp"
#include "polymorph/network/udp/AConnector.hpp"
#include "polymorph/network/dto/ACKDto.hpp"

namespace polymorph::network::udp
{
    class PacketStore;

    class Client : virtual public IPacketHandler
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            static std::unique_ptr<Client> create(std::string host, std::uint16_t port, std::map<OpId, bool> safeties);

            ~Client() override = default;


//////////////////////--------------------------/////////////////////////



///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:


//////////////////////--------------------------/////////////////////////



/////////////////////////////// METHODS /////////////////////////////////
        public:
            template<typename T>
            void send(OpId opId, T &payload, std::function<void(const PacketHeader &, const T &)> callback = nullptr)
            {
                auto serialized = SerializerTrait<T>::serialize(payload);
                _send(opId, serialized, [callback](const PacketHeader &header, const std::vector<std::byte> &payload) {
                    if (callback) {
                        auto packet = SerializerTrait<Packet<T>>::deserialize(payload);
                        callback(header, packet.payload);
                    }
                });
            }

            virtual void connect(std::function<void(bool, SessionId session)> callback) = 0;

            virtual void connectWithSession(SessionId session, AuthorizationKey authKey, std::function<void(bool, SessionId session)> callback) = 0;


        private:
            virtual void _send(OpId opId, const std::vector<std::byte> &payload, std::function<void(const PacketHeader &, const std::vector<std::byte> &)> callback) = 0;

//////////////////////--------------------------/////////////////////////

    };
}
