
//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <ParserUtils.hpp>

#include <gtest/gtest.h>

using namespace ::tosa;
using namespace tfsc::parsers;
using namespace tfsc::tosa;

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
