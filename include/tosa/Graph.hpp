//
// Copyright © 2023-2025 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include "OperatorEnum.hpp"
#include "Tensor.hpp"

#include <memory>
#include <string>

namespace tosa2spirv::spirv
{
class Module;
}

namespace tosa2spirv::tosa
{

/// Class which encapsulates the concept of a graph containing layers.
class Graph
{
    public:
    /// Module::AddGraph() returns a shared pointer to the graph for the user to then make calls to AddXXXOperator().
    /// @param module
    /// @param[in] name std::string that will be used as the name of the graph.
    explicit Graph(std::shared_ptr<spirv::Module> module, std::string name = std::string());

    /// Add an input to the graph, specifying the binding id (Descriptor Set will be 0)
    /// @param[in] input Tensor describing the input
    /// @param[in] bindingId binding id
    /// @return ResId of input
    /// Most state in tosa2spirv is represented as SPIRV instructions and operands
    /// ResIds are pointers to the SPIRV instructions representing inputs, outputs, operators and constants
    ResId AddInput(const Tensor& input, unsigned int bindingId);

    /// Add an output to the graph, specifying its Descriptor Set Binding (Descriptor Set will be 0)
    /// @param[in] outputTensor ResId returned from an AddXXXOperator call which is the operators output
    /// @param[in] bindingId unsigned int representing the Descriptor Set Binding
    void AddOutput(ResId outputTensor, unsigned int bindingId);

    /// Add a tensor to the graph describing constant data, external to the spirv binary
    /// Used for large constants, > 16 elements
    /// @param[in] tensor describing the constant.
    /// @return A ResId of the GraphConstant
    ResId AddGraphConstant(const Tensor& tensor);

    // Depreciated AddGraphConstant
    ResId AddExternalGraphConstant(const Tensor& tensor);

    /// Add a small constant to the graph
    /// Used for constants <= 16 elements
    /// @param[in] attribute describing the constant.
    /// @return A ResId of the GraphConstant
    ResId AddTensorConstant(const Attribute& attribute);

    /// General function to add an operator,
    /// All subsequent AddOperator functions are specialized wrappers of this function.
    /// @param[in] operatorType The operator type enum.
    /// @param[in] inputs       ResIds representing the operator's input values.
    /// @param[in] outputs      Output tensors produced by the operator.
    /// @param[in] attributes   Optional attributes specific to the operator.
    /// @return             The ResIds corresponding to the output values of the operator.
    std::vector<ResId> AddOperator(OperatorEnum operatorType,
                                   const std::vector<ResId>& inputs,
                                   const std::vector<Tensor>& outputs,
                                   const std::vector<Attribute>& attributes = {}) const;

    /// Once all Operators and IO are set, this function creates the spirv instructions describing the graph
    /// After being called no more changes may be made to this graph.
    void FinalizeGraph();

    // See tosa2spirv/python/source_generator.py and README
    // THIS SECTION IS GENERATED WITH TOSA 0.80.0. DO NOT EDIT!
    // GRAPH OPERATOR HELPER FUNCTION BEGIN
    /// Function used to add a Argmax operator to the Graph
    /// @param[in] input ResId representing the input to the Argmax Operator
    /// @param[in] axis Attribute representing the axis to the Argmax Operator
    /// @param[in] output Tensor representing the output to the Argmax Operator
    ResId AddArgmaxOperator(const ResId& input, const Attribute& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Argmax, {input}, {output}, {axis})[0];
    }

    /// Function used to add a AvgPool2d operator to the Graph
    /// @param[in] input ResId representing the input to the AvgPool2d Operator
    /// @param[in] kernel Attribute representing the kernel to the AvgPool2d Operator
    /// @param[in] stride Attribute representing the stride to the AvgPool2d Operator
    /// @param[in] pad Attribute representing the pad to the AvgPool2d Operator
    /// @param[in] acc_size Attribute representing the acc_size to the AvgPool2d Operator
    /// @param[in] input_zp Attribute representing the input_zp to the AvgPool2d Operator
    /// @param[in] output_zp Attribute representing the output_zp to the AvgPool2d Operator
    /// @param[in] output Tensor representing the output to the AvgPool2d Operator
    ResId AddAvgPool2dOperator(const ResId& input,
                               const Attribute& kernel,
                               const Attribute& stride,
                               const Attribute& pad,
                               const Attribute& acc_size,
                               const Attribute& input_zp,
                               const Attribute& output_zp,
                               const Tensor& output)
    {
        return AddOperator(OperatorEnum::AvgPool2d,
                           {input},
                           {output},
                           {kernel, stride, pad, acc_size, input_zp, output_zp})[0];
    }

    /// Function used to add a Conv2d operator to the Graph
    /// @param[in] input ResId representing the input to the Conv2d Operator
    /// @param[in] weight ResId representing the weight to the Conv2d Operator
    /// @param[in] bias ResId representing the bias to the Conv2d Operator
    /// @param[in] pad Attribute representing the pad to the Conv2d Operator
    /// @param[in] stride Attribute representing the stride to the Conv2d Operator
    /// @param[in] dilation Attribute representing the dilation to the Conv2d Operator
    /// @param[in] input_zp Attribute representing the input_zp to the Conv2d Operator
    /// @param[in] weight_zp Attribute representing the weight_zp to the Conv2d Operator
    /// @param[in] local_bound Attribute representing the local_bound to the Conv2d Operator
    /// @param[in] output Tensor representing the output to the Conv2d Operator
    ResId AddConv2dOperator(const ResId& input,
                            const ResId& weight,
                            const ResId& bias,
                            const Attribute& pad,
                            const Attribute& stride,
                            const Attribute& dilation,
                            const Attribute& input_zp,
                            const Attribute& weight_zp,
                            const Attribute& local_bound,
                            const Tensor& output)
    {
        return AddOperator(OperatorEnum::Conv2d,
                           {input, weight, bias},
                           {output},
                           {pad, stride, dilation, input_zp, weight_zp, local_bound})[0];
    }

    /// Function used to add a Conv3d operator to the Graph
    /// @param[in] input ResId representing the input to the Conv3d Operator
    /// @param[in] weight ResId representing the weight to the Conv3d Operator
    /// @param[in] bias ResId representing the bias to the Conv3d Operator
    /// @param[in] pad Attribute representing the pad to the Conv3d Operator
    /// @param[in] stride Attribute representing the stride to the Conv3d Operator
    /// @param[in] dilation Attribute representing the dilation to the Conv3d Operator
    /// @param[in] input_zp Attribute representing the input_zp to the Conv3d Operator
    /// @param[in] weight_zp Attribute representing the weight_zp to the Conv3d Operator
    /// @param[in] local_bound Attribute representing the local_bound to the Conv3d Operator
    /// @param[in] output Tensor representing the output to the Conv3d Operator
    ResId AddConv3dOperator(const ResId& input,
                            const ResId& weight,
                            const ResId& bias,
                            const Attribute& pad,
                            const Attribute& stride,
                            const Attribute& dilation,
                            const Attribute& input_zp,
                            const Attribute& weight_zp,
                            const Attribute& local_bound,
                            const Tensor& output)
    {
        return AddOperator(OperatorEnum::Conv3d,
                           {input, weight, bias},
                           {output},
                           {pad, stride, dilation, input_zp, weight_zp, local_bound})[0];
    }

    /// Function used to add a DepthwiseConv2d operator to the Graph
    /// @param[in] input ResId representing the input to the DepthwiseConv2d Operator
    /// @param[in] weight ResId representing the weight to the DepthwiseConv2d Operator
    /// @param[in] bias ResId representing the bias to the DepthwiseConv2d Operator
    /// @param[in] pad Attribute representing the pad to the DepthwiseConv2d Operator
    /// @param[in] stride Attribute representing the stride to the DepthwiseConv2d Operator
    /// @param[in] dilation Attribute representing the dilation to the DepthwiseConv2d Operator
    /// @param[in] input_zp Attribute representing the input_zp to the DepthwiseConv2d Operator
    /// @param[in] weight_zp Attribute representing the weight_zp to the DepthwiseConv2d Operator
    /// @param[in] local_bound Attribute representing the local_bound to the DepthwiseConv2d Operator
    /// @param[in] output Tensor representing the output to the DepthwiseConv2d Operator
    ResId AddDepthwiseConv2dOperator(const ResId& input,
                                     const ResId& weight,
                                     const ResId& bias,
                                     const Attribute& pad,
                                     const Attribute& stride,
                                     const Attribute& dilation,
                                     const Attribute& input_zp,
                                     const Attribute& weight_zp,
                                     const Attribute& local_bound,
                                     const Tensor& output)
    {
        return AddOperator(OperatorEnum::DepthwiseConv2d,
                           {input, weight, bias},
                           {output},
                           {pad, stride, dilation, input_zp, weight_zp, local_bound})[0];
    }

    /// Function used to add a Fft2d operator to the Graph
    /// @param[in] input_real ResId representing the input_real to the Fft2d Operator
    /// @param[in] input_imag ResId representing the input_imag to the Fft2d Operator
    /// @param[in] inverse Attribute representing the inverse to the Fft2d Operator
    /// @param[in] output_real Tensor representing the output_real to the Fft2d Operator
    /// @param[in] local_bound Attribute representing the local_bound to the Fft2d Operator
    /// @param[in] output_imag Tensor representing the output_imag to the Fft2d Operator
    std::vector<ResId> AddFft2dOperator(const ResId& input_real,
                                        const ResId& input_imag,
                                        const Attribute& inverse,
                                        const Tensor& output_real,
                                        const Attribute& local_bound,
                                        const Tensor& output_imag)
    {
        return AddOperator(OperatorEnum::Fft2d,
                           {input_real, input_imag},
                           {output_real, output_imag},
                           {inverse, local_bound});
    }

    /// Function used to add a FullyConnected operator to the Graph
    /// @param[in] input ResId representing the input to the FullyConnected Operator
    /// @param[in] weight ResId representing the weight to the FullyConnected Operator
    /// @param[in] bias ResId representing the bias to the FullyConnected Operator
    /// @param[in] input_zp Attribute representing the input_zp to the FullyConnected Operator
    /// @param[in] weight_zp Attribute representing the weight_zp to the FullyConnected Operator
    /// @param[in] output Tensor representing the output to the FullyConnected Operator
    ResId AddFullyConnectedOperator(const ResId& input,
                                    const ResId& weight,
                                    const ResId& bias,
                                    const Attribute& input_zp,
                                    const Attribute& weight_zp,
                                    const Tensor& output)
    {
        return AddOperator(OperatorEnum::FullyConnected, {input, weight, bias}, {output}, {input_zp, weight_zp})[0];
    }

    /// Function used to add a Matmul operator to the Graph
    /// @param[in] A ResId representing the A to the Matmul Operator
    /// @param[in] B ResId representing the B to the Matmul Operator
    /// @param[in] A_zp Attribute representing the A_zp to the Matmul Operator
    /// @param[in] B_zp Attribute representing the B_zp to the Matmul Operator
    /// @param[in] output Tensor representing the output to the Matmul Operator
    ResId AddMatmulOperator(const ResId& A,
                            const ResId& B,
                            const Attribute& A_zp,
                            const Attribute& B_zp,
                            const Tensor& output)
    {
        return AddOperator(OperatorEnum::Matmul, {A, B}, {output}, {A_zp, B_zp})[0];
    }

    /// Function used to add a MaxPool2d operator to the Graph
    /// @param[in] input ResId representing the input to the MaxPool2d Operator
    /// @param[in] kernel Attribute representing the kernel to the MaxPool2d Operator
    /// @param[in] stride Attribute representing the stride to the MaxPool2d Operator
    /// @param[in] pad Attribute representing the pad to the MaxPool2d Operator
    /// @param[in] output Tensor representing the output to the MaxPool2d Operator
    ResId AddMaxPool2dOperator(const ResId& input,
                               const Attribute& kernel,
                               const Attribute& stride,
                               const Attribute& pad,
                               const Tensor& output)
    {
        return AddOperator(OperatorEnum::MaxPool2d, {input}, {output}, {kernel, stride, pad})[0];
    }

    /// Function used to add a Rfft2d operator to the Graph
    /// @param[in] input ResId representing the input to the Rfft2d Operator
    /// @param[in] output_real Tensor representing the output_real to the Rfft2d Operator
    /// @param[in] output_imag Tensor representing the output_imag to the Rfft2d Operator
    std::vector<ResId> AddRfft2dOperator(const ResId& input, const Tensor& output_real, const Tensor& output_imag)
    {
        return AddOperator(OperatorEnum::Rfft2d, {input}, {output_real, output_imag});
    }

    /// Function used to add a TransposeConv2d operator to the Graph
    /// @param[in] input ResId representing the input to the TransposeConv2d Operator
    /// @param[in] weight ResId representing the weight to the TransposeConv2d Operator
    /// @param[in] bias ResId representing the bias to the TransposeConv2d Operator
    /// @param[in] out_pad Attribute representing the out_pad to the TransposeConv2d Operator
    /// @param[in] stride Attribute representing the stride to the TransposeConv2d Operator
    /// @param[in] out_shape Attribute representing the out_shape to the TransposeConv2d Operator
    /// @param[in] input_zp Attribute representing the input_zp to the TransposeConv2d Operator
    /// @param[in] weight_zp Attribute representing the weight_zp to the TransposeConv2d Operator
    /// @param[in] local_bound Attribute representing the local_bound to the TransposeConv2d Operator
    /// @param[in] output Tensor representing the output to the TransposeConv2d Operator
    ResId AddTransposeConv2dOperator(const ResId& input,
                                     const ResId& weight,
                                     const ResId& bias,
                                     const Attribute& out_pad,
                                     const Attribute& stride,
                                     const Attribute& out_shape,
                                     const Attribute& input_zp,
                                     const Attribute& weight_zp,
                                     const Attribute& local_bound,
                                     const Tensor& output)
    {
        return AddOperator(OperatorEnum::TransposeConv2d,
                           {input, weight, bias},
                           {output},
                           {out_pad, stride, out_shape, input_zp, weight_zp, local_bound})[0];
    }

    /// Function used to add a Clamp operator to the Graph
    /// @param[in] input ResId representing the input to the Clamp Operator
    /// @param[in] min_val Attribute representing the min_val to the Clamp Operator
    /// @param[in] max_val Attribute representing the max_val to the Clamp Operator
    /// @param[in] output Tensor representing the output to the Clamp Operator
    ResId AddClampOperator(const ResId& input, const Attribute& min_val, const Attribute& max_val, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Clamp, {input}, {output}, {min_val, max_val})[0];
    }

    /// Function used to add a Erf operator to the Graph
    /// @param[in] input ResId representing the input to the Erf Operator
    /// @param[in] output Tensor representing the output to the Erf Operator
    ResId AddErfOperator(const ResId& input, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Erf, {input}, {output})[0];
    }

    /// Function used to add a Sigmoid operator to the Graph
    /// @param[in] input ResId representing the input to the Sigmoid Operator
    /// @param[in] output Tensor representing the output to the Sigmoid Operator
    ResId AddSigmoidOperator(const ResId& input, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Sigmoid, {input}, {output})[0];
    }

    /// Function used to add a Tanh operator to the Graph
    /// @param[in] input ResId representing the input to the Tanh Operator
    /// @param[in] output Tensor representing the output to the Tanh Operator
    ResId AddTanhOperator(const ResId& input, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Tanh, {input}, {output})[0];
    }

    /// Function used to add a Add operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Add Operator
    /// @param[in] input2 ResId representing the input2 to the Add Operator
    /// @param[in] output Tensor representing the output to the Add Operator
    ResId AddAddOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Add, {input1, input2}, {output})[0];
    }

    /// Function used to add a ArithmeticRightShift operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the ArithmeticRightShift Operator
    /// @param[in] input2 ResId representing the input2 to the ArithmeticRightShift Operator
    /// @param[in] round Attribute representing the round to the ArithmeticRightShift Operator
    /// @param[in] output Tensor representing the output to the ArithmeticRightShift Operator
    ResId AddArithmeticRightShiftOperator(const ResId& input1,
                                          const ResId& input2,
                                          const Attribute& round,
                                          const Tensor& output)
    {
        return AddOperator(OperatorEnum::ArithmeticRightShift, {input1, input2}, {output}, {round})[0];
    }

    /// Function used to add a BitwiseAnd operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the BitwiseAnd Operator
    /// @param[in] input2 ResId representing the input2 to the BitwiseAnd Operator
    /// @param[in] output Tensor representing the output to the BitwiseAnd Operator
    ResId AddBitwiseAndOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::BitwiseAnd, {input1, input2}, {output})[0];
    }

    /// Function used to add a BitwiseOr operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the BitwiseOr Operator
    /// @param[in] input2 ResId representing the input2 to the BitwiseOr Operator
    /// @param[in] output Tensor representing the output to the BitwiseOr Operator
    ResId AddBitwiseOrOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::BitwiseOr, {input1, input2}, {output})[0];
    }

    /// Function used to add a BitwiseXor operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the BitwiseXor Operator
    /// @param[in] input2 ResId representing the input2 to the BitwiseXor Operator
    /// @param[in] output Tensor representing the output to the BitwiseXor Operator
    ResId AddBitwiseXorOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::BitwiseXor, {input1, input2}, {output})[0];
    }

    /// Function used to add a Intdiv operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Intdiv Operator
    /// @param[in] input2 ResId representing the input2 to the Intdiv Operator
    /// @param[in] output Tensor representing the output to the Intdiv Operator
    ResId AddIntdivOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Intdiv, {input1, input2}, {output})[0];
    }

    /// Function used to add a LogicalAnd operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the LogicalAnd Operator
    /// @param[in] input2 ResId representing the input2 to the LogicalAnd Operator
    /// @param[in] output Tensor representing the output to the LogicalAnd Operator
    ResId AddLogicalAndOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::LogicalAnd, {input1, input2}, {output})[0];
    }

    /// Function used to add a LogicalLeftShift operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the LogicalLeftShift Operator
    /// @param[in] input2 ResId representing the input2 to the LogicalLeftShift Operator
    /// @param[in] output Tensor representing the output to the LogicalLeftShift Operator
    ResId AddLogicalLeftShiftOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::LogicalLeftShift, {input1, input2}, {output})[0];
    }

    /// Function used to add a LogicalRightShift operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the LogicalRightShift Operator
    /// @param[in] input2 ResId representing the input2 to the LogicalRightShift Operator
    /// @param[in] output Tensor representing the output to the LogicalRightShift Operator
    ResId AddLogicalRightShiftOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::LogicalRightShift, {input1, input2}, {output})[0];
    }

    /// Function used to add a LogicalOr operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the LogicalOr Operator
    /// @param[in] input2 ResId representing the input2 to the LogicalOr Operator
    /// @param[in] output Tensor representing the output to the LogicalOr Operator
    ResId AddLogicalOrOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::LogicalOr, {input1, input2}, {output})[0];
    }

    /// Function used to add a LogicalXor operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the LogicalXor Operator
    /// @param[in] input2 ResId representing the input2 to the LogicalXor Operator
    /// @param[in] output Tensor representing the output to the LogicalXor Operator
    ResId AddLogicalXorOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::LogicalXor, {input1, input2}, {output})[0];
    }

    /// Function used to add a Maximum operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Maximum Operator
    /// @param[in] input2 ResId representing the input2 to the Maximum Operator
    /// @param[in] output Tensor representing the output to the Maximum Operator
    ResId AddMaximumOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Maximum, {input1, input2}, {output})[0];
    }

    /// Function used to add a Minimum operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Minimum Operator
    /// @param[in] input2 ResId representing the input2 to the Minimum Operator
    /// @param[in] output Tensor representing the output to the Minimum Operator
    ResId AddMinimumOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Minimum, {input1, input2}, {output})[0];
    }

    /// Function used to add a Mul operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Mul Operator
    /// @param[in] input2 ResId representing the input2 to the Mul Operator
    /// @param[in] shift Attribute representing the shift to the Mul Operator
    /// @param[in] output Tensor representing the output to the Mul Operator
    ResId AddMulOperator(const ResId& input1, const ResId& input2, const Attribute& shift, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Mul, {input1, input2}, {output}, {shift})[0];
    }

    /// Function used to add a Pow operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Pow Operator
    /// @param[in] input2 ResId representing the input2 to the Pow Operator
    /// @param[in] output Tensor representing the output to the Pow Operator
    ResId AddPowOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Pow, {input1, input2}, {output})[0];
    }

    /// Function used to add a Sub operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Sub Operator
    /// @param[in] input2 ResId representing the input2 to the Sub Operator
    /// @param[in] output Tensor representing the output to the Sub Operator
    ResId AddSubOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Sub, {input1, input2}, {output})[0];
    }

    /// Function used to add a Table operator to the Graph
    /// @param[in] input ResId representing the input to the Table Operator
    /// @param[in] table Attribute representing the table to the Table Operator
    /// @param[in] output Tensor representing the output to the Table Operator
    ResId AddTableOperator(const ResId& input, const Attribute& table, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Table, {input}, {output}, {table})[0];
    }

    /// Function used to add a Abs operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Abs Operator
    /// @param[in] output Tensor representing the output to the Abs Operator
    ResId AddAbsOperator(const ResId& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Abs, {input1}, {output})[0];
    }

    /// Function used to add a BitwiseNot operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the BitwiseNot Operator
    /// @param[in] output Tensor representing the output to the BitwiseNot Operator
    ResId AddBitwiseNotOperator(const ResId& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::BitwiseNot, {input1}, {output})[0];
    }

    /// Function used to add a Ceil operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Ceil Operator
    /// @param[in] output Tensor representing the output to the Ceil Operator
    ResId AddCeilOperator(const ResId& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Ceil, {input1}, {output})[0];
    }

    /// Function used to add a Clz operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Clz Operator
    /// @param[in] output Tensor representing the output to the Clz Operator
    ResId AddClzOperator(const ResId& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Clz, {input1}, {output})[0];
    }

    /// Function used to add a Exp operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Exp Operator
    /// @param[in] output Tensor representing the output to the Exp Operator
    ResId AddExpOperator(const ResId& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Exp, {input1}, {output})[0];
    }

    /// Function used to add a Floor operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Floor Operator
    /// @param[in] output Tensor representing the output to the Floor Operator
    ResId AddFloorOperator(const ResId& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Floor, {input1}, {output})[0];
    }

    /// Function used to add a Log operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Log Operator
    /// @param[in] output Tensor representing the output to the Log Operator
    ResId AddLogOperator(const ResId& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Log, {input1}, {output})[0];
    }

    /// Function used to add a LogicalNot operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the LogicalNot Operator
    /// @param[in] output Tensor representing the output to the LogicalNot Operator
    ResId AddLogicalNotOperator(const ResId& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::LogicalNot, {input1}, {output})[0];
    }

    /// Function used to add a Negate operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Negate Operator
    /// @param[in] input1_zp Attribute representing the input1_zp to the Negate Operator
    /// @param[in] output_zp Attribute representing the output_zp to the Negate Operator
    /// @param[in] output Tensor representing the output to the Negate Operator
    ResId
    AddNegateOperator(const ResId& input1, const Attribute& input1_zp, const Attribute& output_zp, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Negate, {input1}, {output}, {input1_zp, output_zp})[0];
    }

    /// Function used to add a Reciprocal operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Reciprocal Operator
    /// @param[in] output Tensor representing the output to the Reciprocal Operator
    ResId AddReciprocalOperator(const ResId& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Reciprocal, {input1}, {output})[0];
    }

    /// Function used to add a Rsqrt operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Rsqrt Operator
    /// @param[in] output Tensor representing the output to the Rsqrt Operator
    ResId AddRsqrtOperator(const ResId& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Rsqrt, {input1}, {output})[0];
    }

    /// Function used to add a Select operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Select Operator
    /// @param[in] input2 ResId representing the input2 to the Select Operator
    /// @param[in] input3 ResId representing the input3 to the Select Operator
    /// @param[in] output Tensor representing the output to the Select Operator
    ResId AddSelectOperator(const ResId& input1, const ResId& input2, const ResId& input3, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Select, {input1, input2, input3}, {output})[0];
    }

    /// Function used to add a Equal operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Equal Operator
    /// @param[in] input2 ResId representing the input2 to the Equal Operator
    /// @param[in] output Tensor representing the output to the Equal Operator
    ResId AddEqualOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Equal, {input1, input2}, {output})[0];
    }

    /// Function used to add a Greater operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Greater Operator
    /// @param[in] input2 ResId representing the input2 to the Greater Operator
    /// @param[in] output Tensor representing the output to the Greater Operator
    ResId AddGreaterOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Greater, {input1, input2}, {output})[0];
    }

    /// Function used to add a GreaterEqual operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the GreaterEqual Operator
    /// @param[in] input2 ResId representing the input2 to the GreaterEqual Operator
    /// @param[in] output Tensor representing the output to the GreaterEqual Operator
    ResId AddGreaterEqualOperator(const ResId& input1, const ResId& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::GreaterEqual, {input1, input2}, {output})[0];
    }

    /// Function used to add a ReduceAll operator to the Graph
    /// @param[in] input ResId representing the input to the ReduceAll Operator
    /// @param[in] axis Attribute representing the axis to the ReduceAll Operator
    /// @param[in] output Tensor representing the output to the ReduceAll Operator
    ResId AddReduceAllOperator(const ResId& input, const Attribute& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::ReduceAll, {input}, {output}, {axis})[0];
    }

    /// Function used to add a ReduceAny operator to the Graph
    /// @param[in] input ResId representing the input to the ReduceAny Operator
    /// @param[in] axis Attribute representing the axis to the ReduceAny Operator
    /// @param[in] output Tensor representing the output to the ReduceAny Operator
    ResId AddReduceAnyOperator(const ResId& input, const Attribute& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::ReduceAny, {input}, {output}, {axis})[0];
    }

    /// Function used to add a ReduceMax operator to the Graph
    /// @param[in] input ResId representing the input to the ReduceMax Operator
    /// @param[in] axis Attribute representing the axis to the ReduceMax Operator
    /// @param[in] output Tensor representing the output to the ReduceMax Operator
    ResId AddReduceMaxOperator(const ResId& input, const Attribute& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::ReduceMax, {input}, {output}, {axis})[0];
    }

    /// Function used to add a ReduceMin operator to the Graph
    /// @param[in] input ResId representing the input to the ReduceMin Operator
    /// @param[in] axis Attribute representing the axis to the ReduceMin Operator
    /// @param[in] output Tensor representing the output to the ReduceMin Operator
    ResId AddReduceMinOperator(const ResId& input, const Attribute& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::ReduceMin, {input}, {output}, {axis})[0];
    }

    /// Function used to add a ReduceProduct operator to the Graph
    /// @param[in] input ResId representing the input to the ReduceProduct Operator
    /// @param[in] axis Attribute representing the axis to the ReduceProduct Operator
    /// @param[in] output Tensor representing the output to the ReduceProduct Operator
    ResId AddReduceProductOperator(const ResId& input, const Attribute& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::ReduceProduct, {input}, {output}, {axis})[0];
    }

    /// Function used to add a ReduceSum operator to the Graph
    /// @param[in] input ResId representing the input to the ReduceSum Operator
    /// @param[in] axis Attribute representing the axis to the ReduceSum Operator
    /// @param[in] output Tensor representing the output to the ReduceSum Operator
    ResId AddReduceSumOperator(const ResId& input, const Attribute& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::ReduceSum, {input}, {output}, {axis})[0];
    }

    /// Function used to add a Concat operator to the Graph
    /// @param[in] input1 std::vector<ResId> representing the input1 to the Concat Operator
    /// @param[in] axis Attribute representing the axis to the Concat Operator
    /// @param[in] output Tensor representing the output to the Concat Operator
    ResId AddConcatOperator(const std::vector<ResId>& input1, const Attribute& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Concat, {input1}, {output}, {axis})[0];
    }

    /// Function used to add a Pad operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Pad Operator
    /// @param[in] padding Attribute representing the padding to the Pad Operator
    /// @param[in] pad_const Attribute representing the pad_const to the Pad Operator
    /// @param[in] output Tensor representing the output to the Pad Operator
    ResId
    AddPadOperator(const ResId& input1, const Attribute& padding, const Attribute& pad_const, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Pad, {input1}, {output}, {padding, pad_const})[0];
    }

    /// Function used to add a Reshape operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Reshape Operator
    /// @param[in] shape Attribute representing the shape to the Reshape Operator
    /// @param[in] output Tensor representing the output to the Reshape Operator
    ResId AddReshapeOperator(const ResId& input1, const Attribute& shape, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Reshape, {input1}, {output}, {shape})[0];
    }

    /// Function used to add a Reverse operator to the Graph
    /// @param[in] input ResId representing the input to the Reverse Operator
    /// @param[in] axis Attribute representing the axis to the Reverse Operator
    /// @param[in] output Tensor representing the output to the Reverse Operator
    ResId AddReverseOperator(const ResId& input, const Attribute& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Reverse, {input}, {output}, {axis})[0];
    }

    /// Function used to add a Slice operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Slice Operator
    /// @param[in] start Attribute representing the start to the Slice Operator
    /// @param[in] size Attribute representing the size to the Slice Operator
    /// @param[in] output Tensor representing the output to the Slice Operator
    ResId AddSliceOperator(const ResId& input1, const Attribute& start, const Attribute& size, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Slice, {input1}, {output}, {start, size})[0];
    }

    /// Function used to add a Tile operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Tile Operator
    /// @param[in] multiples Attribute representing the multiples to the Tile Operator
    /// @param[in] output Tensor representing the output to the Tile Operator
    ResId AddTileOperator(const ResId& input1, const Attribute& multiples, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Tile, {input1}, {output}, {multiples})[0];
    }

    /// Function used to add a Transpose operator to the Graph
    /// @param[in] input1 ResId representing the input1 to the Transpose Operator
    /// @param[in] perms Attribute representing the perms to the Transpose Operator
    /// @param[in] output Tensor representing the output to the Transpose Operator
    ResId AddTransposeOperator(const ResId& input1, const Attribute& perms, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Transpose, {input1}, {output}, {perms})[0];
    }

    /// Function used to add a Gather operator to the Graph
    /// @param[in] values ResId representing the values to the Gather Operator
    /// @param[in] indices ResId representing the indices to the Gather Operator
    /// @param[in] output Tensor representing the output to the Gather Operator
    ResId AddGatherOperator(const ResId& values, const ResId& indices, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Gather, {values, indices}, {output})[0];
    }

    /// Function used to add a Scatter operator to the Graph
    /// @param[in] values_in ResId representing the values_in to the Scatter Operator
    /// @param[in] indices ResId representing the indices to the Scatter Operator
    /// @param[in] input ResId representing the input to the Scatter Operator
    /// @param[in] values_out Tensor representing the values_out to the Scatter Operator
    ResId AddScatterOperator(const ResId& values_in, const ResId& indices, const ResId& input, const Tensor& values_out)
    {
        return AddOperator(OperatorEnum::Scatter, {values_in, indices, input}, {values_out})[0];
    }

    /// Function used to add a Resize operator to the Graph
    /// @param[in] input ResId representing the input to the Resize Operator
    /// @param[in] scale Attribute representing the scale to the Resize Operator
    /// @param[in] offset Attribute representing the offset to the Resize Operator
    /// @param[in] border Attribute representing the border to the Resize Operator
    /// @param[in] mode Attribute representing the mode to the Resize Operator
    /// @param[in] output Tensor representing the output to the Resize Operator
    ResId AddResizeOperator(const ResId& input,
                            const Attribute& scale,
                            const Attribute& offset,
                            const Attribute& border,
                            const Attribute& mode,
                            const Tensor& output)
    {
        return AddOperator(OperatorEnum::Resize, {input}, {output}, {scale, offset, border, mode})[0];
    }

    /// Function used to add a Cast operator to the Graph
    /// @param[in] input ResId representing the input to the Cast Operator
    /// @param[in] output Tensor representing the output to the Cast Operator
    ResId AddCastOperator(const ResId& input, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Cast, {input}, {output})[0];
    }

    /// Function used to add a Rescale operator to the Graph
    /// @param[in] input ResId representing the input to the Rescale Operator
    /// @param[in] output Tensor representing the output to the Rescale Operator
    /// @param[in] input_zp Attribute representing the input_zp to the Rescale Operator
    /// @param[in] output_zp Attribute representing the output_zp to the Rescale Operator
    /// @param[in] multiplier Attribute representing the multiplier to the Rescale Operator
    /// @param[in] shift Attribute representing the shift to the Rescale Operator
    /// @param[in] scale32 Attribute representing the scale32 to the Rescale Operator
    /// @param[in] double_round Attribute representing the double_round to the Rescale Operator
    /// @param[in] per_channel Attribute representing the per_channel to the Rescale Operator
    /// @param[in] input_unsigned Attribute representing the input_unsigned to the Rescale Operator
    /// @param[in] output_unsigned Attribute representing the output_unsigned to the Rescale Operator
    ResId AddRescaleOperator(const ResId& input,
                             const Tensor& output,
                             const Attribute& input_zp,
                             const Attribute& output_zp,
                             const Attribute& multiplier,
                             const Attribute& shift,
                             const Attribute& scale32,
                             const Attribute& double_round,
                             const Attribute& per_channel,
                             const Attribute& input_unsigned,
                             const Attribute& output_unsigned)
    {
        return AddOperator(OperatorEnum::Rescale,
                           {input},
                           {output},
                           {input_zp,
                            output_zp,
                            multiplier,
                            shift,
                            scale32,
                            double_round,
                            per_channel,
                            input_unsigned,
                            output_unsigned})[0];
    }

    // GRAPH OPERATOR HELPER FUNCTION END
    std::string m_Name;

    private:
    std::shared_ptr<spirv::Module> m_Module;

    // Instructions built up over the lifetime of the graph, then inserted at lifetime end
    std::vector<const spirv::Instruction*> m_Inputs;
    std::vector<const spirv::Instruction*> m_Outputs;

    spirv::Instruction* m_TosaVersion;

    uint32_t m_GraphConstantId = 0;
};

} // namespace tosa2spirv::tosa
