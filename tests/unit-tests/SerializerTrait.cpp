/*
** EPITECH PROJECT, 2022
** SerializerTrait.cpp
** File description:
** SerializerTrait.cpp
*/

#include <gtest/gtest.h>
#include "Polymorph/Network/SerializerTrait.hpp"

TEST(SerializerTrait, PacketHeader)
{
    polymorph::network::PacketHeader header;
    header.opId = 1;
    header.pId = 2;
    header.sId = 3;
    header.pSize = 4;
    std::vector<std::byte> serialized = polymorph::network::SerializerTrait<polymorph::network::PacketHeader>::serialize(header);
    ASSERT_EQ(serialized.size(), sizeof(polymorph::network::PacketHeader));
    polymorph::network::PacketHeader deserialized = polymorph::network::SerializerTrait<polymorph::network::PacketHeader>::deserialize(serialized);
    ASSERT_EQ(deserialized.opId, header.opId);
    ASSERT_EQ(deserialized.pId, header.pId);
    ASSERT_EQ(deserialized.sId, header.sId);
    ASSERT_EQ(deserialized.pSize, header.pSize);
}

TEST(SerializerTrait, Packet)
{
    polymorph::network::PacketHeader header {0};
    uint8_t payload = 42;
    polymorph::network::Packet<uint8_t> packet;
    packet.header = header;
    packet.payload = payload;
    std::vector<std::byte> serialized = polymorph::network::SerializerTrait<polymorph::network::Packet<uint8_t>>::serialize(packet);
    ASSERT_EQ(serialized.size(), sizeof(polymorph::network::PacketHeader) + sizeof(uint8_t));
    polymorph::network::Packet<uint8_t> deserialized = polymorph::network::SerializerTrait<polymorph::network::Packet<uint8_t>>::deserialize(serialized);
    ASSERT_EQ(deserialized.header.opId, header.opId);
    ASSERT_EQ(deserialized.header.pId, header.pId);
    ASSERT_EQ(deserialized.header.sId, header.sId);
    ASSERT_EQ(deserialized.header.pSize, header.pSize);
    ASSERT_EQ(deserialized.payload, payload);
}

struct A
{
    uint8_t a;
    uint8_t b;
    uint8_t c;
};

TEST(SerializerTrait, CustomPOD)
{
    A a;
    a.a = 1;
    a.b = 2;
    a.c = 3;
    std::vector<std::byte> serialized = polymorph::network::SerializerTrait<A>::serialize(a);
    ASSERT_EQ(serialized.size(), sizeof(A));
    A deserialized = polymorph::network::SerializerTrait<A>::deserialize(serialized);
    ASSERT_EQ(deserialized.a, a.a);
    ASSERT_EQ(deserialized.b, a.b);
    ASSERT_EQ(deserialized.c, a.c);
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

TEST(SerializerTrait, CustomNonPOD)
{
    B b;
    b.str = "Hello World!";
    std::vector<std::byte> serialized = polymorph::network::SerializerTrait<B>::serialize(b);
    ASSERT_EQ(serialized.size(), b.str.size());
    B deserialized = polymorph::network::SerializerTrait<B>::deserialize(serialized);
    ASSERT_EQ(deserialized.str, b.str);
}

TEST(SerializerTrait, PacketNonPOD)
{
    using namespace polymorph::network;
    B b;
    b.str = "Hello World!";
    PacketHeader header;
    header.opId = 1;
    Packet<B> packet;
    packet.header = header;
    packet.payload = b;
    std::vector<std::byte> serialized = SerializerTrait<Packet<B>>::serialize(packet);
    Packet<B> deserialized = SerializerTrait<Packet<B>>::deserialize(serialized);
    ASSERT_EQ(deserialized.header.opId, header.opId);
    ASSERT_EQ(deserialized.payload.str, b.str);
}