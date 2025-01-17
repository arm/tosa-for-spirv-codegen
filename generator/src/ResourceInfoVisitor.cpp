//
// Copyright © 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#include <Graph.hpp>
#include <OperatorDefinitions.hpp>
#include <ResourceInfoVisitor.hpp>

namespace tosa2spirv::generator
{

ResourceInfoVisitor::~ResourceInfoVisitor() = default;

void ResourceInfoVisitor::Visit(const graphbuilder::Module& module)
{
    for (auto& constant : module.GetGraphConstants())
    {
        const auto constId = constant.GetGraphConstantId();
        const auto& tensor = constant.GetTensor();

        if (m_ResourceInfo.GetConstants().count(constId) == 0)
        {
            m_ResourceInfo.AddConstant(GetVulkanFormatFromDataType(tensor.GetDataType()),
                                       constant.GetGraphConstantId(),
                                       tensor.GetTensorShape());
        }
    }

    // Now that were visited all layers in all graphs we can add index to the inputs and outputs.
    auto inputIndex = 0;
    for (auto& input : m_ResourceInfo.GetInputs())
    {
        // Set the resource infos Indexes
        input.m_IOIndex = inputIndex;
        inputIndex++;
    }

    auto outputIndex = 0;
    for (auto& output : m_ResourceInfo.GetOutputs())
    {
        // Set the resource infos Indexes
        output.m_IOIndex = outputIndex;
        outputIndex++;
    }
}

void ResourceInfoVisitor::Visit(const graphbuilder::Graph& graph)
{
    for (auto& op : graph.GetOperators())
    {
        // Loop over all tensors and add resource entries as needed.
        for (auto& tensor : op.m_InputTensors)
        {
            if (tensor.GetDescSetBinding())
            {
                // Create string used to identify inputs and outputs
                auto LayerNameAndId = GetOperatorName(op.m_OperatorEnum) + "_" +
                                      std::to_string(op.m_ResIds[0].Get());
                m_ResourceInfo.AddInput(tensor.GetDescSetBinding()->GetBinding(),          // bindingId
                                        tensor.GetDescSetBinding()->GetDescriptorSet(),    // descriptorSet
                                        GetVulkanFormatFromDataType(tensor.GetDataType()), // format
                                        -1,                                                 // inputIndex
                                        "VK_TENSOR_TILING_LINEAR_EXT",                      // layout
                                        LayerNameAndId,                                     // name
                                        "VK_DESCRIPTOR_TYPE_STORAGE_TENSOR_EXT",            // resourceType
                                        tensor.GetTensorShape());                          // shape
            }
        }
        unsigned int index = 0;
        for (auto& tensor : op.m_OutputTensors)
        {
            if (tensor.GetDescSetBinding() && tensor.IsGraphOutput())
            {
                auto LayerNameAndId = GetOperatorName(op.m_OperatorEnum) + "_" +
                                      std::to_string(op.m_ResIds[index++].Get());
                m_ResourceInfo.AddOutput(tensor.GetDescSetBinding()->GetBinding(),
                                         tensor.GetDescSetBinding()->GetDescriptorSet(),
                                         GetVulkanFormatFromDataType(tensor.GetDataType()),
                                         -1,
                                         "VK_TENSOR_TILING_LINEAR_EXT",
                                         LayerNameAndId,
                                         "VK_DESCRIPTOR_TYPE_STORAGE_TENSOR_EXT",
                                         tensor.GetTensorShape());
            }
        }
    }
}

} // namespace generator