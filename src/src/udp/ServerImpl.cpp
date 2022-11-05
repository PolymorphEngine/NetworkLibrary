/*
** EPITECH PROJECT, 2022
** ServerImpl.cpp
** File description:
** ServerImpl.cpp
*/

#include <iostream>
#include "polymorph/network/udp/ServerImpl.hpp"
#include "polymorph/network/dto/ConnectionDto.hpp"
#include "authorizationKey.hpp"
#include "udp/SafePacketManager.hpp"
#include "polymorph/network/dto/ConnectionResponseDto.hpp"
#include "polymorph/network/exceptions/UnauthorizedException.hpp"
#include "polymorph/network/dto/ACKDto.hpp"
#include "polymorph/network/dto/SessionTransferRequestDto.hpp"
#include "polymorph/network/dto/SessionTransferResponseDto.hpp"

std::unique_ptr<polymorph::network::udp::Server> polymorph::network::udp::Server::create(uint16_t port, std::map<OpId, bool> safeties)
{
    return std::make_unique<ServerImpl>(port, safeties);
}
polymorph::network::udp::ServerImpl::ServerImpl(std::uint16_t port, std::map<OpId, bool> safeties)
    : APacketHandler(asio::ip::udp::endpoint(asio::ip::udp::v4(), port)),
      _packetManager(_context, safeties,
            [this](std::shared_ptr<SafePacketManager> manager) {
                APacketHandler::_socket.async_send_to(asio::buffer(manager->sPacket), manager->endpoint,
                    [manager](const asio::error_code &error, std::size_t) {
                        if (error) {
                            std::cerr << "Error while sending packet: " << error.message() << std::endl;
                            return;
                        }
                    }
                );
            }
        ),
      AConnector(APacketHandler::_socket),
      _safeties(std::move(safeties))
{
    _safeties[ConnectionResponseDto::opId] = true;
    _safeties[ACKDto::opId] = false;
    _safeties[SessionTransferResponseDto::opId] = true;
}

void polymorph::network::udp::ServerImpl::_ackReceived(const asio::ip::udp::endpoint &from,
                                                   polymorph::network::PacketId acknowledgedId)
{
    if (!_packetManager.hasClient(from)) {
        std::cerr << "Received ack from unknown client!" << std::endl;
        return;
    }
    _packetManager.storeOf(from).confirmReceived(acknowledgedId);
}

void polymorph::network::udp::ServerImpl::_packetSent(const asio::ip::udp::endpoint &to,
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

void polymorph::network::udp::ServerImpl::_onPacketReceived(const asio::ip::udp::endpoint &from,
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

void polymorph::network::udp::ServerImpl::_handleConnectionHandshake(const asio::ip::udp::endpoint &from,
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

void polymorph::network::udp::ServerImpl::_sendAckPacket(const asio::ip::udp::endpoint &from,
    const polymorph::network::PacketHeader &header)
{
    ACKDto ack{ .id = header.pId};
    sendTo<ACKDto>(ACKDto::opId, ack, _sessionStore.sessionOf(from));
}

void polymorph::network::udp::ServerImpl::_handleSessionTransfer(const asio::ip::udp::endpoint &from,
                                                             const polymorph::network::PacketHeader &header,
                                                             const std::vector<std::byte> &bytes)
{
    auto packet = SerializerTrait<Packet<SessionTransferRequestDto>>::deserialize(bytes);
    auto key = _sessionStore.generateTcpAuthorizationKey(packet.header.sId);
    SessionTransferResponseDto response{ .authKey = key};

    sendTo<SessionTransferResponseDto>(SessionTransferResponseDto::opId, response, packet.header.sId);
}

std::uint16_t polymorph::network::udp::ServerImpl::getRunningPort() const
{
    return APacketHandler::_socket.local_endpoint().port();
}

void polymorph::network::udp::ServerImpl::start()
{
    startConnection();
    _run();
}

polymorph::network::SessionStore *polymorph::network::udp::ServerImpl::getSessionStore()
{
    return &_sessionStore;
}

void polymorph::network::udp::ServerImpl::setSessionStore(polymorph::network::SessionStore *sessionStore)
{
    _sessionStore = *sessionStore;
}

void polymorph::network::udp::ServerImpl::copyTcpSessionsFrom(polymorph::network::SessionStore *other)
{
    _sessionStore.copyTcpSessionsFrom(*other);
}

void polymorph::network::udp::ServerImpl::copyUdpSessionsFrom(polymorph::network::SessionStore *other)
{
    _sessionStore.copyUdpSessionsFrom(*other);
}

void polymorph::network::udp::ServerImpl::copyTcpAuthorizationKeysFrom(polymorph::network::SessionStore *other)
{
    _sessionStore.copyTcpAuthorizationKeysFrom(*other);
}

void polymorph::network::udp::ServerImpl::copyUdpAuthorizationKeysFrom(polymorph::network::SessionStore *other)
{
    _sessionStore.copyUdpAuthorizationKeysFrom(*other);
}

void polymorph::network::udp::ServerImpl::_sendTo(polymorph::network::OpId opId, const std::vector<std::byte> &data, polymorph::network::SessionId sessionId,
        std::function<void(const PacketHeader &, const std::vector<std::byte> &)> callback)
{
    auto endpoint = _sessionStore.udpEndpointOf(sessionId);
    PacketId id = _packetManager.packetIdOf(endpoint);
    PacketHeader header = {};
    header.pId = id;
    header.opId = opId;
    header.sId = sessionId;
    header.pSize = data.size();
    std::vector<std::byte> sPacket = SerializerTrait<PacketHeader>::serialize(header);
    sPacket.insert(sPacket.end(), data.begin(), data.end());
    _packetManager.storeOf(endpoint).addToSendList(header, sPacket);
    if (callback)
        _addSendCallback(endpoint, header.pId, callback);
    AConnector::send(endpoint, sPacket);
}

void polymorph::network::udp::ServerImpl::_send(polymorph::network::OpId opId, const std::vector<std::byte> &data)
{
    auto sessions = _sessionStore.allUdpSessions();

    for (auto &session : sessions) {
        _sendTo(opId, data, session, std::function<void(const PacketHeader &, const std::vector<std::byte> &)>());
    }
}
