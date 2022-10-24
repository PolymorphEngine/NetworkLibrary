/*
** EPITECH PROJECT, 2022
** Server.cpp
** File description:
** Server.cpp
*/

#include <iostream>
#include "Polymorph/Network/udp/Server.hpp"
#include "Polymorph/Network/dto/ConnectionDto.hpp"
#include "authorizationKey.hpp"
#include "Polymorph/Network/dto/ConnectionResponseDto.hpp"
#include "Polymorph/Network/exceptions/UnauthorizedException.hpp"
#include "Polymorph/Network/dto/ACKDto.hpp"

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
            }),
            _safeties(std::move(safeties))
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

void polymorph::network::udp::Server::_onPacketReceived(const asio::ip::udp::endpoint &from,
                                                        const polymorph::network::PacketHeader &header,
                                                        const std::vector<std::byte> &bytes)
{
    if (header.opId != ConnectionDto::opId)
        _handleConnectionHandshake(from, header, bytes);
    if (_safeties.contains(header.opId) && _safeties.at(header.opId))
        _sendAckPacket(from, header, bytes);
}

void polymorph::network::udp::Server::_handleConnectionHandshake(const asio::ip::udp::endpoint &from,
                                                                 const polymorph::network::PacketHeader &header,
                                                                 const std::vector<std::byte> &bytes)
{
    auto packet = SerializerTrait<Packet<ConnectionDto>>::deserialize(bytes);

    if (authorizationKey::areSame(packet.payload.authKey, authorizationKey::zero)) {
        auto sId = _sessionStore.registerClient(from);
        ConnectionResponseDto response{ .authorized = true};
        sendTo<ConnectionResponseDto>(ConnectionResponseDto::opId, response, sId);
        return;
    }

    try {
        _sessionStore.registerAuthoredClient(from, packet.payload.sessionId, packet.payload.authKey);
        ConnectionResponseDto response{ .authorized = true};
        sendTo<ConnectionResponseDto>(ConnectionResponseDto::opId, response, packet.payload.sessionId);
    } catch(const exceptions::UnauthorizedException &e) {
        std::cerr << "Error while registering client: " << e.what() << std::endl;
        auto sId = _sessionStore.registerClient(from);
        ConnectionResponseDto response{ .authorized = false};
        sendTo<ConnectionResponseDto>(ConnectionResponseDto::opId, response, sId);
    }
}

void polymorph::network::udp::Server::_sendAckPacket(const asio::ip::udp::endpoint &from,
    const polymorph::network::PacketHeader &header, const std::vector<std::byte> &bytes)
{
    ACKDto ack{ .id = header.pId};
    sendTo<ACKDto>(ACKDto::opId, ack, _sessionStore.sessionOf(from));
}
