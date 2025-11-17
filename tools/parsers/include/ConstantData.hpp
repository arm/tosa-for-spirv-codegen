//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <cstdint>
#include <type_traits>
#include <variant>
#include <vector>

namespace tfsc::parsers
{
using ConstantData = std::variant<std::vector<uint8_t>,
                                  std::vector<int8_t>,
                                  std::vector<int16_t>,
                                  std::vector<int32_t>,
                                  std::vector<int64_t>,
                                  std::vector<float>>;

using PointerPair = std::pair<const void*, std::size_t>;

// Converts a vector of ConstantData to a vector of void* pointers
static std::vector<PointerPair> ConvertConstantDataToVoid(std::vector<ConstantData> constants)
{
    std::vector<PointerPair> voidPointers;
    voidPointers.reserve(constants.size());

    for (const auto& constant : constants)
    {
        std::visit(
            [&voidPointers](const auto& dataVector) {
                using DataType = std::decay_t<decltype(dataVector)>;
                voidPointers.emplace_back(static_cast<const void*>(dataVector.data()),
                                          dataVector.size() * sizeof(typename DataType::value_type));
            },
            constant);
    }

    return voidPointers;
}

} // namespace tfsc::parsers
