/*
** EPITECH PROJECT, 2022
** ServerPacketManager.cpp
** File description:
** ServerPacketManager.cpp
*/


#include <gtest/gtest.h>
#include "polymorph/network/udp/ServerPacketManager.hpp"
#include "polymorph/network/udp/Server.hpp"

TEST(ServerPacketManagerTests, SimpleRegistering) {
    using namespace polymorph::network;
    using namespace polymorph::network::udp;
    asio::io_context context;
    std::map<OpId, bool> mapping = {
        {1, true}
    };
    auto clbk = [](std::shared_ptr<SafePacketManager>) {};
    ServerPacketManager store(context, mapping, clbk);


    asio::ip::udp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 4242);

    ASSERT_EQ(store.hasClient(endpoint), 0);
    ASSERT_EQ(store.registerClient(endpoint), 1);
    ASSERT_EQ(store.hasClient(endpoint), 1);
}

TEST(ServerPacketManagerTests, SimpleRegisterUnregister) {
    using namespace polymorph::network;
    using namespace polymorph::network::udp;
    asio::io_context context;
    std::map<OpId, bool> mapping = {
            {1, true}
    };
    auto clbk = [](std::shared_ptr<SafePacketManager>) {};
    ServerPacketManager store(context, mapping, clbk);

    asio::ip::udp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 4242);

    ASSERT_EQ(store.registerClient(endpoint), 1);
    ASSERT_EQ(store.removeClient(endpoint), 1);
    ASSERT_EQ(store.removeClient(endpoint), 0);
    ASSERT_EQ(store.hasClient(endpoint), 0);
}

TEST(ServerPacketManagerTests, AccessRegistedClientStore) {
    using namespace polymorph::network;
    using namespace polymorph::network::udp;
    asio::io_context context;
    std::map<OpId, bool> mapping = {
            {1, true}
    };
    auto clbk = [](std::shared_ptr<SafePacketManager>) {};
    ServerPacketManager store(context, mapping, clbk);


    asio::ip::udp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 4242);

    ASSERT_EQ(store.registerClient(endpoint), 1);
    ASSERT_NO_THROW({
                        store.storeOf(endpoint);
                    });
}

TEST(ServerPacketManagerTests, AccessStoreOfEmptyServerPacketManager) {
    using namespace polymorph::network;
    using namespace polymorph::network::udp;
    asio::io_context context;
    std::map<OpId, bool> mapping = {
            {1, true}
    };
    auto clbk = [](std::shared_ptr<SafePacketManager>) {};
    ServerPacketManager store(context, mapping, clbk);


    asio::ip::udp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 4242);

    ASSERT_THROW({
                     store.storeOf(endpoint);
                 }, std::out_of_range);
}

TEST(ServerPacketManagerTests, AccessStoreOfRemovedClient) {
    using namespace polymorph::network;
    using namespace polymorph::network::udp;
    asio::io_context context;
    std::map<OpId, bool> mapping = {
            {1, true}
    };
    auto clbk = [](std::shared_ptr<SafePacketManager>) {};
    ServerPacketManager store(context, mapping, clbk);

    asio::ip::udp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 4242);

    store.registerClient(endpoint);
    store.removeClient(endpoint);
    ASSERT_THROW({
        store.storeOf(endpoint);
    }, std::out_of_range);
}

TEST(ServerPacketManagerTests, AutoIncrementPacketId) {
    using namespace polymorph::network;
    using namespace polymorph::network::udp;
    asio::io_context context;
    std::map<OpId, bool> mapping = {
            {1, true}
    };
    auto clbk = [](std::shared_ptr<SafePacketManager>) {};
    ServerPacketManager store(context, mapping, clbk);

    asio::ip::udp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 4242);

    ASSERT_EQ(store.packetIdOf(endpoint), 1);
    ASSERT_EQ(store.packetIdOf(endpoint), 2);
}