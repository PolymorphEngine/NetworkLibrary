/*
** EPITECH PROJECT, 2022
** Server.cpp
** File description:
** Server.cpp
*/

#include <iostream>
#include "Polymorph/Network/udp/Server.hpp"

polymorph::network::udp::Server::Server(std::uint16_t port, std::map<OpId, bool> safeties)
    : APacketHandler(asio::ip::udp::endpoint(asio::ip::udp::v4(), port)),
        _packetManager(_context, safeties,
            [this](const std::vector<std::byte> &sPacket, const asio::ip::udp::endpoint &recipient) {
                _socket.async_send_to(asio::buffer(sPacket), recipient,
                    [recipient](const asio::error_code &error, std::size_t bytes_transferred) {
                        if (error) {
                            std::cerr << "Error while sending packet: " << error.message() << std::endl;
                            return;
                        }
                    });
            })
{

}

void polymorph::network::udp::Server::ackReceived(const asio::ip::udp::endpoint &from,
                                                  polymorph::network::PacketId acknowledgedId)
{
    if (!_packetManager.hasClient(from)) {
        std::cerr << "Received ack from unknown client!" << std::endl;
        return;
    }
    _packetManager.storeOf(from).confirmReceived(acknowledgedId);
}

void polymorph::network::udp::Server::packetSent(const asio::ip::udp::endpoint &to,
                                                 const polymorph::network::PacketHeader &header,
                                                 const std::vector<std::byte> &data)
{
    if (!_packetManager.hasClient(to)) {
        std::cerr << "Sent packet to unknown client!" << std::endl;
        return;
    }
    _packetManager.storeOf(to).confirmSent(to, header.pId);
}
