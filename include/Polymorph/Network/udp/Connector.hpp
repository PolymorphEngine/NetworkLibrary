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
#include "Polymorph/Network/udp/IPacketHandler.hpp"

namespace polymorph::network::udp {

    /**
     * @brief Connector class, used to send and receive packets
     *        It is shared between the client and the server
     */
    class Connector {

////////////////////// CONSTRUCTORS/DESTRUCTORS /////////////////////////

        public:
            /**
             * @brief Construct a new Connector object
             * @param handler The IPacketHandler to use to handle packets
             */
            Connector(IPacketHandler &handler);

            ~Connector() = default;


    //////////////////////--------------------------/////////////////////////



    ///////////////////////////// PROPERTIES ////////////////////////////////
        public:


        private:
            std::queue<std::vector<std::byte>> _sendQueue;
            std::array<std::byte, 1024> _receiveBuffer;
            IPacketHandler &_packetHandler;
            asio::ip::udp::socket &_socket;
            std::atomic<bool> _writeInProgress;


    //////////////////////--------------------------/////////////////////////



    /////////////////////////////// METHODS /////////////////////////////////
        public:
            /**
             * @brief Send a packet to a client
             * @param to The endpoint of the client to send the packet to
             * @param data The packet to send
             */
            void send(asio::ip::udp::endpoint to, const std::vector<std::byte> &data);

            /**
             * @brief Start the receive and run the io_context
             */
            void start();

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
