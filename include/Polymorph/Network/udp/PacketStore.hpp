/*
** EPITECH PROJECT, 2020
** PacketStore.hpp
** File description:
** header for PacketStore.c
*/


#pragma once

#include <asio/io_context.hpp>
#include <map>
#include <asio/ip/udp.hpp>

#include "Polymorph/Network/types.hpp"
#include "Polymorph/Network/PacketHeader.hpp"

namespace polymorph::network::udp
{
    class SafePacketManager;

    class PacketStore
    {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            PacketStore(asio::io_context &context, std::map<OpId, bool> safeties, std::function<void(std::vector<std::byte>, asio::ip::udp::endpoint)> resendCallback);


            ~PacketStore() = default;


//////////////////////--------------------------/////////////////////////



///////////////////////////// PROPERTIES ////////////////////////////////
        public:



        private:
            /**
             * @property The list of packets that have been sent and are waiting for an ack
             */
            std::vector<std::shared_ptr<SafePacketManager>> _safeSent;

            /**
             * @property The list of packets waiting to be sent
             */
            std::vector<std::pair<PacketHeader, std::vector<std::byte>>> _toSend;

            /**
             * @property The callback to call when a packet needs to be resent (timeout expired)
             */
            std::function<void(std::vector<std::byte>, asio::ip::udp::endpoint)> _resendCallback;

            /**
             * @property The mapping to know if a packet needs to be safe (have ack) or not
             */
            std::map<OpId, bool> _safeties;

            /**
             * @property The asio context used to create the socket
             */
            asio::io_context &_context;

            /**
             * @property The timeout after which a packet is considered lost
             */
            std::chrono::milliseconds _timeout = DEFAULT_UDP_TIMEOUT;

            /**
             * @property The maximum number of retries before a packet is considered definitely lost
             */
            std::uint8_t _maxRetries = DEFAULT_UDP_RETRIES;

            /**
             * @property Mutex to protect the _safeSent vector
             */
            std::mutex _safeSentMutex;

            /**
             * @property Mutex to protect the _toSend vector
             */
            std::mutex _toSendMutex;


//////////////////////--------------------------/////////////////////////



/////////////////////////////// METHODS /////////////////////////////////
        public:
            /**
             * @brief Add a packet to the list of packets to send
             * @param header The packet to send
             * @param data The serialized version of the packet
             */
            void addToSendList(PacketHeader header, std::vector<std::byte> data);

            /**
             * @brief Remove a packet from the _safeSent list
             * @param id The id of the packet to remove
             */
            void confirmReceived(const PacketId &id);

            /**
             * @brief Remove a packet from the _toSend list and pass it eventually to the _safeSent list if it is safe
             * @param id The id of the packet to remove
             */
            void confirmSent(const asio::ip::udp::endpoint &endpoint, const PacketId &id);

        private:

            /**
             * @brief Add a packet to the _safeSent list
             * @param header The header of the packet
             * @param data The serialized version of the packet
             */
            void _markAsAC(PacketHeader header, std::vector<std::byte> data, const asio::ip::udp::endpoint &endpoint);


//////////////////////--------------------------/////////////////////////

    };
}
