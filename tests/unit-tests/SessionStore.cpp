/*
** EPITECH PROJECT, 2022
** SessionStore.cpp
** File description:
** SessionStore.cpp
*/

#include <gtest/gtest.h>
#include <asio/ip/udp.hpp>
#include "SessionStore.hpp"
#include "polymorph/network/exceptions/UnknownSessionException.hpp"

TEST(SessionStoreTests, UdpSessionStoreId)
{
    using namespace polymorph::network;
    SessionStore store;
    auto endpoint = asio::ip::udp::endpoint(asio::ip::address_v4::loopback(), 0);
    auto session = store.registerClient(endpoint);
    ASSERT_EQ(store.sessionOf(endpoint), session);
}

TEST(SessionStoreTests, TcpSessionStoreId)
{
    using namespace polymorph::network;
    SessionStore store;
    auto endpoint = asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(), 0);
    auto session = store.registerClient(endpoint);
    ASSERT_EQ(store.sessionOf(endpoint), session);
}

TEST(SessionStoreTests, UdpInvalidEraseSessionStoreId)
{
    using namespace polymorph::network;
    SessionStore store;
    auto endpoint = asio::ip::udp::endpoint(asio::ip::address_v4::loopback(), 0);
    ASSERT_NO_THROW({
        store.removeClient(endpoint);
    });
}

TEST(SessionStoreTests, TcpInvalidEraseSessionStoreId)
{
    using namespace polymorph::network;
    SessionStore store;
    auto endpoint = asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(), 0);
    ASSERT_NO_THROW({
        store.removeClient(endpoint);
    });
}

TEST(SessionStoreTests, IdInvalidEraseSessionStoreId)
{
    using namespace polymorph::network;
    SessionStore store;
    ASSERT_EQ(store.removeClient(4242), false);
}

TEST(SessionStoreTests, UdpValidEraseSessionStoreId)
{
    using namespace polymorph::network;
    SessionStore store;
    auto endpoint = asio::ip::udp::endpoint(asio::ip::address_v4::loopback(), 0);
    store.registerClient(endpoint);
    ASSERT_NO_THROW({
        store.removeClient(endpoint);
    });
}

TEST(SessionStoreTests, TcpValidEraseSessionStoreId)
{
    using namespace polymorph::network;
    SessionStore store;
    auto endpoint = asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(), 0);
    store.registerClient(endpoint);
    ASSERT_NO_THROW({
        store.removeClient(endpoint);
    });
}

TEST(SessionStoreTests, IdValidEraseSessionStoreId)
{
    using namespace polymorph::network;
    SessionStore store;
    auto session = store.registerClient(asio::ip::udp::endpoint(asio::ip::address_v4::loopback(), 0));
    ASSERT_EQ(store.removeClient(session), true);
}

TEST(SessionStoreTests, UdpFailRemoveSessionStoreId)
{
    using namespace polymorph::network;
    SessionStore store;
    auto endpoint = asio::ip::udp::endpoint(asio::ip::address_v4::loopback(), 0);
    auto session = store.registerClient(endpoint);
    ASSERT_EQ(store.sessionOf(endpoint), session);
    store.removeClient(endpoint);
    ASSERT_THROW(store.sessionOf(endpoint), exceptions::UnknownSessionException);
}

TEST(SessionStoreTests, TcpFailRemoveSessionStoreId)
{
    using namespace polymorph::network;
    SessionStore store;
    auto endpoint = asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(), 0);
    auto session = store.registerClient(endpoint);
    ASSERT_EQ(store.sessionOf(endpoint), session);
    store.removeClient(endpoint);
    ASSERT_THROW(store.sessionOf(endpoint), exceptions::UnknownSessionException);
}

TEST(SessionStoreTests, UdpRemoveSessionStoreId)
{
    using namespace polymorph::network;
    SessionStore store;
    auto endpoint = asio::ip::udp::endpoint(asio::ip::address_v4::loopback(), 0);
    auto session = store.registerClient(endpoint);
    ASSERT_EQ(store.sessionOf(endpoint), session);
    ASSERT_NO_THROW(store.sessionOf(endpoint));
}

TEST(AsioTest, AsioEndpointTcp)
{
    auto endpoint1 = asio::ip::tcp::endpoint(asio::ip::make_address_v4("127.0.0.0"), 0);
    auto endpoint2 = asio::ip::tcp::endpoint(asio::ip::make_address_v4("127.0.0.0"), 0);
    ASSERT_EQ(endpoint1, endpoint2);
}

TEST(AsioTest, AsioEndpointUdp)
{
    auto endpoint1 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.0"), 0);
    auto endpoint2 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.0"), 0);
    ASSERT_EQ(endpoint1, endpoint2);
}

TEST(AsioTest, AsioNotEqualEndpointTcp)
{
    auto endpoint1 = asio::ip::tcp::endpoint(asio::ip::make_address_v4("127.0.0.1"), 0);
    auto endpoint2 = asio::ip::tcp::endpoint(asio::ip::make_address_v4("127.0.0.0"), 0);
    ASSERT_NE(endpoint1, endpoint2);
}

TEST(AsioTest, AsioNotEqualEndpointUdp)
{
    auto endpoint1 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.1"), 0);
    auto endpoint2 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.0"), 0);
    ASSERT_NE(endpoint1, endpoint2);
}

TEST(SessionStoreTests, TcpRemoveSessionStoreId)
{
    using namespace polymorph::network;
    SessionStore store;
    auto endpoint = asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(), 0);
    auto session = store.registerClient(endpoint);
    ASSERT_EQ(store.sessionOf(endpoint), session);
    ASSERT_NO_THROW(store.sessionOf(endpoint));
}

TEST(SessionStoreTests, UdpExceptSessionStoreId)
{
    using namespace polymorph::network;
    SessionStore store;
    auto endpoint1 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.1"), 1);
    auto endpoint2 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.2"), 2);
    auto endpoint3 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.3"), 3);
    auto session = store.registerClient(endpoint1);
    store.registerClient(endpoint2);
    store.registerClient(endpoint3);
    auto excepted = store.udpEndpointsOfExcept(session);
    ASSERT_EQ(excepted.size(), 2);
}

TEST(SessionStoreTests, Copy)
{
    using namespace polymorph::network;
    SessionStore store;
    auto endpoint1 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.1"), 1);
    auto endpoint2 = asio::ip::tcp::endpoint(asio::ip::make_address_v4("127.0.0.2"), 2);
    auto endpoint3 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.3"), 3);
    auto endpoint4 = asio::ip::tcp::endpoint(asio::ip::make_address_v4("127.0.0.4"), 4);
    auto session1 = store.registerClient(endpoint1);
    auto session2 = store.registerClient(endpoint2);
    auto session3 = store.registerClient(endpoint3);
    auto session4 = store.registerClient(endpoint4);
    SessionStore copy = store;
    ASSERT_EQ(copy.sessionOf(endpoint1), session1);
    ASSERT_EQ(copy.sessionOf(endpoint2), session2);
    ASSERT_EQ(copy.sessionOf(endpoint3), session3);
    ASSERT_EQ(copy.sessionOf(endpoint4), session4);
}

TEST(SessionStoreTests, CopyTcpSessions)
{
    using namespace polymorph::network;
    SessionStore store;
    SessionStore copy = store;
    auto endpoint1 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.1"), 1);
    auto endpoint2 = asio::ip::tcp::endpoint(asio::ip::make_address_v4("127.0.0.2"), 2);
    auto endpoint3 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.3"), 3);
    auto endpoint4 = asio::ip::tcp::endpoint(asio::ip::make_address_v4("127.0.0.4"), 4);
    store.registerClient(endpoint1);
    auto session2 = store.registerClient(endpoint2);
    store.registerClient(endpoint3);
    auto session4 = store.registerClient(endpoint4);
    copy.copyTcpSessionsFrom(store);
    ASSERT_EQ(copy.sessionOf(endpoint2), session2);
    ASSERT_EQ(copy.sessionOf(endpoint4), session4);
    try {
        copy.sessionOf(endpoint1);
        FAIL();
    } catch (const exceptions::UnknownSessionException&) {}
    try {
        copy.sessionOf(endpoint3);
        FAIL();
    } catch (const exceptions::UnknownSessionException&) {}

}

TEST(SessionStoreTests, CopyUdpSessions)
{
    using namespace polymorph::network;
    SessionStore store;
    SessionStore copy = store;
    auto endpoint1 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.1"), 1);
    auto endpoint2 = asio::ip::tcp::endpoint(asio::ip::make_address_v4("127.0.0.2"), 2);
    auto endpoint3 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.3"), 3);
    auto endpoint4 = asio::ip::tcp::endpoint(asio::ip::make_address_v4("127.0.0.4"), 4);
    auto session1 = store.registerClient(endpoint1);
    store.registerClient(endpoint2);
    auto session3 = store.registerClient(endpoint3);
    store.registerClient(endpoint4);
    copy.copyUdpSessionsFrom(store);
    ASSERT_EQ(copy.sessionOf(endpoint1), session1);
    ASSERT_EQ(copy.sessionOf(endpoint3), session3);
    try {
        copy.sessionOf(endpoint2);
        FAIL();
    } catch (const exceptions::UnknownSessionException&) {}
    try {
        copy.sessionOf(endpoint4);
        FAIL();
    } catch (const exceptions::UnknownSessionException&) {}
}

TEST(SessionStoreTests, CopyUdpAuthorizationKeys)
{
    using namespace polymorph::network;
    SessionStore store;
    SessionStore copy = store;
    auto endpoint1 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.1"), 1);
    auto endpoint2 = asio::ip::tcp::endpoint(asio::ip::make_address_v4("127.0.0.2"), 2);
    auto endpoint3 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.3"), 3);
    auto endpoint4 = asio::ip::tcp::endpoint(asio::ip::make_address_v4("127.0.0.4"), 4);
    auto session2 = store.registerClient(endpoint2);
    auto session4 = store.registerClient(endpoint4);
    auto key2 = store.generateUdpAuthorizationKey(session2);
    auto key4 = store.generateUdpAuthorizationKey(session4);
    copy.copyUdpAuthorizationKeysFrom(store);
    ASSERT_NO_THROW({
        copy.registerAuthoredClient(endpoint1, session2, key2);
    });
    ASSERT_NO_THROW({
        copy.registerAuthoredClient(endpoint3, session4, key4);
    });
}

TEST(SessionStoreTests, CopyTcpAuthorizationKeys)
{
    using namespace polymorph::network;
    SessionStore store;
    SessionStore copy = store;
    auto endpoint1 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.1"), 1);
    auto endpoint2 = asio::ip::tcp::endpoint(asio::ip::make_address_v4("127.0.0.2"), 2);
    auto endpoint3 = asio::ip::udp::endpoint(asio::ip::make_address_v4("127.0.0.3"), 3);
    auto endpoint4 = asio::ip::tcp::endpoint(asio::ip::make_address_v4("127.0.0.4"), 4);
    auto session1 = store.registerClient(endpoint1);
    auto session3 = store.registerClient(endpoint3);
    auto key1 = store.generateTcpAuthorizationKey(session1);
    auto key3 = store.generateTcpAuthorizationKey(session3);
    copy.copyTcpAuthorizationKeysFrom(store);
    ASSERT_NO_THROW({
        copy.registerAuthoredClient(endpoint2, session1, key1);
    });
    ASSERT_NO_THROW({
        copy.registerAuthoredClient(endpoint4, session3, key3);
    });
}