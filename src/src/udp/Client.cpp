/*
** EPITECH PROJECT, 2022
** Client.cpp
** File description:
** Client.cpp
*/


#include <iostream>
#include "Polymorph/Network/udp/Client.hpp"
#include "Polymorph/Network/dto/ConnectionDto.hpp"
#include "Polymorph/Network/dto/ConnectionResponseDto.hpp"
#include "Polymorph/Network/dto/ACKDto.hpp"
#include "udp/SafePacketManager.hpp"
#include "Polymorph/Network/dto/SessionTransferRequestDto.hpp"


polymorph::network::udp::Client::Client(std::string host, std::uint16_t port, std::map<OpId, bool> safeties)
    : _serverEndpoint(asio::ip::make_address_v4(host), port),
    APacketHandler(asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)),
    _packetStore(_context, safeties,
        [this](std::shared_ptr<SafePacketManager> manager) {
            std::cerr << "Resending lost packet" << std::endl;
            _socket.async_send_to(asio::buffer(manager->sPacket), manager->endpoint,
                    [manager](const asio::error_code &error, std::size_t) {
                        if (error) {
                            std::cerr << "Error while sending packet: " << error.message() << std::endl;
                            return;
                        }
                    }
            );
        }),
    _safeties(std::move(safeties))
{
    _safeties[ConnectionDto::opId] = true;
    _safeties[ACKDto::opId] = false;
    _safeties[SessionTransferRequestDto::opId] = true;
}

void polymorph::network::udp::Client::ackReceived(const asio::ip::udp::endpoint&, polymorph::network::PacketId acknoledgedId)
{
    _packetStore.confirmReceived(acknoledgedId);
}

void polymorph::network::udp::Client::packetSent(const asio::ip::udp::endpoint& to, const polymorph::network::PacketHeader &header,
                                                 const std::vector<std::byte> &bytes)
{
    _packetStore.confirmSent(to, header.pId);
    _callAndPopSendCallback(to, header, bytes);
}

void polymorph::network::udp::Client::connect(std::function<void(bool, SessionId session)> callback)
{
    ConnectionDto dto{ 0, 0};
    send<ConnectionDto>(ConnectionDto::opId, dto);
    _isConnecting = false;

    registerReceiveHandler<ConnectionResponseDto>(ConnectionResponseDto::opId, [this, callback](const PacketHeader &header, const ConnectionResponseDto &payload) {
        _isConnected = payload.authorized;
        callback(payload.authorized, header.sId);
    });
}

void polymorph::network::udp::Client::connectWithSession(polymorph::network::SessionId session,
     polymorph::network::AuthorizationKey authKey, std::function<void(bool, SessionId)> callback)
{
    ConnectionDto dto{ .sessionId = session, .authKey = authKey };
    send<ConnectionDto>(ConnectionDto::opId, dto);
    _isConnecting = false;

    registerReceiveHandler<ConnectionResponseDto>(ConnectionResponseDto::opId, [this, callback](const PacketHeader &header, const ConnectionResponseDto &payload) {
        _isConnected = payload.authorized;
        callback(payload.authorized, header.sId);
    });
}

void polymorph::network::udp::Client::_onPacketReceived(const asio::ip::udp::endpoint &from,
    const polymorph::network::PacketHeader &header, const std::vector<std::byte>&)
{
    if (_safeties.contains(header.opId) && _safeties[header.opId])
        _sendAckPacket(from, header);
}

void polymorph::network::udp::Client::_sendAckPacket(const asio::ip::udp::endpoint&,
    const polymorph::network::PacketHeader &header)
{
    ACKDto ack{ .id = header.pId};

    send<ACKDto>(ACKDto::opId, ack);
}
