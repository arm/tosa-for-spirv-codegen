//
// Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <tosa2spirv.hpp>
#include <gtest/gtest.h>



TEST(TOSA2SPIRV, IdGenerator)
{
    using namespace tosa2spirv::graphbuilder;
    IdGenerator gen1;
    IdGenerator gen2;

    auto id1 = gen1.GetNextId();
    auto id2 = gen2.GetNextId();

    // Check Both IDs are the same for the 2 generators.
    EXPECT_EQ(id1, id2);

    // Restart generator 2
    gen2.Restart();

    // Generate 2 more IDs
    id1 = gen1.GetNextId();
    id2 = gen2.GetNextId();

    // Check the Ids are different
    EXPECT_EQ(id1, 2);
    EXPECT_EQ(id2, 1);
}

TEST(TOSA2SPIRV, IdGeneratorWrite)
{
    using namespace tosa2spirv::graphbuilder;
    auto module = Module::Create();
    auto binary = module->Write();

    auto module2 = Module::Create();
    auto binary2 = module2->Write();
    auto binary3 = module2->Write();

    // Check Binaries are the same for the 2 modules including the bounds which is set using generated IDs
    EXPECT_EQ(binary, binary2);
    // Check subsequent calls to Write() retain the same bounds Id on module thanks to IdGenerator::Restart()
    EXPECT_EQ(binary2, binary3);
}

TEST(TOSA2SPIRV, GetNextAvailableBindingTest)
{
    using namespace tosa2spirv::graphbuilder;

    // Add binding ID
    auto module = Module::Create();
    EXPECT_EQ(module->GetNextAvailableBinding(0), 0);

    // Create pair with Binding of 0 to DescSet 0
    module->AddDescSetBindingPair(0, 0, "m1.desc0.0");
    EXPECT_EQ(module->GetNextAvailableBinding(0), 1); // Expect max+1 from descSet 0

    // Create pair with Binding of 2 to DescSet 0
    module->AddDescSetBindingPair(0, 2, "m1.desc0.2");
    EXPECT_EQ(module->GetNextAvailableBinding(0), 3);

    module->AddDescSetBindingPair(0, module->GetNextAvailableBinding(0), "desc1");
    EXPECT_EQ(module->GetNextAvailableBinding(0), 4);

    auto module2 = Module::Create();
    module2->AddDescSetBindingPair(0, 0, "m2.desc0.0");
    EXPECT_EQ(module2->GetNextAvailableBinding(0), 1); // Expect max+1 from new module, descSet 0

    module2->AddDescSetBindingPair(1, 2, "m2.desc1.2");
    EXPECT_EQ(module2->GetNextAvailableBinding(1), 3); // Expect max+1 from descSet 1

    module2->AddDescSetBindingPair(1, 3, "m2.desc1.3");
    EXPECT_EQ(module2->GetNextAvailableBinding(0), 1);
}