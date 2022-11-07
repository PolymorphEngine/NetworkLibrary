/*
** EPITECH PROJECT, 2020
** Connector.hpp
** File description:
** header for Connector.c
*/

#pragma once

#include <asio/ip/udp.hpp>
#include <asio/io_context.hpp>
#include <queue>
#include <vector>
#include "udp/IPacketReceiver.hpp"

namespace polymorph::network::udp {

    /**
     * @brief Connector class, used to send and receive packets
     *        It is shared between the client and the server
     */
    class AConnector : virtual public IPacketReceiver {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            /**
             * @brief Construct a new Connector object
             * @param handler The IPacketHandler to use to handle packets
             */
            AConnector(asio::ip::udp::socket &socket);

            ~AConnector() override = default;


    //////////////////////--------------------------/////////////////////////



    ///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            /**
             * @property The vector containing the data to be sent with it's corresponding endpoint
             */
            std::queue<std::pair<asio::ip::udp::endpoint, std::vector<std::byte>>> _sendQueue;

            /**
             * @property An array containing the data received that need to be processed
             */
            std::array<std::byte, 1024> _receiveBuffer;

            /**
             * @property The socket to use to send and receive packets
             */
            asio::ip::udp::socket &_socket;

            /**
             * @property Atomic boolean to know if we are currently busy sending a packet
             */
            std::atomic<bool> _writeInProgress;

            std::atomic<bool> _receiveInProgress;

            std::mutex _sendQueueMutex;

            asio::ip::udp::endpoint _endpoint;


    //////////////////////--------------------------/////////////////////////



    /////////////////////////////// METHODS /////////////////////////////////
        public:
            /**
             * @brief Send a packet to a client
             * @param to The endpoint of the client to send the packet to
             * @param data The packet to send
             */
            void send(const asio::ip::udp::endpoint& to, const std::vector<std::byte> &data);

            /**
             * @brief Start the receive and run the io_context
             */
            void startConnection();

            bool isWriteInProgress() const;

            bool isReceiveInProgress() const;

        private:
            /**
             * @brief Start a receive operation
             */
            void _doReceive();

            /**
             * @brief Send all the packets in the send queue
             */
            void _doSend();

            /**
             * @brief Determine the type of the packet (require ACK or not) and call the right method
             * @param data The packet data
             */
            void _determinePacket(const std::vector<std::byte> &data);



    //////////////////////--------------------------/////////////////////////

    };

} // polymorph::network::udp
