/*
** EPITECH PROJECT, 2022
** SessionStore.cpp
** File description:
** SessionStore.cpp
*/


#include "Polymorph/Network/SessionStore.hpp"
#include "authorizationKey.hpp"
#include "Polymorph/Network/exceptions/UnknownAuthorizationKeyException.hpp"
#include "Polymorph/Network/exceptions/UnauthorizedException.hpp"
#include "Polymorph/Network/exceptions/AlreadyRegisteredException.hpp"
#include "Polymorph/Network/exceptions/UnknownSessionException.hpp"

void polymorph::network::SessionStore::registerAuthoredClient(asio::ip::udp::endpoint endpoint,
        polymorph::network::SessionId sessionId, polymorph::network::AuthorizationKey key)
{
    try {
        std::unique_lock<std::mutex> lock(_udpSessionsAuthorizationKeysMutex);
        auto verifyKey = _udpSessionsAuthorizationKeys.at(sessionId);
        lock.unlock();
        if (authorizationKey::areSame(verifyKey, key)) {
            _udpSessions[endpoint] = sessionId;
        } else {
            throw exceptions::UnauthorizedException("InvalidAuthorization key");
        }
    } catch (std::out_of_range &e) {
        throw exceptions::UnknownAuthorizationKeyException("Unknown authorization key");
    }
}

polymorph::network::SessionId polymorph::network::SessionStore::registerClient(asio::ip::udp::endpoint endpoint)
{
    std::lock_guard<std::mutex> lock(_udpSessionsMutex);

    try {
        return _udpSessions.at(endpoint);
    } catch (std::out_of_range &e) {
        auto sId = _findAvailableUdpSessionId();
        _udpSessions.emplace(endpoint, sId);
        return sId;
    }
}

void polymorph::network::SessionStore::registerAuthoredClient(asio::ip::tcp::socket socket,
        polymorph::network::SessionId sessionId, polymorph::network::AuthorizationKey key)
{
    try {
        std::unique_lock<std::mutex> lock(_tcpSessionsAuthorizationKeysMutex);
        auto verifyKey = _tcpSessionsAuthorizationKeys.at(sessionId);
        lock.unlock();
        if (authorizationKey::areSame(verifyKey, key)) {
            _tcpSessions[socket.remote_endpoint()] = sessionId;
        } else {
            throw exceptions::UnauthorizedException("InvalidAuthorization key");
        }
    } catch (std::out_of_range &e) {
        throw exceptions::UnknownAuthorizationKeyException("Unknown authorization key");
    }
}

polymorph::network::SessionId polymorph::network::SessionStore::registerClient(asio::ip::tcp::socket socket)
{
    auto sId = _findAvailableTcpSessionId();
    std::lock_guard<std::mutex> lock(_tcpSessionsMutex);

    try {
        _tcpSessions.at(socket.remote_endpoint());
        throw exceptions::AlreadyRegisteredException("Client already connected");
    } catch (std::out_of_range &e) {
    }

    _tcpSessions.emplace(std::move(socket.remote_endpoint()), sId);
    return sId;
}

void polymorph::network::SessionStore::removeClient(asio::ip::udp::endpoint endpoint)
{
    std::lock_guard<std::mutex> lock(_udpSessionsMutex);

    _udpSessions.erase(endpoint);
}

void polymorph::network::SessionStore::removeClient(asio::ip::tcp::socket socket)
{
    std::lock_guard<std::mutex> lock(_tcpSessionsMutex);

    _tcpSessions.erase(socket.remote_endpoint());
}

bool polymorph::network::SessionStore::removeClient(polymorph::network::SessionId sessionid)
{
    std::lock_guard<std::mutex> lock(_udpSessionsMutex);
    std::lock_guard<std::mutex> lock2(_tcpSessionsMutex);

    auto udpNb = std::erase_if(_udpSessions, [&sessionid](auto &pair) {
        return pair.second == sessionid;
    }) != 0;
    auto tcpNb = std::erase_if(_tcpSessions, [&sessionid](auto &pair) {
        return pair.second == sessionid;
    }) != 0;
    return udpNb != 0 || tcpNb != 0;
}

polymorph::network::SessionId polymorph::network::SessionStore::sessionOf(asio::ip::udp::endpoint endpoint)
{
    try {
        std::lock_guard<std::mutex> lock(_udpSessionsMutex);
        return _udpSessions.at(endpoint);
    } catch (std::out_of_range &e) {
        throw exceptions::UnknownSessionException("Unknown session");
    }
}

polymorph::network::SessionId polymorph::network::SessionStore::sessionOf(asio::ip::tcp::socket socket)
{
    try {
        std::lock_guard<std::mutex> lock(_tcpSessionsMutex);
        return _tcpSessions.at(socket.remote_endpoint());
    } catch (std::out_of_range &e) {
        throw exceptions::UnknownSessionException("Unknown session");
    }
}

asio::ip::udp::endpoint polymorph::network::SessionStore::udpEndpointOf(polymorph::network::SessionId session)
{
    std::lock_guard<std::mutex> lock(_udpSessionsMutex);
    auto it = std::find_if(_udpSessions.begin(), _udpSessions.end(), [session](auto &pair) {
        return pair.second == session;
    });

    if (it == _udpSessions.end())
        throw exceptions::UnknownSessionException("Unknown session");
    return it->first;
}

std::vector<asio::ip::udp::endpoint>
polymorph::network::SessionStore::udpEndpointsOfExcept(polymorph::network::SessionId except)
{
    std::lock_guard<std::mutex> lock(_udpSessionsMutex);
    std::vector<asio::ip::udp::endpoint> sessions;

    for (auto &session: _udpSessions)
        if (session.second != except)
            sessions.push_back(session.first);
    return sessions;
}

std::vector<polymorph::network::SessionId> polymorph::network::SessionStore::allUdpSessions()
{
    std::lock_guard<std::mutex> lock(_udpSessionsMutex);
    std::vector<SessionId> sessions;

    for (auto &session: _udpSessions)
        sessions.push_back(session.second);
    return sessions;
}

polymorph::network::AuthorizationKey
polymorph::network::SessionStore::generateUdpAuthorizationKey(polymorph::network::SessionId sessionId)
{
    std::lock_guard<std::mutex> lock(_udpSessionsAuthorizationKeysMutex);

    try {
        return _udpSessionsAuthorizationKeys.at(sessionId);
    } catch (std::out_of_range &e) {
        auto key = authorizationKey::generate();
        _udpSessionsAuthorizationKeys.emplace(sessionId, key);
        return key;
    }
}

polymorph::network::AuthorizationKey
polymorph::network::SessionStore::generateTcpAuthorizationKey(polymorph::network::SessionId sessionId)
{
    std::lock_guard<std::mutex> lock(_tcpSessionsAuthorizationKeysMutex);

    try {
        return _tcpSessionsAuthorizationKeys.at(sessionId);
    } catch (std::out_of_range &e) {
        auto key = authorizationKey::generate();
        _tcpSessionsAuthorizationKeys.emplace(sessionId, key);
        return key;
    }
}

polymorph::network::SessionId polymorph::network::SessionStore::_findAvailableTcpSessionId()
{
    std::lock_guard<std::mutex> lock(_tcpSessionsMutex);
    SessionId i = 0;

    while (std::find_if(_tcpSessions.begin(), _tcpSessions.end(), [&i](auto &p) { return p.second == i; }) != _tcpSessions.end())
        ++i;
    return i;
}

polymorph::network::SessionId polymorph::network::SessionStore::_findAvailableUdpSessionId()
{
    std::lock_guard<std::mutex> lock(_udpSessionsMutex);
    SessionId i = 0;

    while (std::find_if(_udpSessions.begin(), _udpSessions.end(), [&i](auto &p) { return p.second == i; }) != _udpSessions.end())
        ++i;
    return i;
}
