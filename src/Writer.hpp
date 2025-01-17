//
// Copyright 2017 The Clspv Authors. All rights reserved.
// Copyright © 2023, 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <Context.hpp>
#include <GraphInfo.hpp>
#include <IdGenerator.hpp>
#include <Instruction.hpp>
#include <Operand.hpp>
#include <Tensor.hpp>
#include <TypeRegistry.hpp>

#include <spirv/unified1/spirv.hpp>

#include <cstdint>
#include <map>
#include <vector>


namespace tosa2spirv
{

namespace spirvwriter
{

/// Enum to denote if an additional type is being passed to WriteGraphConstantInstruction.
/// Each additional type will require a special case during type creation
enum AdditionalType
{
    none = 0,
    array = 1,
};

using namespace ::spv;

/// Class which handles writing SPIR-V instructions to binary.
class Writer
{
public:
    /// Constructor which initialises the Writer's references to the IdGenerator, TypeRegistry and Context.
    /// This IdGenerator& and Context pointer should be passed down from the Module's member variable.
    /// @param[in] idGenerator IdGenerator reference to be saved as member variable.
    /// @param[in] context Context pointer to be saved as member variable.
    explicit Writer(graphbuilder::IdGenerator& idGenerator, std::shared_ptr<graphbuilder::Context> context)
        : m_IdGenerator(idGenerator)
        , m_TypeRegistry(*this, idGenerator)
        , m_Context(context)
    {
        InitTypeMap();
    };

    /// Constructor which initialises the Writer's references to the IdGenerator, TypeRegistry and Context.
    /// This IdGenerator& should be passed down from the Module's member variable.
    /// @param[in] idGenerator IdGenerator reference to be saved as member variable.
    /// context pointer is initialized to point to a new instance of the Context class.
    explicit Writer(graphbuilder::IdGenerator& idGenerator)
            : m_IdGenerator(idGenerator)
            , m_TypeRegistry(*this, idGenerator)
            , m_Context(std::make_shared<graphbuilder::Context>())
    {
        InitTypeMap();
    };

    /// Delete default constructor
    Writer() = delete;

    /// Default destructor.
    ~Writer() = default;

    /// Return the SPIR-V binary vector to the user.
    /// @return A SPIR-V binary vector.
    std::vector<uint32_t>& GetBinary() { return m_Binary; };

    /// Write the SPIR-V header information to the binary data structure.
    /// This must be called at the start.
    void WriteHeader();

    /// Write a list of instructions to the binary data structure.
    /// @param[in] instructions List of instructions to write to binary.
    void WriteBinary(InstructionList& instructions);

    /// Write an instruction to the binary data structure.
    /// @param[in] instruction Instruction to write to binary.
    void WriteBinary(Instruction& instruction);

    /// Update the bounds defined in the header.
    /// This must be the last ResId used + 1.
    /// @param[in] bound Bound value to write into header.
    void WriteBounds(const uint32_t& bound);

    /// Write Capability instructions to the binary data structure.
    /// @param[in] capabilities Vector of Capabilities to write to binary.
    void WriteCapabilities(const std::vector<Capability>& capabilities);

    /// Write Extension instruction to the binary data structure.
    /// @param[in] extension name, as a string, of extension to write to binary.
    void WriteExtension(const std::string& extension);

    /// Write Extended Instruction Set Import instruction to the binary data structure.
    /// @param[in] instructionSet name, as a string, of instruction set to write to binary.
    /// @return ResId Result Id of the Instruction Set that has been imported.
    graphbuilder::ResId WriteExtInstSetImport(const std::string& instructionSet);

    /// Write a MemoryModel instruction to the binary data structure.
    /// @param[in] addressingModel Addressing Model to write to binary.
    /// @param[in] memoryModel Memory Model to write to binary.
    void WriteMemoryModel(AddressingModel addressingModel = AddressingModelLogical,
                          MemoryModel memoryModel = MemoryModelVulkan);

    /// Save the position for the decorations to start from.
    void SaveDecorationPosition();

    /// Write a Decoration instruction to the binary data structure.
    /// @param[in] decoration Decoration to write to binary.
    /// @param[in] literalNum Operand literal number to write to binary.
    /// @param[in] id ResId to write to Decoration.
    void WriteDecoration(Decoration decoration, uint32_t literalNum, graphbuilder::ResId id);

    /// Helper method to get the TypeId for AddXXXInstruction methods.
    /// @param[in] tensor Tensor reference to get the TypeId.
    /// @return ResId for the input tensor.
    graphbuilder::ResId CreatePrimitiveTypeId(const graphbuilder::Tensor& tensor);

    /// Add a Constant instruction for a Graph Input or Output index.
    /// Only supports adding single constant data.
    /// @param[in] index Index value used to create a 32-bit integer.
    /// @return Operand containing the ResultId of the Tensor Instruction that has been written.
    Operand WriteGraphInputOutputIndexConstant(int32_t index);

    /// Write a Constant instruction.
    /// Only supports adding single constant data.
    /// @param[in] tensor Tensor reference to write to binary.
    /// @return Operand containing the ResultId of the Tensor Instruction that has been added.
    Operand WriteConstantInstruction(const graphbuilder::Tensor& tensor);

    /// Write a Tensor instruction to the binary data structure.
    /// @param[in] tensor Tensor reference to write to binary.
    /// @param[in] typeName AdditionalType enum used to allow creation of Tensors with types requiring special cases.
    /// @param[in] TensorShapeAsArray Boolean to force the tensor's shape to be used as array's values.
    /// @return Operand containing the ResultId of the Tensor Instruction that has been added.
    Operand WriteTensorInstruction(const graphbuilder::Tensor& tensor,
                                   AdditionalType typeName = AdditionalType::none,
                                   bool TensorShapeAsArray = false);

    Operand WriteGraphConstantInstruction(graphbuilder::GraphConstant& graph_constant, AdditionalType typeName);
    /// Write an GraphConstant instruction to the binary data structure.
    /// @param[in] tensor Tensor reference to write to binary.
    /// @param[in] typeName AdditionalType enum used to allow creation of Tensors with types requiring special cases.
    /// @return Operand containing the ResultId of the Tensor Instruction that has been added.

    /// @param[in] tensor Tensor reference to write to binary.
    /// @param[in] tensorTypeId Instruction for the type of the tensor passed in.
    /// @param[in] TensorShapeAsArray Boolean to force the tensor's shape to be used as array's values.
    /// @return vector of Operands: Array type, length and composite operand IDs.
    std::vector<Operand> WriteArrayCompoundInstruction(const graphbuilder::Tensor& tensor,
                                                       graphbuilder::ResId tensorTypeId = 0,
                                                       bool TensorShapeAsArray = false);

    /// @param[in] tensor Tensor reference to write to binary.
    /// @param[in] tensorTypeId Instruction for the type of the tensor passed in.
    /// @return Operand containing the ResultId of the Tensor Instruction that has been added.
    Operand WriteTensorCompoundInstruction(const graphbuilder::Tensor& tensor,
                                           graphbuilder::ResId tensorTypeId = 0);

    /// @param[in] tensor Tensor reference to write to binary.
    /// @param[in] tensorTypeId Instruction for the type of the tensor passed in.
    /// @param[in] TensorShapeAsArray Boolean to force the tensor's shape to be used as array's values.
    /// @return ResID for the new Tensor.
    graphbuilder::ResId WriteArrayTensorInstruction(const graphbuilder::Tensor& tensor,
                                                    graphbuilder::ResId tensorTypeId,
                                                    bool TensorShapeAsArray = false);

    /// @param[in] TensorTypeIdOpVect OpTypeTensorArm Operands vector for OpTypeStruct.
    /// @return Operand for the constructed OpTypeStruct.
    Operand WriteTypeStructInstruction(std::vector<Operand>& TensorTypeIdOpVect);

    /// @param[in] layerId ResId for the layer output.
    /// @param[in] TensorTypeIdOp OpTypeTensorArm Operands to identify the tensor type.
    /// @param[in] index to identify the position in the TypeStruct Composite.
    /// @return Instruction for the constructed OpCompositeExtract to be added to m_LayerList in graph.
    Instruction WriteCompositeExtractInstruction(graphbuilder::ResId operatorId,
                                                 graphbuilder::ResId compositeExtractId,
                                                 Operand& TensorTypeIdOp,
                                                 unsigned int index);

    /// Write a GraphType instruction to the binary data structure.
    /// @param[in] numInputs Operand containing number of inputs to the graph.
    /// @param[in] inputs Vector of GraphIOInfo containing resultIds of graph inputs.
    /// @param[in] outputs Vector of GraphIOInfo containing resultIds of graph outputs.
    /// @return Operand containing the ResultId of the GraphType Instruction that has been added.
    Operand WriteGraphTypeInstruction(unsigned int numInputs,
                                      const std::vector<graphbuilder::GraphIOInfo>& inputs,
                                      const std::vector<graphbuilder::GraphIOInfo>& outputs);

    /// Write a GraphEntryPoint instruction to the binary data structure.
    /// @param[in] graphId ResId of the graph.
    /// @param[in] name std::string with the name of the graph.
    /// @param[in] inputs Vector of PointerAndTensorPairs containing resultIds of graph inputs.
    /// @param[in] outputs Vector of PointerAndTensorPairs containing resultIds of graph outputs.
    void WriteGraphEntryPointInstruction(const graphbuilder::ResId& graphId,
                                         const std::string& name,
                                         const std::vector<graphbuilder::GraphIOInfo>& inputs,
                                         const std::vector<graphbuilder::GraphIOInfo>& outputs);

    /// Write a GraphExt instruction to the binary data structure.
    /// @param[in] graphId Operand containing ResId of the graph.
    /// @param[in] opGraphTypeId Operand containing ResId of the graph type..
    void WriteGraphInstruction(const graphbuilder::ResId& graphId,
                               const Operand& opGraphTypeId);

    /// Write a GraphEnd instruction to the binary data structure to signify the end of the graph.
    void WriteGraphEndInstruction();

    /// Write a GraphInput instruction to the binary data structure.
    /// @param[in] list InstructionList that will later be passed to WriteBinary().
    /// @param[in] inputTensorIdOp Operand containing the ResultId of the Input Tensor Instruction.
    /// @return Operand containing the ResultId of the Tensor Instruction that has been added.
    Operand AddGraphInputInstruction(InstructionList& list, const Operand& inputTensorIdOp);

    /// Write a GraphSetOutputs instruction to the binary data structure.
    /// @param[in] outputs GraphIOInfo struct containing Operands with IDs for layers which will be graph outputs.
    void WriteGraphSetOutputInstruction(const std::vector<graphbuilder::GraphIOInfo>& outputs);

    /// Write a Pointer instruction to the binary data structure.
    /// @param[in] list InstructionList that will later be passed to WriteBinary().
    /// @param[in] tensor Tensor pointer that will be represented by the pointer instruction.
    /// @return Operand containing the ResultId of the Tensor Instruction that has been added.
    Operand AddPointerInstruction(InstructionList& list, const graphbuilder::Tensor& tensor);

    /// Getter used to access member IdGenerator to allow writer to generate Ids.
    /// @return Operand containing the ResultId of the Tensor Instruction that has been added.
    graphbuilder::ResId GetNextId() { return m_IdGenerator.GetNextId(); };

    TypeRegistry& GetTypeRegistry() { return m_TypeRegistry; };

private:
    void WriteWord(const uint32_t& word);
    void WriteWordAtPos(const uint32_t& word, const uint32_t& position);
    void WriteOperand(const Operand& operand);
    void WriteOperandAtPos(const Operand& operand, const uint32_t& position);
    void WriteWordCountAndOpcode(const Instruction& instruction);
    void WriteWordCountAndOpcodeAtPos(const Instruction& instruction, const uint32_t& position);

    /// Helper method to initialize the m_TypeMap for constructor.
    void InitTypeMap();

    std::vector<uint32_t> m_Binary;

    std::map<graphbuilder::DataType, std::tuple<spv::Op, unsigned int, unsigned int>> m_TypeMap;

    /// Stores the position of the bound, which is used when invoking WriteBounds().
    uint32_t m_BoundsPosition = 0;
    /// Stores the position of the last decoration, which is set in WriteMemoryModel() and used in WriteDecoration().
    uint32_t m_DecorationPosition = 0;
    graphbuilder::IdGenerator& m_IdGenerator;
    std::shared_ptr<graphbuilder::Context> m_Context;
    TypeRegistry m_TypeRegistry;
};

} // spirvwriter namespace

} // tosa2spirv namespace