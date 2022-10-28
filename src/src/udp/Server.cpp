/*
** EPITECH PROJECT, 2022
** Server.cpp
** File description:
** Server.cpp
*/

#include <iostream>
#include "polymorph/network/udp/Server.hpp"
#include "polymorph/network/dto/ConnectionDto.hpp"
#include "authorizationKey.hpp"
#include "udp/SafePacketManager.hpp"
#include "polymorph/network/dto/ConnectionResponseDto.hpp"
#include "polymorph/network/exceptions/UnauthorizedException.hpp"
#include "polymorph/network/dto/ACKDto.hpp"
#include "polymorph/network/dto/SessionTransferRequestDto.hpp"
#include "polymorph/network/dto/SessionTransferResponseDto.hpp"

polymorph::network::udp::Server::Server(std::uint16_t port, std::map<OpId, bool> safeties, SessionStore &sessionStore)
    : APacketHandler(asio::ip::udp::endpoint(asio::ip::udp::v4(), port)),
        _packetManager(_context, safeties,
            [this](std::shared_ptr<SafePacketManager> manager) {
                _socket.async_send_to(asio::buffer(manager->sPacket), manager->endpoint,
                    [manager](const asio::error_code &error, std::size_t) {
                        if (error) {
                            std::cerr << "Error while sending packet: " << error.message() << std::endl;
                            return;
                        }
                    }
                );
            }
        ),
        _safeties(std::move(safeties)),
        _sessionStore(sessionStore)
{
    _safeties[ConnectionResponseDto::opId] = true;
    _safeties[ACKDto::opId] = false;
    _safeties[SessionTransferResponseDto::opId] = true;
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
                                                 const std::vector<std::byte>&bytes)
{
    if (!_packetManager.hasClient(to)) {
        std::cerr << "Sent packet to unknown client!" << std::endl;
        return;
    }
    _callAndPopSendCallback(to, header, bytes);
    _packetManager.storeOf(to).confirmSent(to, header.pId);
}

void polymorph::network::udp::Server::_onPacketReceived(const asio::ip::udp::endpoint &from,
                                                        const polymorph::network::PacketHeader &header,
                                                        const std::vector<std::byte> &bytes)
{
    if (header.opId == ConnectionDto::opId)
        _handleConnectionHandshake(from, header, bytes);
    else if (header.opId == SessionTransferRequestDto::opId)
        _handleSessionTransfer(from, header, bytes);
    if (_safeties.contains(header.opId) && _safeties.at(header.opId))
        _sendAckPacket(from, header);
}

void polymorph::network::udp::Server::_handleConnectionHandshake(const asio::ip::udp::endpoint &from,
                                                                 const polymorph::network::PacketHeader&,
                                                                 const std::vector<std::byte> &bytes)
{
    auto packet = SerializerTrait<Packet<ConnectionDto>>::deserialize(bytes);

    if (authorizationKey::areSame(packet.payload.authKey, authorizationKey::zero)) {
        auto sId = _sessionStore.registerClient(from);
        if (!_packetManager.registerClient(from)) {
            std::cerr << "Failed to register client!" << std::endl;
            return;
        }
        ConnectionResponseDto response{ .authorized = true};
        sendTo<ConnectionResponseDto>(ConnectionResponseDto::opId, response, sId);
        return;
    }

    try {
        _sessionStore.registerAuthoredClient(from, packet.payload.sessionId, packet.payload.authKey);
        ConnectionResponseDto response{.authorized = false};
        if (!_packetManager.registerClient(from)) {
            _sessionStore.removeClient(from);
        } else
            response.authorized = true;
        sendTo<ConnectionResponseDto>(ConnectionResponseDto::opId, response, packet.payload.sessionId);
    } catch(const exceptions::UnauthorizedException &e) {
        std::cerr << "Error while registering client: " << e.what() << std::endl;
        auto sId = _sessionStore.registerClient(from);
        ConnectionResponseDto response{ .authorized = false};
        sendTo<ConnectionResponseDto>(ConnectionResponseDto::opId, response, sId);
    }
}

void polymorph::network::udp::Server::_sendAckPacket(const asio::ip::udp::endpoint &from,
    const polymorph::network::PacketHeader &header)
{
    ACKDto ack{ .id = header.pId};
    sendTo<ACKDto>(ACKDto::opId, ack, _sessionStore.sessionOf(from));
}

void polymorph::network::udp::Server::_handleSessionTransfer(const asio::ip::udp::endpoint &from,
                                                             const polymorph::network::PacketHeader &header,
                                                             const std::vector<std::byte> &bytes)
{
    auto packet = SerializerTrait<Packet<SessionTransferRequestDto>>::deserialize(bytes);
    auto key = _sessionStore.generateTcpAuthorizationKey(packet.header.sId);
    SessionTransferResponseDto response{ .authKey = key};

    sendTo<SessionTransferResponseDto>(SessionTransferResponseDto::opId, response, packet.header.sId);
}

std::uint16_t polymorph::network::udp::Server::getRunningPort() const
{
    return _socket.local_endpoint().port();
}
