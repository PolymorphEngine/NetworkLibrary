/*
** EPITECH PROJECT, 2022
** AuthKey.cpp
** File description:
** AuthKey.cpp
*/

#include <gtest/gtest.h>
#include "authorizationKey.hpp"

TEST(AuthKey, SimpleKeyGenCmp)
{
    polymorph::network::AuthorizationKey key = polymorph::network::authorizationKey::generate();
    polymorph::network::AuthorizationKey key2 = polymorph::network::authorizationKey::generate();
    ASSERT_FALSE(polymorph::network::authorizationKey::areSame(key, key2));
}

TEST(AuthKey, SameKeyGenCmp)
{
    polymorph::network::AuthorizationKey key = polymorph::network::authorizationKey::generate();
    ASSERT_TRUE(polymorph::network::authorizationKey::areSame(key, key));
}

TEST(AuthKey, ZeroKeyGenCmp)
{
    polymorph::network::AuthorizationKey key = polymorph::network::authorizationKey::generate();
    polymorph::network::AuthorizationKey zero{0};
    ASSERT_FALSE(polymorph::network::authorizationKey::areSame(key, zero));
}

TEST(AuthKey, FullZeroKeyGenCmp)
{
    polymorph::network::AuthorizationKey zero1{0};
    polymorph::network::AuthorizationKey zero2{0};
    ASSERT_TRUE(polymorph::network::authorizationKey::areSame(zero1, zero2));
}

TEST(AuthKey, ManualSameKeyGenCmp)
{
    polymorph::network::AuthorizationKey polyKey {'p', 'o', 'l', 'y', 'm', 'o', 'r', 'p', 'h', 'n', 'e', 't', 'w', 'o', 'r', 'k'};
    polymorph::network::AuthorizationKey polyKey2 {'p', 'o', 'l', 'y', 'm', 'o', 'r', 'p', 'h', 'n', 'e', 't', 'w', 'o', 'r', 'k'};
    ASSERT_TRUE(polymorph::network::authorizationKey::areSame(polyKey, polyKey2));
}

TEST(AuthKey, ManualZeroKeyGenCmp)
{
    polymorph::network::AuthorizationKey polyKey {'p', 'o', 'l', 'y', 'm', 'o', 'r', 'p', 'h', 'n', 'e', 't', 'w', 'o', 'r', 'k'};
    polymorph::network::AuthorizationKey zero2{0};
    ASSERT_FALSE(polymorph::network::authorizationKey::areSame(polyKey, zero2));
}

TEST(AuthKey, SimpleManualKeyGenCmp)
{
    polymorph::network::AuthorizationKey polyKey {'p', 'o', 'l', 'y', 'm', 'o', 'r', 'p', 'h', 'n', 'e', 't', 'w', 'o', 'r', 'k'};
    polymorph::network::AuthorizationKey polyKey2 {'p', 'o', 'l', 'y', 'm', 'o', 'r', 'p', 'h', 'n', 'e', 't', 'w', 'o', 'r', 'K'};
    ASSERT_FALSE(polymorph::network::authorizationKey::areSame(polyKey, polyKey2));
}

TEST(AuthKey, SizeManualKeyGenCmp)
{
    polymorph::network::AuthorizationKey polyKey {'p', 'o', 'l', 'y', 'm', 'o', 'r', 'p', 'h', 'n', 'e', 't', 'w', 'o', 'r', 'k'};
    polymorph::network::AuthorizationKey polyKey2 {'d', 'e', 'e', 'd', 'b', 'e', 'e', 'f'};
    ASSERT_FALSE(polymorph::network::authorizationKey::areSame(polyKey, polyKey2));
}
