//
// Copyright © 2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <AssemblyUtils.hpp>
#include <Module.hpp>

#include <cstdlib>
#include <memory>

namespace testutils
{
using namespace tfsc::spirv;

enum class ModelView
{
    problemsOnly,
    model,
    module
};

struct ComparatorOptions
{
    ModelView m_modelView = ModelView::model;
};

std::string
CompareModules(const std::shared_ptr<Module>& lhs, const std::shared_ptr<Module>& rhs, ComparatorOptions co = {});

inline std::string CompareModules(const std::shared_ptr<Module>& lhs, const std::string& rhs)
{
    return CompareModules(lhs, testutils::LoadSPIRVDisassembly(rhs));
}

} // namespace testutils
