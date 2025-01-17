//
// Copyright © 2023, 2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <DescSetBinding.hpp>
#include <OperatorDefinitions.hpp>
#include <ResId.hpp>

#include <vector>
#include <memory>

namespace tosa2spirv
{

namespace graphbuilder
{

/// Enum class to encapsulate the different data types of Tensors.
enum class DataType
{
    int4_t = 0,
    int8_t = 1,
    int16_t = 2,
    int32_t = 3,
    uint8_t = 4,
    uint16_t = 5,
    uint32_t = 6,
    float16_t = 7,
    float32_t = 8,
    bfloat16_t = 9,
    bool_t = 10,
};

/// Forward declare internal TensorInfo used a private member in Tensor.
struct TensorInfo;

class GraphConstant;
/// Class which encapsulates tosa2spirv's internal representation of a Tensor.
class Tensor
{
public:
    using TensorShape = std::vector<unsigned int>;
    using ModulePtrRef = const std::unique_ptr<Module, void(*)(Module*)>&;

    /// Constructor for Tensor
    /// @param[in] dataType Datatype that tensor values are.
    /// @param[in] shape Shape of the Tensor. If Tensor shape is empty then the shape will be set to 1 automatically.
    /// @param[in] id Id of the Tensor which may have been the return value of a call to Graph::AddXXXLayer();
    /// @param[in] isGraphOutput Used to determine if a layer is an output in the graph.
    /// @param[in] tensorValuePtr shared_ptr used to store the actual values of the tensor stored as uint32_t.
    /// Note: It is advised to use the helper methods such as ConvertInt32tToUint32t() to pass tensorValuePtr.
    Tensor(DataType dataType,
           TensorShape shape,
           ResId id = 0,
           bool isGraphOutput = false,
           const std::vector<uint32_t>& tensorValuePtr = {});

    Tensor(const Tensor& other) noexcept;
    /// Move Constructor
    Tensor(Tensor&& other) noexcept;
    /// Assignment Operator
    Tensor& operator= (const Tensor& other);

    /// Creates an Attribute Tensor
    /// @param[in] dataType Datatype that tensor values are.
    /// @param[in] shape Shape of the Tensor. If Tensor shape is empty then the shape will be set to 1 automatically.
    /// @return an instance of the Tensor class
    static Tensor CreateAttribute(const DataType& dataType, const TensorShape& shape);

    /// Creates an Attribute Tensor with a tensor value being passed through a tensor value pointer
    /// @param[in] dataType Datatype that tensor values are.
    /// @param[in] shape Shape of the Tensor. If Tensor shape is empty then the shape will be set to 1 automatically.
    /// @param[in] tensorValuePtr shared_ptr used to store the actual values of the tensor stored as uint32_t.
    /// @return an instance of the Tensor class
    static Tensor CreateAttribute(const DataType& dataType,
                                  const TensorShape& shape,
                                  const std::vector<uint32_t>& tensorValuePtr);

    /// Creates an Input Tensor and automatically adds a DescSetBinding and assigns it to the Input tensor.
    /// @param[in] dataType Datatype that tensor values are.
    /// @param[in] shape Shape of the Tensor. If Tensor shape is empty then the shape will be set to 1 automatically.
    /// @param[in] module pointer reference to the Module object.
    /// @param[in] name DescSetBinding name, defaults to "input_tensor_ptr".
    /// @return a reference to an instance of the Tensor class
    static Tensor CreateInput(const DataType& dataType,
                              const TensorShape& shape,
                              ModulePtrRef module,
                              const std::string& name = "input_tensor_ptr");

    /// Creates an Input Tensor, adds a DescSetBinding with provided binding index & assigns it to the Input tensor.
    /// @param[in] dataType Datatype that tensor values are.
    /// @param[in] shape Shape of the Tensor. If Tensor shape is empty then the shape will be set to 1 automatically.
    /// @param[in] module pointer reference to the Module object.
    /// @param[in] nameBindingPair std::pair containing DescSetBinding name and binding index.
    /// @return an instance of the Tensor class
    static Tensor CreateInput(const DataType& dataType,
                              const TensorShape& shape,
                              ModulePtrRef module,
                              const std::pair<const std::string, uint32_t>& nameBindingPair);

    /// Creates an Output Tensor and automatically adds a DescSetBinding and assigns it to the Output tensor.
    /// @param[in] dataType Datatype that tensor values are.
    /// @param[in] shape Shape of the Tensor. If Tensor shape is empty then the shape will be set to 1 automatically.
    /// @param[in] module pointer reference to the Module object.
    /// @param[in] name DescSetBinding name, defaults to "output_tensor_ptr".
    /// @return an instance of the Tensor class
    static Tensor  CreateOutput(const DataType& dataType,
                                const TensorShape& shape,
                                ModulePtrRef module,
                                const std::string& name =  "output_tensor_ptr");

    /// Creates an Output Tensor, adds a DescSetBinding with provided binding index & assigns it to the Output tensor.
    /// @param[in] dataType Datatype that tensor values are.
    /// @param[in] shape Shape of the Tensor. If Tensor shape is empty then the shape will be set to 1 automatically.
    /// @param[in] module pointer reference to the Module object.
    /// @param[in] nameBindingPair std::pair containing DescSetBinding name and binding index.
    /// @return an instance of the Tensor class
    static Tensor  CreateOutput(const DataType& dataType,
                                const TensorShape& shape,
                                ModulePtrRef module,
                                const std::pair<const std::string, uint32_t>& nameBindingPair);

    /// Creates an Input Tensor that is coming from a previous layer
    /// @param[in] dataType Datatype that tensor values are.
    /// @param[in] shape Shape of the Tensor. If Tensor shape is empty then the shape will be set to 1 automatically.
    /// @param[in] id ResId of the Tensor which may have been the return value of a call to Graph::AddXXXLayer();
    /// @return an instance of the Tensor class
    static Tensor CreateInputFromLayer(const DataType& dataType, const TensorShape& shape, const ResId& id);

    /// Creates an Output Tensor
    /// @param[in] dataType Datatype that tensor values are.
    /// @param[in] shape Shape of the Tensor. If Tensor shape is empty then the shape will be set to 1 automatically.
    /// @return an instance of the Tensor class
    static Tensor CreateOutputToLayer(const DataType& dataType, const TensorShape& shape);

    DescSetBinding* GetDescSetBinding() const;

    void SetDescSetBinding(DescSetBinding& pair);

    /// Getter for DataType.
    /// @return DataType from internal TensorInfo;
    const DataType& GetDataType() const;

    /// Getter for TensorShape.
    /// @return TensorShape, which is a vector of unsigned integers, from internal TensorInfo;
    const TensorShape& GetTensorShape() const;

    /// Getter for ResId.
    /// @return ResId from internal TensorInfo;
    const ResId& GetResId() const;

    /// Getter for IsGraphOutput.
    /// @return bool, if the tensor is to be used in a graph output.
    bool IsGraphOutput() const;

    /// Getter for Tensor Elements
    /// @return unsigned int for number of elements in Tensor
    unsigned int GetNumElements() const;

    /// Getter for Tensor values which are stored as uint32_t which is how they are written to SPIR-V.
    /// @return Vector of uint32_t values.
    std::vector<uint32_t> GetTensorValuesUint32t() const;

    /// Helper method used to convert users int32_t values into uint32_t for writing to SPIR-V Binary.
    /// @param[in] values const std::vector<int32_t>& that will be cast to uint32_t.
    /// @param[in] signless bool which sets whether the values will be output as signless, defaulted to true.
    /// @return Vector of uint32_t values that can be passed to constructor of Tensor.
    static std::vector<uint32_t> ConvertInt32tToUint32t(const std::vector<int32_t>& values, bool signless = true);

    /// Helper method used to convert users int16_t values into uint32_t for writing to SPIR-V Binary.
    /// @param[in] values const std::vector<int32_t>& that will be cast to uint32_t.
    /// @param[in] signless bool which sets whether the values will be output as signless, defaulted to true
    /// @return Vector of uint32_t values that can be passed to constructor of Tensor.
    static std::vector<uint32_t> ConvertInt16tToUint32t(const std::vector<int16_t>& values, bool signless = true);

    /// Helper method used to convert users int8_t values into uint32_t for writing to SPIR-V Binary.
    /// @param[in] values const std::vector<int32_t>& that will be cast to uint32_t.
    /// @param[in] signless bool which sets whether the values will be output as signless, defaulted to true
    /// @return Vector of uint32_t values that can be passed to constructor of Tensor.
    static std::vector<uint32_t> ConvertInt8tToUint32t(const std::vector<int8_t>& values, bool signless = true);

    /// Helper method used to convert users float32 values into uint32_t for writing to SPIR-V Binary.
    /// @param[in] values const std::vector<float>& that will be cast to uint32_t.
    /// @return Vector of uint32_t values that can be passed to constructor of Tensor.
    static std::vector<uint32_t> ConvertFloat32ToUint32t(const std::vector<float>& values);

    /// Helper method used to convert users float16 values into uint32_t for writing to SPIR-V Binary.
    /// @param[in] values const std::vector<float>& that will be cast to uint32_t.
    /// @return Vector of uint32_t values that can be passed to constructor of Tensor.
    static std::vector<uint32_t> ConvertFloat16ToUint32t(const std::vector<float>& values);

    /// Helper method used to convert users bool values into uint32_t for writing to SPIR-V Binary.
    /// @param[in] values const std::vector<bool>& that will be cast to uint32_t.
    /// @return Vector of uint32_t values that can be passed to constructor of Tensor.
    static std::vector<uint32_t> ConvertBoolToUint32t(const std::vector<bool>& values);

    /// Check if Tensor has values stored in TensorValuePtr
    /// @return bool, if the TensorValuePtr is not nullptr.
    bool HasValue() const;

    /// Get the category of the tensor
    /// @return Category
    Category GetCategory() const;

    /// Set the category of the tensor
    /// @param category
    void SetCategory(Category category);

    /// Get the metadata of the tensor
    /// @return Metadata
    Metadata GetMetadata() const;

    /// Set the metadata of the tensor
    /// @param metadata
    void SetMetadata(Metadata metadata);

    /// Default Destructor.
    ~Tensor();

private:

    /// Helper method used to convert users templated values into uint32_t for writing to SPIR-V Binary.
    /// @param[in] values const std::vector<T>& that will be cast to uint32_t.
    /// @param[in] signless bool which sets whether the values will be output as signless, defaulted to true
    /// @return Vector of uint32_t values that can be passed to constructor of Tensor.
    template<typename T>
    static std::vector<uint32_t> ConvertValuesToUint32t(const std::vector<T>& values,
                                                        DataType originalDatatype,
                                                        bool signless);

    /// Pointer to TensorInfo, hiding implementation in order to maintain stability.
    std::unique_ptr<TensorInfo> m_TensorInfo;

    /// Pointer to Tensor Values, these values are stored as uint32_t which is how they are written to SPIR-V Binary.
    std::vector<uint32_t> m_TensorAttributes;
};

} // namespace graphbuilder

} // namespace tosa2spirv
