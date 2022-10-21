/*
** EPITECH PROJECT, 2022
** SerializerTrait.cpp
** File description:
** SerializerTrait.cpp
*/

#include <gtest/gtest.h>
#include "Polymorph/Network/DtoHandler.hpp"

TEST(DtoHandler, SimpleRegistered)
{
    using namespace polymorph::network;
    uint8_t payload = 42;
    PacketHeader packetHeader { 3, 4, 5, 6 };
    packetHeader.opId = 1;
    Packet<uint8_t> packet{ packetHeader, payload };
    auto serialized = SerializerTrait<Packet<uint8_t>>::serialize(packet);

    bool check = false;

    DtoHandler<uint8_t>::registerHandler(1, [&payload, &packetHeader, &check](PacketHeader pHeader, uint8_t &data) {
        ASSERT_EQ(data, payload);
        ASSERT_EQ(pHeader.opId, packetHeader.opId);
        ASSERT_EQ(pHeader.pId, packetHeader.pId);
        ASSERT_EQ(pHeader.sId, packetHeader.sId);
        ASSERT_EQ(pHeader.pSize, packetHeader.pSize);
        check = true;
    });
    DtoHandler<uint8_t>::handleReceivedPacket(serialized);
    ASSERT_TRUE(check);
    DtoHandler<uint8_t>::unregisterHandler(1);
}

TEST(DtoHandler, MultipleRegistered)
{
    using namespace polymorph::network;
    uint8_t payload = 42;
    PacketHeader packetHeader { 3, 4, 5, 6 };
    packetHeader.opId = 1;
    Packet<uint8_t> packet;
    packet.header = packetHeader;
    packet.payload = payload;
    auto serialized = SerializerTrait<Packet<uint8_t>>::serialize(packet);

    bool check1 = false;
    bool check2 = false;

    DtoHandler<uint8_t>::registerHandler(1, [&payload, &check1](PacketHeader pHeader, uint8_t &data) {
        ASSERT_EQ(data, payload);
        check1 = true;
    });

    DtoHandler<uint8_t>::registerHandler(1, [&payload, &check2](PacketHeader pHeader, uint8_t &data) {
        ASSERT_EQ(data, payload);
        check2 = true;
    });
    DtoHandler<uint8_t>::handleReceivedPacket(serialized);
    ASSERT_TRUE(check1);
    ASSERT_TRUE(check2);
    DtoHandler<uint8_t>::unregisterHandler(1);
}

TEST(DtoHandler, NoneRegistered)
{
    using namespace polymorph::network;
    uint8_t payload = 42;
    PacketHeader packetHeader { 3, 4, 5, 6 };
    packetHeader.opId = 1;
    Packet<uint8_t> packet{ packetHeader, payload };
    auto serialized = SerializerTrait<Packet<uint8_t>>::serialize(packet);


    EXPECT_NO_THROW({
        DtoHandler<uint8_t>::handleReceivedPacket(serialized);
    });
}

TEST(DtoHandler, OtherRegistered)
{
    using namespace polymorph::network;
    uint8_t payload = 42;
    PacketHeader packetHeader { 3, 4, 5, 6 };
    packetHeader.opId = 1;
    Packet<uint8_t> packet{ packetHeader, payload };
    auto serialized = SerializerTrait<Packet<uint8_t>>::serialize(packet);

    bool check = true;

    DtoHandler<uint16_t>::registerHandler(1, [&payload, &check](PacketHeader pHeader, uint16_t &data) {
        ASSERT_EQ(data, payload);
        check = false;
    });

    EXPECT_NO_THROW({
        DtoHandler<uint8_t>::handleReceivedPacket(serialized);
    });
    ASSERT_TRUE(check);
    DtoHandler<uint16_t>::unregisterHandler(1);
}
struct A
{
    uint8_t a;
    uint8_t b;
    uint8_t c;
};

TEST(DtoHandler, CustomRegistered)
{
    using namespace polymorph::network;
    A payload;
    payload.a = 42;
    payload.b = 42;
    payload.c = 42;
    PacketHeader packetHeader { 3, 4, 5, 6 };
    packetHeader.opId = 1;
    Packet<A> packet{ packetHeader, payload };
    auto serialized = SerializerTrait<Packet<A>>::serialize(packet);

    bool check = false;

    DtoHandler<A>::registerHandler(1, [&payload, &check](PacketHeader pHeader, A &data) {
        ASSERT_EQ(data.a, payload.a);
        ASSERT_EQ(data.b, payload.b);
        ASSERT_EQ(data.c, payload.c);
        check = true;
    });
    DtoHandler<A>::handleReceivedPacket(serialized);
    ASSERT_TRUE(check);
    DtoHandler<A>::unregisterHandler(1);
}

struct B
{
    std::string str;
};

template<>
struct polymorph::network::SerializerTrait<B>
{
    static std::vector<std::byte> serialize(const B &b)
    {
        std::vector<std::byte> buffer;
        buffer.resize(b.str.size());
        std::memcpy(buffer.data(), b.str.data(), b.str.size());
        return buffer;
    }

    static B deserialize(const std::vector<std::byte> &buffer)
    {
        B b;
        b.str.resize(buffer.size());
        std::memcpy(b.str.data(), buffer.data(), buffer.size());
        return b;
    }
};

TEST(DtoHandler, CustomNonPOD)
{
    using namespace polymorph::network;
    B b;
    b.str = "Hello World!";
    PacketHeader packetHeader { 3, 4, 5, 6 };
    packetHeader.opId = 1;
    Packet<B> packet{ packetHeader, b };
    std::vector<std::byte> serialized = polymorph::network::SerializerTrait<Packet<B>>::serialize(packet);


    bool check = false;

    DtoHandler<B>::registerHandler(1, [&b, &check](PacketHeader pHeader, B &data) {
        ASSERT_EQ(data.str, b.str);
        check = true;
    });
    DtoHandler<B>::handleReceivedPacket(serialized);
    ASSERT_TRUE(check);
    DtoHandler<B>::unregisterHandler(1);
}