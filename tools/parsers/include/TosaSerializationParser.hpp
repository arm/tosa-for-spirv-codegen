//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// THIS FILE IS GENERATED WITH TOSA 1.0. DO NOT EDIT!
// See tosa2spirv/python/source_generator.py and README

#pragma once
#include "IParser.hpp"

#include <tosa/Graph.hpp>
#include <tosa_serialization_handler.h>

#include <memory>

using tfsc::tosa::ResId;

namespace tfsc::parsers
{
/// Class which can parse a TOSA Serialization structure and generate SPIR-V.
class TosaSerializationParser : public IParser
{
    public:
    /// Constructor for TosaSerializationParser.
    /// Initialises block specified by the user.
    /// This constructor will also initialise a Module which will be used to add a graph and write SPIR-V.
    /// @param[in] block TOSA Serialization basic block used for parsing.
    explicit TosaSerializationParser(::tosa::TosaSerializationBasicBlock* block)
        : m_Block(block)
    {
    }

    /// Constructor for TosaSerializationParser.
    /// Initialises block from the TOSA Serialization Handler specified by name.
    /// This constructor will also initialise a Module which will be used to add a graph and write SPIR-V.
    /// @param[in] handler TOSA Serialization Handler used for parsing.
    /// @param[in] blockName The name of the block to use within the TOSA Serialization Handler.
    explicit TosaSerializationParser(::tosa::TosaSerializationHandler* handler, const std::string& blockName);

    /// Overridden method that parses the TOSA Serialization structure passed to the constructor and generates SPIR-V.
    /// @return A SPIR-V binary vector.
    std::shared_ptr<spirv::Module> GenerateSPIRVModule(std::string graphName) override;
    std::vector<uint32_t> GenerateSPIRV(std::string graphName) override;

    private:
    /// Internal operator parsing methods.
    void ParseOperator(::tosa::TosaSerializationOperator* op, tfsc::tosa::Graph& graph);

    /// Internal constant operator parsing method.
    void ParseConstantOp(::tosa::TosaSerializationOperator* op, tosa::Graph& graph);

    /// Internal constant shape parsing method.
    ResId ParseConstantShape(const std::string& shapeName, tosa::Graph& graph);

    /// Internal constant tensor parsing method.
    ResId ParseConstantTensor(const std::string& tensorName, tosa::Graph& graph);

    /// TOSA Serialization basic block used for parsing.
    /// The user retains the memory.
    ::tosa::TosaSerializationBasicBlock* m_Block;

    /// Private function to store GraphConstant in a map for writing to VGF
    /// @param graph graph to add constant too
    /// @param[in] constTensor Tensor which the GraphConstant is assigned to.
    /// @param[in] data graph constant data contained in a variant.
    /// @param constName Name of the tensor
    ResId AddExternalConstant(tosa::Graph& graph,
                              const tosa::Tensor& constTensor,
                              const ConstantData& data,
                              const std::string& constName);

    /// Internal function to get input tensor ResId
    ResId GetInputTensorResId(tosa::Graph& graph, const std::string& inputName);

    /// Map of TensorNames to ResIds
    std::map<const std::string, ResId, std::less<>> m_OpNameResIdMap;
    std::map<ResId, tosa::Tensor> m_ResIdTensorMap;
};

} // namespace tfsc::parsers
