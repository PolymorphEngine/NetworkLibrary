/*
** EPITECH PROJECT, 2022
** ClientImpl.cpp
** File description:
** ClientImpl.cpp
*/


#include <iostream>
#include <future>
#include "udp/ClientImpl.hpp"
#include "polymorph/network/dto/ConnectionDto.hpp"
#include "polymorph/network/dto/ConnectionResponseDto.hpp"
#include "polymorph/network/dto/ACKDto.hpp"
#include "udp/SafePacketManager.hpp"
#include "polymorph/network/dto/SessionTransferRequestDto.hpp"
#include "polymorph/network/dto/DisconnectionDto.hpp"

std::unique_ptr<polymorph::network::udp::Client> polymorph::network::udp::Client::create(std::string host, std::uint16_t port, std::map<OpId, bool> safeties)
{
    return std::make_unique<ClientImpl>(host, port, safeties);
}

polymorph::network::udp::ClientImpl::ClientImpl(std::string host, std::uint16_t port, std::map<OpId, bool> safeties)
    : _serverEndpoint(asio::ip::make_address_v4(host), port),
      APacketHandler(asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)),
      _packetStore(_context, safeties),
      AConnector(APacketHandler::_socket),
      _safeties(std::move(safeties))
{
    _safeties[ConnectionDto::opId] = true;
    _safeties[ACKDto::opId] = false;
    _safeties[SessionTransferRequestDto::opId] = true;
    _safeties[DisconnectionDto::opId] = true;

    _packetStore.setResendCallback([this](std::shared_ptr<SafePacketManager> manager) {
        std::cerr << "Resending lost packet" << std::endl;
        APacketHandler::_socket.async_send_to(asio::buffer(manager->sPacket), manager->endpoint,
            [manager](const asio::error_code &error, std::size_t) {
                if (error) {
                    std::cerr << "Error while sending packet: " << error.message() << std::endl;
                    return;
                }
            }
        );
    });
}

polymorph::network::udp::ClientImpl::~ClientImpl()
{
    if (!_isConnected)
        return;
    DisconnectionDto dto;
    std::promise<void> promise;
    auto future = promise.get_future();

    Client::send<DisconnectionDto>(polymorph::network::DisconnectionDto::opId, dto, [&promise](const PacketHeader &header, const DisconnectionDto &payload) {
        promise.set_value();
    });
    future.wait_for(std::chrono::seconds(1));
    stop();
    if (!_context.stopped())
        _context.stop();
    while (isWriteInProgress() || isReceiveInProgress()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void polymorph::network::udp::ClientImpl::_ackReceived(const asio::ip::udp::endpoint &to,
                                                       const polymorph::network::PacketHeader &header,
                                                       const std::vector<std::byte> &bytes)
{
    auto packet = SerializerTrait<Packet<ACKDto>>::deserialize(bytes);
    _packetStore.confirmReceived(packet.payload.id);
    _callAndPopSendCallback(to, header, bytes);
}

void polymorph::network::udp::ClientImpl::_packetSent(const asio::ip::udp::endpoint& to, const polymorph::network::PacketHeader &header,
                                                  const std::vector<std::byte> &bytes)
{
    _packetStore.confirmSent(to, header.pId);
    if (_safeties.contains(header.opId) && _safeties[header.opId])
        _callAndPopSendCallback(to, header, bytes);
}

void polymorph::network::udp::ClientImpl::connect(std::function<void(bool, SessionId session)> callback)
{
    ConnectionDto dto{ 0, 0};
    Client::send<ConnectionDto>(ConnectionDto::opId, dto);
    _isConnecting = false;

    registerReceiveHandler<ConnectionResponseDto>(ConnectionResponseDto::opId, [this, callback](const PacketHeader &header, const ConnectionResponseDto &payload) {
        _isConnected = payload.authorized;
        _currentSession = header.sId;
        callback(payload.authorized, header.sId);
        return 1;
    });
    startConnection();
    _run();
}

void polymorph::network::udp::ClientImpl::connectWithSession(polymorph::network::SessionId session,
     polymorph::network::AuthorizationKey authKey, std::function<void(bool, SessionId)> callback)
{
    ConnectionDto dto{ .sessionId = session, .authKey = authKey };
    Client::send<ConnectionDto>(ConnectionDto::opId, dto);
    _isConnecting = false;

    registerReceiveHandler<ConnectionResponseDto>(ConnectionResponseDto::opId, [this, callback](const PacketHeader &header, const ConnectionResponseDto &payload) {
        _isConnected = payload.authorized;
        callback(payload.authorized, header.sId);
        return 1;
    });
    startConnection();
    _run();
}

void polymorph::network::udp::ClientImpl::_onPacketReceived(const asio::ip::udp::endpoint &from,
    const polymorph::network::PacketHeader &header, const std::vector<std::byte>&)
{
    if (_safeties.contains(header.opId) && _safeties[header.opId])
        _sendAckPacket(from, header);
}

void polymorph::network::udp::ClientImpl::_sendAckPacket(const asio::ip::udp::endpoint&,
    const polymorph::network::PacketHeader &header)
{
    ACKDto ack{ .id = header.pId};

    Client::send<ACKDto>(ACKDto::opId, ack);
}

void polymorph::network::udp::ClientImpl::_send(polymorph::network::OpId opId, const std::vector<std::byte> &payload,
        std::function<void(const PacketHeader &, const std::vector<std::byte> &)> callback)
{
    if (!_isConnecting && !_isConnected && opId != ACKDto::opId) {
        std::cerr << "Trying to send a packet before ClientImpl is connected" << std::endl;
        return;
    }
    ++_currentPacketId;
    PacketHeader header{};
    header.pId = _currentPacketId;
    header.opId = opId;
    header.sId = _currentSession;
    header.pSize = static_cast<std::uint16_t>(payload.size());
    std::vector<std::byte> sPacket = SerializerTrait<PacketHeader>::serialize(header);
    sPacket.insert(sPacket.end(), payload.begin(), payload.end());
    _packetStore.addToSendList(header, sPacket);
    if (callback)
        _addSendCallback(_serverEndpoint, header.pId, callback);
    AConnector::send(_serverEndpoint, sPacket);
}
