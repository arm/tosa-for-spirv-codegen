
//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <ParserUtils.hpp>

#include <gtest/gtest.h>

using namespace ::tosa;
using namespace tosa2spirv::parsers;
using namespace tosa2spirv::tosa;

TEST(UnpackInt4Signed, EvenNumberOfElements_PositiveAndNegative)
{
    // numElements = 2: one packed byte → two outputs
    // packed byte: high nibble = 0x1 (→ +1), low nibble = 0xF (→ sign-extended to 0xFF)
    Tensor t(DataType::uint8_t, {2});
    std::vector<uint8_t> in = {static_cast<uint8_t>((1 << 4) | 0xF)};
    auto out = UnpackInt4Signed(in, t);

    ASSERT_EQ(out.size(), 2u);
    EXPECT_EQ(out[0], uint8_t(0xFF));
    EXPECT_EQ(out[1], uint8_t(0x01));
}

TEST(ConvertToInt64, PositiveAndNegative48Bit)
{
    // 2 elements of 48-bit:
    //  1st = 1
    //  2nd = 0xFF'FF'FF'FF'FF'FF → sign-extended → -1
    Tensor t(DataType::int48_t, {2});
    std::vector<uint8_t> in = {// little-endian 48-bit +1
                               0x01,
                               0x00,
                               0x00,
                               0x00,
                               0x00,
                               0x00,
                               // all ones → -1
                               0xFF,
                               0xFF,
                               0xFF,
                               0xFF,
                               0xFF,
                               0xFF};
    auto out = ConvertToInt64(in, t);

    ASSERT_EQ(out.size(), 2u);
    EXPECT_EQ(out[0], int64_t(1));
    EXPECT_EQ(out[1], int64_t(-1));
}

TEST(ConvertToUint32, Uint8Type)
{
    // 4 elements of uint8_t → each padded to 4 bytes yields same values
    Tensor t(DataType::uint8_t, {4});
    std::vector<uint8_t> in = {1, 2, 3, 4};
    auto out = ConvertToUint32(in, t);

    ASSERT_EQ(out, std::vector<uint32_t>({1, 2, 3, 4}));
}

TEST(ConvertToUint32, Int16Type)
{
    // 3 elements of int16_t: [1,2,3] little-endian
    Tensor t(DataType::int16_t, {3});
    std::vector<uint8_t> in = {
        1,
        0, // 1
        2,
        0, // 2
        3,
        0 // 3
    };
    auto out = ConvertToUint32(in, t);

    ASSERT_EQ(out, std::vector<uint32_t>({1, 2, 3}));
}

TEST(ConvertToUint32, Int32Type)
{
    // 2 elements of int32_t: [0x01020304, 0x0A0B0C0D]
    Tensor t(DataType::int32_t, {2});
    std::vector<uint8_t> in = {
        0x04,
        0x03,
        0x02,
        0x01, // 0x01020304
        0x0D,
        0x0C,
        0x0B,
        0x0A // 0x0A0B0C0D
    };
    auto out = ConvertToUint32(in, t);

    ASSERT_EQ(out.size(), 2u);
    EXPECT_EQ(out[0], 0x01020304u);
    EXPECT_EQ(out[1], 0x0A0B0C0Du);
}
