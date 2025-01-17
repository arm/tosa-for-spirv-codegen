//
// Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

// THIS FILE IS GENERATED WITH TOSA 0.80.0. DO NOT EDIT!
// See tosa2spirv/python/source_generator.py and README

#pragma once
#include <IParser.hpp>
#include <tosa_serialization_handler.h>
#include <cstdint>
#include <memory>
#include <vector>

namespace tosa2spirv
{
namespace parsers
{
/// Class which can parse a TOSA Serialization structure and generate SPIR-V.
class TosaSerializationParser : public IParser
{
public:
    /// Constructor for TosaSerializationParser.
    /// Initialises block specified by the user.
    /// This constructor will also initialise a Module which will be used to add a graph and write SPIR-V.
    /// @param[in] block TOSA Serialization basic block used for parsing.
    explicit TosaSerializationParser(tosa::TosaSerializationBasicBlock* block) : m_Block(block) {};

    /// Constructor for TosaSerializationParser.
    /// Initialises block from the TOSA Serialization Handler specified by name.
    /// This constructor will also initialise a Module which will be used to add a graph and write SPIR-V.
    /// @param[in] handler TOSA Serialization Handler used for parsing.
    /// @param[in] blockName The name of the block to use within the TOSA Serialization Handler.
    explicit TosaSerializationParser(tosa::TosaSerializationHandler* handler, std::string& blockName);

    /// Overridden method that parses the TOSA Serialization structure passed to the constructor and generates SPIR-V.
    /// @return A SPIR-V binary vector.
    std::vector<uint32_t> GenerateSPIRV(std::string graphName = std::string()) override;

    /// Overridden method that parses the TOSA Serialization structure and gathers the graph constants.
    /// @return A vector containing weights, biases and any other graph constants e.g. TOSA Attributes.
    std::map<uint32_t, GraphConstant> GetGraphConstants() override { return m_GraphConstants; }

private:

    /// Internal input initializer
    graphbuilder::Tensor InitializeInputTensor(const tosa::TosaSerializationTensor* inputTosaTensor,
                                               const std::string& inputName,
                                               const graphbuilder::Graph* graph);

    /// Internal output initializer
    graphbuilder::Tensor InitializeOutputTensor(tosa::TosaSerializationTensor* outputTosaTensor,
                                                const std::string& outputName);

    /// Private function to store GraphConstant in a map for writing to VGF
    /// @param[in] tensor Tensor which the GraphConstant is assigned to.
    /// @param[in] data graph constant data contained in a variant.
    /// @param tensorName Name of the tensor
    void AddGraphConstantToMap(const graphbuilder::Tensor& tensor,
                               const GraphConstant& data,
                               const std::string& tensorName);

    /// Internal conversion from DType to DataType
    static graphbuilder::DataType GetDataTypeFromDType(tosa::DType dType);

    /// Internal method to handle Identity operator in the block by
    /// mapping their inputName/OutputName to OutputName/InputName and ignore Identity in the parser.
    void OptimizeIdentityOp();

    /// Internal method to get the inputName after skipping Identity Ops.
    std::string GetInputName(std::string inputName);

    /// Internal method to get the outputName after skipping Identity Ops.
    std::string GetOutputName(std::string outputName);

    /// Internal operator parsing methods.
    void ParseOp(tosa::TosaSerializationOperator* op, graphbuilder::Graph* graph);

    /// TOSA Serialization basic block used for parsing.
    /// The user retains the memory.
    tosa::TosaSerializationBasicBlock* m_Block;

    /// unordered_map that holds key-value pairs of the input layer names and binding index from the TOSA graph.
    std::unordered_map<std::string, uint32_t> m_InputLayerNameMap;

    /// unordered_map that holds key-value pairs of output layer names and binding index from the TOSA graph.
    std::unordered_map<std::string, uint32_t> m_OutputLayerNameMap;

    /// unordered_map that holds key-value pairs of input names and output names for Identity operators.
    std::unordered_map<std::string, std::string> m_IdentityInputNameMap;

    /// unordered_map that holds key-value pairs of output names and input names for Identity operators.
    std::unordered_map<std::string, std::string> m_IdentityOutputNameMap;

    /// Increment counter for descriptorSet binding index.
    uint32_t m_DescSetBindingIdx {0};

    /// Map of TensorNames to ResIds
    std::map<const std::string, graphbuilder::ResId> m_OpNameResIdMap;
};

} // namespace parsers

} // namespace tosa2spirv