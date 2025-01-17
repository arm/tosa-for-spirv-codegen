//
// Copyright © 2023-2024 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: Apache-2.0
//


#pragma once

#include <ResId.hpp>
#include <Module.hpp>
#include <Operator.hpp>
#include <Tensor.hpp>

namespace tosa2spirv
{

/// Forward declaration of internal spirvwriter namespace.
namespace spirvwriter
{

/// Forward declaration of internal Writer.
class Writer;

}

namespace graphbuilder
{
/// Forward declaration of internal GraphIOInfo and ILayer.
struct GraphIOInfo;

struct GraphInfo;
class IVisitor;

/// Class which encapsulates the concept of a graph containing layers.
class Graph : public IVisitable
{
public:
    /// Marking Module class as a friend so it can access private constructor and Write function.
    friend Module;

    std::vector<ResId> AddOperator(OperatorEnum operatorType,
                                   const std::vector<Tensor>& inputs,
                                   const std::vector<Tensor>& outputs,
                                   std::vector<Tensor> attributes = {});

    ResId GetNextResId() const
    {
        return m_IdGenerator.GetNextId();
    }

    /// Function used to override the order of outputs from a graph.
    /// By default, graph outputs will be added in the order that they occur in the graph.
    /// @param[in] outputResIds vector of ResIds returned from AddXXXLayer functions,
    ///                         which represents outputs of the graph.
    void SetGraphOutputs(const std::vector<ResId>& outputResIds);

    /// Accept function to allow for getting graph / layer information via a Visitor
    /// @param[in] visitor The visitor which will hold the graph information
    void Accept(IVisitor& visitor) const override;

    /// Function used to return a vector of all the operators in the Graph
    std::vector<Operator> GetOperators() const { return m_Operators; }

    /// Default destructor
    ~Graph() override = default;

// See tosa2spirv/python/source_generator.py and README
// THIS SECTION IS GENERATED WITH TOSA 0.80.0. DO NOT EDIT!
    /// Function used to add a Argmax operator to the Graph
    /// @param[in] input Tensor representing the input to the Argmax Operator
    /// @param[in] axis Tensor representing the axis to the Argmax Operator
    /// @param[in] output Tensor representing the output to the Argmax Operator
    ResId AddArgmaxOperator(const Tensor& input, const Tensor& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Argmax, {input}, {output}, {axis})[0];
    }

    /// Function used to add a AvgPool2d operator to the Graph
    /// @param[in] input Tensor representing the input to the AvgPool2d Operator
    /// @param[in] kernel Tensor representing the kernel to the AvgPool2d Operator
    /// @param[in] stride Tensor representing the stride to the AvgPool2d Operator
    /// @param[in] pad Tensor representing the pad to the AvgPool2d Operator
    /// @param[in] acc_size Tensor representing the acc_size to the AvgPool2d Operator
    /// @param[in] input_zp Tensor representing the input_zp to the AvgPool2d Operator
    /// @param[in] output_zp Tensor representing the output_zp to the AvgPool2d Operator
    /// @param[in] output Tensor representing the output to the AvgPool2d Operator
    ResId AddAvgPool2dOperator(const Tensor& input,
                               const Tensor& kernel,
                               const Tensor& stride,
                               const Tensor& pad,
                               const Tensor& acc_size,
                               const Tensor& input_zp,
                               const Tensor& output_zp,
                               const Tensor& output)
    {
        return AddOperator(OperatorEnum::AvgPool2d,
                           {input},
                           {output},
                           {kernel, stride, pad, acc_size, input_zp, output_zp})[0];
    }

    /// Function used to add a Conv2d operator to the Graph
    /// @param[in] input Tensor representing the input to the Conv2d Operator
    /// @param[in] weight Tensor representing the weight to the Conv2d Operator
    /// @param[in] bias Tensor representing the bias to the Conv2d Operator
    /// @param[in] pad Tensor representing the pad to the Conv2d Operator
    /// @param[in] stride Tensor representing the stride to the Conv2d Operator
    /// @param[in] dilation Tensor representing the dilation to the Conv2d Operator
    /// @param[in] input_zp Tensor representing the input_zp to the Conv2d Operator
    /// @param[in] weight_zp Tensor representing the weight_zp to the Conv2d Operator
    /// @param[in] local_bound Tensor representing the local_bound to the Conv2d Operator
    /// @param[in] output Tensor representing the output to the Conv2d Operator
    ResId AddConv2dOperator(const Tensor& input,
                            const Tensor& weight,
                            const Tensor& bias,
                            const Tensor& pad,
                            const Tensor& stride,
                            const Tensor& dilation,
                            const Tensor& input_zp,
                            const Tensor& weight_zp,
                            const Tensor& local_bound,
                            const Tensor& output)
    {
        return AddOperator(OperatorEnum::Conv2d,
                           {input, weight, bias},
                           {output},
                           {pad, stride, dilation, input_zp, weight_zp, local_bound})[0];
    }

    /// Function used to add a Conv3d operator to the Graph
    /// @param[in] input Tensor representing the input to the Conv3d Operator
    /// @param[in] weight Tensor representing the weight to the Conv3d Operator
    /// @param[in] bias Tensor representing the bias to the Conv3d Operator
    /// @param[in] pad Tensor representing the pad to the Conv3d Operator
    /// @param[in] stride Tensor representing the stride to the Conv3d Operator
    /// @param[in] dilation Tensor representing the dilation to the Conv3d Operator
    /// @param[in] input_zp Tensor representing the input_zp to the Conv3d Operator
    /// @param[in] weight_zp Tensor representing the weight_zp to the Conv3d Operator
    /// @param[in] local_bound Tensor representing the local_bound to the Conv3d Operator
    /// @param[in] output Tensor representing the output to the Conv3d Operator
    ResId AddConv3dOperator(const Tensor& input,
                            const Tensor& weight,
                            const Tensor& bias,
                            const Tensor& pad,
                            const Tensor& stride,
                            const Tensor& dilation,
                            const Tensor& input_zp,
                            const Tensor& weight_zp,
                            const Tensor& local_bound,
                            const Tensor& output)
    {
        return AddOperator(OperatorEnum::Conv3d,
                           {input, weight, bias},
                           {output},
                           {pad, stride, dilation, input_zp, weight_zp, local_bound})[0];
    }

    /// Function used to add a DepthwiseConv2d operator to the Graph
    /// @param[in] input Tensor representing the input to the DepthwiseConv2d Operator
    /// @param[in] weight Tensor representing the weight to the DepthwiseConv2d Operator
    /// @param[in] bias Tensor representing the bias to the DepthwiseConv2d Operator
    /// @param[in] pad Tensor representing the pad to the DepthwiseConv2d Operator
    /// @param[in] stride Tensor representing the stride to the DepthwiseConv2d Operator
    /// @param[in] dilation Tensor representing the dilation to the DepthwiseConv2d Operator
    /// @param[in] input_zp Tensor representing the input_zp to the DepthwiseConv2d Operator
    /// @param[in] weight_zp Tensor representing the weight_zp to the DepthwiseConv2d Operator
    /// @param[in] local_bound Tensor representing the local_bound to the DepthwiseConv2d Operator
    /// @param[in] output Tensor representing the output to the DepthwiseConv2d Operator
    ResId AddDepthwiseConv2dOperator(const Tensor& input,
                                     const Tensor& weight,
                                     const Tensor& bias,
                                     const Tensor& pad,
                                     const Tensor& stride,
                                     const Tensor& dilation,
                                     const Tensor& input_zp,
                                     const Tensor& weight_zp,
                                     const Tensor& local_bound,
                                     const Tensor& output)
    {
        return AddOperator(OperatorEnum::DepthwiseConv2d,
                           {input, weight, bias},
                           {output},
                           {pad, stride, dilation, input_zp, weight_zp, local_bound})[0];
    }

    /// Function used to add a Fft2d operator to the Graph
    /// @param[in] input_real Tensor representing the input_real to the Fft2d Operator
    /// @param[in] input_imag Tensor representing the input_imag to the Fft2d Operator
    /// @param[in] inverse Tensor representing the inverse to the Fft2d Operator
    /// @param[in] output_real Tensor representing the output_real to the Fft2d Operator
    /// @param[in] local_bound Tensor representing the local_bound to the Fft2d Operator
    /// @param[in] output_imag Tensor representing the output_imag to the Fft2d Operator
    std::vector<ResId> AddFft2dOperator(const Tensor& input_real,
                                        const Tensor& input_imag,
                                        const Tensor& inverse,
                                        const Tensor& output_real,
                                        const Tensor& local_bound,
                                        const Tensor& output_imag)
    {
        return AddOperator(OperatorEnum::Fft2d,
                           {input_real, input_imag},
                           {output_real, output_imag},
                           {inverse, local_bound});
    }

    /// Function used to add a FullyConnected operator to the Graph
    /// @param[in] input Tensor representing the input to the FullyConnected Operator
    /// @param[in] weight Tensor representing the weight to the FullyConnected Operator
    /// @param[in] bias Tensor representing the bias to the FullyConnected Operator
    /// @param[in] input_zp Tensor representing the input_zp to the FullyConnected Operator
    /// @param[in] weight_zp Tensor representing the weight_zp to the FullyConnected Operator
    /// @param[in] output Tensor representing the output to the FullyConnected Operator
    ResId AddFullyConnectedOperator(const Tensor& input,
                                    const Tensor& weight,
                                    const Tensor& bias,
                                    const Tensor& input_zp,
                                    const Tensor& weight_zp,
                                    const Tensor& output)
    {
        return AddOperator(OperatorEnum::FullyConnected,
                           {input, weight, bias},
                           {output},
                           {input_zp, weight_zp})[0];
    }

    /// Function used to add a Matmul operator to the Graph
    /// @param[in] A Tensor representing the A to the Matmul Operator
    /// @param[in] B Tensor representing the B to the Matmul Operator
    /// @param[in] A_zp Tensor representing the A_zp to the Matmul Operator
    /// @param[in] B_zp Tensor representing the B_zp to the Matmul Operator
    /// @param[in] output Tensor representing the output to the Matmul Operator
    ResId AddMatmulOperator(const Tensor& A,
                            const Tensor& B,
                            const Tensor& A_zp,
                            const Tensor& B_zp,
                            const Tensor& output)
    {
        return AddOperator(OperatorEnum::Matmul,
                           {A, B},
                           {output},
                           {A_zp, B_zp})[0];
    }

    /// Function used to add a MaxPool2d operator to the Graph
    /// @param[in] input Tensor representing the input to the MaxPool2d Operator
    /// @param[in] kernel Tensor representing the kernel to the MaxPool2d Operator
    /// @param[in] stride Tensor representing the stride to the MaxPool2d Operator
    /// @param[in] pad Tensor representing the pad to the MaxPool2d Operator
    /// @param[in] output Tensor representing the output to the MaxPool2d Operator
    ResId AddMaxPool2dOperator(const Tensor& input,
                               const Tensor& kernel,
                               const Tensor& stride,
                               const Tensor& pad,
                               const Tensor& output)
    {
        return AddOperator(OperatorEnum::MaxPool2d,
                           {input},
                           {output},
                           {kernel, stride, pad})[0];
    }

    /// Function used to add a Rfft2d operator to the Graph
    /// @param[in] input Tensor representing the input to the Rfft2d Operator
    /// @param[in] output_real Tensor representing the output_real to the Rfft2d Operator
    /// @param[in] output_imag Tensor representing the output_imag to the Rfft2d Operator
    std::vector<ResId> AddRfft2dOperator(const Tensor& input, const Tensor& output_real, const Tensor& output_imag)
    {
        return AddOperator(OperatorEnum::Rfft2d, {input}, {output_real, output_imag});
    }

    /// Function used to add a TransposeConv2d operator to the Graph
    /// @param[in] input Tensor representing the input to the TransposeConv2d Operator
    /// @param[in] weight Tensor representing the weight to the TransposeConv2d Operator
    /// @param[in] bias Tensor representing the bias to the TransposeConv2d Operator
    /// @param[in] out_pad Tensor representing the out_pad to the TransposeConv2d Operator
    /// @param[in] stride Tensor representing the stride to the TransposeConv2d Operator
    /// @param[in] out_shape Tensor representing the out_shape to the TransposeConv2d Operator
    /// @param[in] input_zp Tensor representing the input_zp to the TransposeConv2d Operator
    /// @param[in] weight_zp Tensor representing the weight_zp to the TransposeConv2d Operator
    /// @param[in] local_bound Tensor representing the local_bound to the TransposeConv2d Operator
    /// @param[in] output Tensor representing the output to the TransposeConv2d Operator
    ResId AddTransposeConv2dOperator(const Tensor& input,
                                     const Tensor& weight,
                                     const Tensor& bias,
                                     const Tensor& out_pad,
                                     const Tensor& stride,
                                     const Tensor& out_shape,
                                     const Tensor& input_zp,
                                     const Tensor& weight_zp,
                                     const Tensor& local_bound,
                                     const Tensor& output)
    {
        return AddOperator(OperatorEnum::TransposeConv2d,
                           {input, weight, bias},
                           {output},
                           {out_pad, stride, out_shape, input_zp, weight_zp, local_bound})[0];
    }

    /// Function used to add a Clamp operator to the Graph
    /// @param[in] input Tensor representing the input to the Clamp Operator
    /// @param[in] min_val Tensor representing the min_val to the Clamp Operator
    /// @param[in] max_val Tensor representing the max_val to the Clamp Operator
    /// @param[in] output Tensor representing the output to the Clamp Operator
    ResId AddClampOperator(const Tensor& input, const Tensor& min_val, const Tensor& max_val, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Clamp, {input}, {output}, {min_val, max_val})[0];
    }

    /// Function used to add a Erf operator to the Graph
    /// @param[in] input Tensor representing the input to the Erf Operator
    /// @param[in] output Tensor representing the output to the Erf Operator
    ResId AddErfOperator(const Tensor& input, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Erf, {input}, {output})[0];
    }

    /// Function used to add a Sigmoid operator to the Graph
    /// @param[in] input Tensor representing the input to the Sigmoid Operator
    /// @param[in] output Tensor representing the output to the Sigmoid Operator
    ResId AddSigmoidOperator(const Tensor& input, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Sigmoid, {input}, {output})[0];
    }

    /// Function used to add a Tanh operator to the Graph
    /// @param[in] input Tensor representing the input to the Tanh Operator
    /// @param[in] output Tensor representing the output to the Tanh Operator
    ResId AddTanhOperator(const Tensor& input, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Tanh, {input}, {output})[0];
    }

    /// Function used to add a Add operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Add Operator
    /// @param[in] input2 Tensor representing the input2 to the Add Operator
    /// @param[in] output Tensor representing the output to the Add Operator
    ResId AddAddOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Add, {input1, input2}, {output})[0];
    }

    /// Function used to add a ArithmeticRightShift operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the ArithmeticRightShift Operator
    /// @param[in] input2 Tensor representing the input2 to the ArithmeticRightShift Operator
    /// @param[in] round Tensor representing the round to the ArithmeticRightShift Operator
    /// @param[in] output Tensor representing the output to the ArithmeticRightShift Operator
    ResId AddArithmeticRightShiftOperator(const Tensor& input1, const Tensor& input2, const Tensor& round, const Tensor& output)
    {
        return AddOperator(OperatorEnum::ArithmeticRightShift, {input1, input2}, {output}, {round})[0];
    }

    /// Function used to add a BitwiseAnd operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the BitwiseAnd Operator
    /// @param[in] input2 Tensor representing the input2 to the BitwiseAnd Operator
    /// @param[in] output Tensor representing the output to the BitwiseAnd Operator
    ResId AddBitwiseAndOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::BitwiseAnd, {input1, input2}, {output})[0];
    }

    /// Function used to add a BitwiseOr operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the BitwiseOr Operator
    /// @param[in] input2 Tensor representing the input2 to the BitwiseOr Operator
    /// @param[in] output Tensor representing the output to the BitwiseOr Operator
    ResId AddBitwiseOrOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::BitwiseOr, {input1, input2}, {output})[0];
    }

    /// Function used to add a BitwiseXor operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the BitwiseXor Operator
    /// @param[in] input2 Tensor representing the input2 to the BitwiseXor Operator
    /// @param[in] output Tensor representing the output to the BitwiseXor Operator
    ResId AddBitwiseXorOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::BitwiseXor, {input1, input2}, {output})[0];
    }

    /// Function used to add a Intdiv operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Intdiv Operator
    /// @param[in] input2 Tensor representing the input2 to the Intdiv Operator
    /// @param[in] output Tensor representing the output to the Intdiv Operator
    ResId AddIntdivOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Intdiv, {input1, input2}, {output})[0];
    }

    /// Function used to add a LogicalAnd operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the LogicalAnd Operator
    /// @param[in] input2 Tensor representing the input2 to the LogicalAnd Operator
    /// @param[in] output Tensor representing the output to the LogicalAnd Operator
    ResId AddLogicalAndOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::LogicalAnd, {input1, input2}, {output})[0];
    }

    /// Function used to add a LogicalLeftShift operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the LogicalLeftShift Operator
    /// @param[in] input2 Tensor representing the input2 to the LogicalLeftShift Operator
    /// @param[in] output Tensor representing the output to the LogicalLeftShift Operator
    ResId AddLogicalLeftShiftOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::LogicalLeftShift, {input1, input2}, {output})[0];
    }

    /// Function used to add a LogicalRightShift operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the LogicalRightShift Operator
    /// @param[in] input2 Tensor representing the input2 to the LogicalRightShift Operator
    /// @param[in] output Tensor representing the output to the LogicalRightShift Operator
    ResId AddLogicalRightShiftOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::LogicalRightShift, {input1, input2}, {output})[0];
    }

    /// Function used to add a LogicalOr operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the LogicalOr Operator
    /// @param[in] input2 Tensor representing the input2 to the LogicalOr Operator
    /// @param[in] output Tensor representing the output to the LogicalOr Operator
    ResId AddLogicalOrOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::LogicalOr, {input1, input2}, {output})[0];
    }

    /// Function used to add a LogicalXor operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the LogicalXor Operator
    /// @param[in] input2 Tensor representing the input2 to the LogicalXor Operator
    /// @param[in] output Tensor representing the output to the LogicalXor Operator
    ResId AddLogicalXorOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::LogicalXor, {input1, input2}, {output})[0];
    }

    /// Function used to add a Maximum operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Maximum Operator
    /// @param[in] input2 Tensor representing the input2 to the Maximum Operator
    /// @param[in] output Tensor representing the output to the Maximum Operator
    ResId AddMaximumOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Maximum, {input1, input2}, {output})[0];
    }

    /// Function used to add a Minimum operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Minimum Operator
    /// @param[in] input2 Tensor representing the input2 to the Minimum Operator
    /// @param[in] output Tensor representing the output to the Minimum Operator
    ResId AddMinimumOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Minimum, {input1, input2}, {output})[0];
    }

    /// Function used to add a Mul operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Mul Operator
    /// @param[in] input2 Tensor representing the input2 to the Mul Operator
    /// @param[in] shift Tensor representing the shift to the Mul Operator
    /// @param[in] output Tensor representing the output to the Mul Operator
    ResId AddMulOperator(const Tensor& input1, const Tensor& input2, const Tensor& shift, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Mul, {input1, input2}, {output}, {shift})[0];
    }

    /// Function used to add a Pow operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Pow Operator
    /// @param[in] input2 Tensor representing the input2 to the Pow Operator
    /// @param[in] output Tensor representing the output to the Pow Operator
    ResId AddPowOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Pow, {input1, input2}, {output})[0];
    }

    /// Function used to add a Sub operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Sub Operator
    /// @param[in] input2 Tensor representing the input2 to the Sub Operator
    /// @param[in] output Tensor representing the output to the Sub Operator
    ResId AddSubOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Sub, {input1, input2}, {output})[0];
    }

    /// Function used to add a Table operator to the Graph
    /// @param[in] input Tensor representing the input to the Table Operator
    /// @param[in] table Tensor representing the table to the Table Operator
    /// @param[in] output Tensor representing the output to the Table Operator
    ResId AddTableOperator(const Tensor& input, const Tensor& table, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Table, {input}, {output}, {table})[0];
    }

    /// Function used to add a Abs operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Abs Operator
    /// @param[in] output Tensor representing the output to the Abs Operator
    ResId AddAbsOperator(const Tensor& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Abs, {input1}, {output})[0];
    }

    /// Function used to add a BitwiseNot operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the BitwiseNot Operator
    /// @param[in] output Tensor representing the output to the BitwiseNot Operator
    ResId AddBitwiseNotOperator(const Tensor& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::BitwiseNot, {input1}, {output})[0];
    }

    /// Function used to add a Ceil operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Ceil Operator
    /// @param[in] output Tensor representing the output to the Ceil Operator
    ResId AddCeilOperator(const Tensor& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Ceil, {input1}, {output})[0];
    }

    /// Function used to add a Clz operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Clz Operator
    /// @param[in] output Tensor representing the output to the Clz Operator
    ResId AddClzOperator(const Tensor& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Clz, {input1}, {output})[0];
    }

    /// Function used to add a Exp operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Exp Operator
    /// @param[in] output Tensor representing the output to the Exp Operator
    ResId AddExpOperator(const Tensor& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Exp, {input1}, {output})[0];
    }

    /// Function used to add a Floor operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Floor Operator
    /// @param[in] output Tensor representing the output to the Floor Operator
    ResId AddFloorOperator(const Tensor& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Floor, {input1}, {output})[0];
    }

    /// Function used to add a Log operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Log Operator
    /// @param[in] output Tensor representing the output to the Log Operator
    ResId AddLogOperator(const Tensor& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Log, {input1}, {output})[0];
    }

    /// Function used to add a LogicalNot operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the LogicalNot Operator
    /// @param[in] output Tensor representing the output to the LogicalNot Operator
    ResId AddLogicalNotOperator(const Tensor& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::LogicalNot, {input1}, {output})[0];
    }

    /// Function used to add a Negate operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Negate Operator
    /// @param[in] input1_zp Tensor representing the input1_zp to the Negate Operator
    /// @param[in] output_zp Tensor representing the output_zp to the Negate Operator
    /// @param[in] output Tensor representing the output to the Negate Operator
    ResId AddNegateOperator(const Tensor& input1, const Tensor& input1_zp, const Tensor& output_zp, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Negate, {input1}, {output}, {input1_zp, output_zp})[0];
    }

    /// Function used to add a Reciprocal operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Reciprocal Operator
    /// @param[in] output Tensor representing the output to the Reciprocal Operator
    ResId AddReciprocalOperator(const Tensor& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Reciprocal, {input1}, {output})[0];
    }

    /// Function used to add a Rsqrt operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Rsqrt Operator
    /// @param[in] output Tensor representing the output to the Rsqrt Operator
    ResId AddRsqrtOperator(const Tensor& input1, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Rsqrt, {input1}, {output})[0];
    }

    /// Function used to add a Select operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Select Operator
    /// @param[in] input2 Tensor representing the input2 to the Select Operator
    /// @param[in] input3 Tensor representing the input3 to the Select Operator
    /// @param[in] output Tensor representing the output to the Select Operator
    ResId AddSelectOperator(const Tensor& input1, const Tensor& input2, const Tensor& input3, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Select, {input1, input2, input3}, {output})[0];
    }

    /// Function used to add a Equal operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Equal Operator
    /// @param[in] input2 Tensor representing the input2 to the Equal Operator
    /// @param[in] output Tensor representing the output to the Equal Operator
    ResId AddEqualOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Equal, {input1, input2}, {output})[0];
    }

    /// Function used to add a Greater operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Greater Operator
    /// @param[in] input2 Tensor representing the input2 to the Greater Operator
    /// @param[in] output Tensor representing the output to the Greater Operator
    ResId AddGreaterOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Greater, {input1, input2}, {output})[0];
    }

    /// Function used to add a GreaterEqual operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the GreaterEqual Operator
    /// @param[in] input2 Tensor representing the input2 to the GreaterEqual Operator
    /// @param[in] output Tensor representing the output to the GreaterEqual Operator
    ResId AddGreaterEqualOperator(const Tensor& input1, const Tensor& input2, const Tensor& output)
    {
        return AddOperator(OperatorEnum::GreaterEqual, {input1, input2}, {output})[0];
    }

    /// Function used to add a ReduceAll operator to the Graph
    /// @param[in] input Tensor representing the input to the ReduceAll Operator
    /// @param[in] axis Tensor representing the axis to the ReduceAll Operator
    /// @param[in] output Tensor representing the output to the ReduceAll Operator
    ResId AddReduceAllOperator(const Tensor& input, const Tensor& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::ReduceAll, {input}, {output}, {axis})[0];
    }

    /// Function used to add a ReduceAny operator to the Graph
    /// @param[in] input Tensor representing the input to the ReduceAny Operator
    /// @param[in] axis Tensor representing the axis to the ReduceAny Operator
    /// @param[in] output Tensor representing the output to the ReduceAny Operator
    ResId AddReduceAnyOperator(const Tensor& input, const Tensor& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::ReduceAny, {input}, {output}, {axis})[0];
    }

    /// Function used to add a ReduceMax operator to the Graph
    /// @param[in] input Tensor representing the input to the ReduceMax Operator
    /// @param[in] axis Tensor representing the axis to the ReduceMax Operator
    /// @param[in] output Tensor representing the output to the ReduceMax Operator
    ResId AddReduceMaxOperator(const Tensor& input, const Tensor& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::ReduceMax, {input}, {output}, {axis})[0];
    }

    /// Function used to add a ReduceMin operator to the Graph
    /// @param[in] input Tensor representing the input to the ReduceMin Operator
    /// @param[in] axis Tensor representing the axis to the ReduceMin Operator
    /// @param[in] output Tensor representing the output to the ReduceMin Operator
    ResId AddReduceMinOperator(const Tensor& input, const Tensor& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::ReduceMin, {input}, {output}, {axis})[0];
    }

    /// Function used to add a ReduceProduct operator to the Graph
    /// @param[in] input Tensor representing the input to the ReduceProduct Operator
    /// @param[in] axis Tensor representing the axis to the ReduceProduct Operator
    /// @param[in] output Tensor representing the output to the ReduceProduct Operator
    ResId AddReduceProductOperator(const Tensor& input, const Tensor& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::ReduceProduct, {input}, {output}, {axis})[0];
    }

    /// Function used to add a ReduceSum operator to the Graph
    /// @param[in] input Tensor representing the input to the ReduceSum Operator
    /// @param[in] axis Tensor representing the axis to the ReduceSum Operator
    /// @param[in] output Tensor representing the output to the ReduceSum Operator
    ResId AddReduceSumOperator(const Tensor& input, const Tensor& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::ReduceSum, {input}, {output}, {axis})[0];
    }

    /// Function used to add a Concat operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Concat Operator
    /// @param[in] axis Tensor representing the axis to the Concat Operator
    /// @param[in] output Tensor representing the output to the Concat Operator
    ResId AddConcatOperator(const std::vector<Tensor>& input1, const Tensor& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Concat, {input1}, {output}, {axis})[0];
    }

    /// Function used to add a Pad operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Pad Operator
    /// @param[in] padding Tensor representing the padding to the Pad Operator
    /// @param[in] pad_const Tensor representing the pad_const to the Pad Operator
    /// @param[in] output Tensor representing the output to the Pad Operator
    ResId AddPadOperator(const Tensor& input1, const Tensor& padding, const Tensor& pad_const, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Pad, {input1, padding}, {output}, {pad_const})[0];
    }

    /// Function used to add a Reshape operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Reshape Operator
    /// @param[in] shape Tensor representing the shape to the Reshape Operator
    /// @param[in] output Tensor representing the output to the Reshape Operator
    ResId AddReshapeOperator(const Tensor& input1, const Tensor& shape, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Reshape, {input1}, {output}, {shape})[0];
    }

    /// Function used to add a Reverse operator to the Graph
    /// @param[in] input Tensor representing the input to the Reverse Operator
    /// @param[in] axis Tensor representing the axis to the Reverse Operator
    /// @param[in] output Tensor representing the output to the Reverse Operator
    ResId AddReverseOperator(const Tensor& input, const Tensor& axis, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Reverse, {input}, {output}, {axis})[0];
    }

    /// Function used to add a Slice operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Slice Operator
    /// @param[in] start Tensor representing the start to the Slice Operator
    /// @param[in] size Tensor representing the size to the Slice Operator
    /// @param[in] output Tensor representing the output to the Slice Operator
    ResId AddSliceOperator(const Tensor& input1, const Tensor& start, const Tensor& size, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Slice, {input1}, {output}, {start, size})[0];
    }

    /// Function used to add a Tile operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Tile Operator
    /// @param[in] multiples Tensor representing the multiples to the Tile Operator
    /// @param[in] output Tensor representing the output to the Tile Operator
    ResId AddTileOperator(const Tensor& input1, const Tensor& multiples, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Tile, {input1}, {output}, {multiples})[0];
    }

    /// Function used to add a Transpose operator to the Graph
    /// @param[in] input1 Tensor representing the input1 to the Transpose Operator
    /// @param[in] perms Tensor representing the perms to the Transpose Operator
    /// @param[in] output Tensor representing the output to the Transpose Operator
    ResId AddTransposeOperator(const Tensor& input1, const Tensor& perms, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Transpose, {input1}, {output}, {perms})[0];
    }

    /// Function used to add a Gather operator to the Graph
    /// @param[in] values Tensor representing the values to the Gather Operator
    /// @param[in] indices Tensor representing the indices to the Gather Operator
    /// @param[in] output Tensor representing the output to the Gather Operator
    ResId AddGatherOperator(const Tensor& values, const Tensor& indices, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Gather, {values, indices}, {output})[0];
    }

    /// Function used to add a Scatter operator to the Graph
    /// @param[in] values_in Tensor representing the values_in to the Scatter Operator
    /// @param[in] indices Tensor representing the indices to the Scatter Operator
    /// @param[in] input Tensor representing the input to the Scatter Operator
    /// @param[in] values_out Tensor representing the values_out to the Scatter Operator
    ResId AddScatterOperator(const Tensor& values_in, const Tensor& indices, const Tensor& input, const Tensor& values_out)
    {
        return AddOperator(OperatorEnum::Scatter, {values_in, indices, input}, {values_out})[0];
    }

    /// Function used to add a Resize operator to the Graph
    /// @param[in] input Tensor representing the input to the Resize Operator
    /// @param[in] scale Tensor representing the scale to the Resize Operator
    /// @param[in] offset Tensor representing the offset to the Resize Operator
    /// @param[in] border Tensor representing the border to the Resize Operator
    /// @param[in] mode Tensor representing the mode to the Resize Operator
    /// @param[in] output Tensor representing the output to the Resize Operator
    ResId AddResizeOperator(const Tensor& input,
                            const Tensor& scale,
                            const Tensor& offset,
                            const Tensor& border,
                            const Tensor& mode,
                            const Tensor& output)
    {
        return AddOperator(OperatorEnum::Resize,
                           {input},
                           {output},
                           {scale, offset, border, mode})[0];
    }

    /// Function used to add a Cast operator to the Graph
    /// @param[in] input Tensor representing the input to the Cast Operator
    /// @param[in] output Tensor representing the output to the Cast Operator
    ResId AddCastOperator(const Tensor& input, const Tensor& output)
    {
        return AddOperator(OperatorEnum::Cast, {input}, {output})[0];
    }

    /// Function used to add a Rescale operator to the Graph
    /// @param[in] input Tensor representing the input to the Rescale Operator
    /// @param[in] output Tensor representing the output to the Rescale Operator
    /// @param[in] input_zp Tensor representing the input_zp to the Rescale Operator
    /// @param[in] output_zp Tensor representing the output_zp to the Rescale Operator
    /// @param[in] multiplier Tensor representing the multiplier to the Rescale Operator
    /// @param[in] shift Tensor representing the shift to the Rescale Operator
    /// @param[in] scale32 Tensor representing the scale32 to the Rescale Operator
    /// @param[in] double_round Tensor representing the double_round to the Rescale Operator
    /// @param[in] per_channel Tensor representing the per_channel to the Rescale Operator
    /// @param[in] input_unsigned Tensor representing the input_unsigned to the Rescale Operator
    /// @param[in] output_unsigned Tensor representing the output_unsigned to the Rescale Operator
    ResId AddRescaleOperator(const Tensor& input,
                             const Tensor& output,
                             const Tensor& input_zp,
                             const Tensor& output_zp,
                             const Tensor& multiplier,
                             const Tensor& shift,
                             const Tensor& scale32,
                             const Tensor& double_round,
                             const Tensor& per_channel,
                             const Tensor& input_unsigned,
                             const Tensor& output_unsigned)
    {
        return AddOperator(OperatorEnum::Rescale,
                           {input},
                           {output},
                           {input_zp, output_zp, multiplier, shift, scale32, double_round, per_channel, input_unsigned, output_unsigned})[0];
    }


//                           ==================  Generated Code End ==================

private:
    /// Private Write function used by Module friend class to write its member Graphs to Binary.
    /// @param[in] writer Writer initialized by Module used to write to binary.
    /// @param[in] tosaId ResID of the Extended Instruction Set Import of TOSA.
    void Write(spirvwriter::Writer& writer, const ResId& tosaId);

    /// Private Graph constructor as graphs should only be created by the Module friend class AddGraph() method.
    /// Module::AddGraph() returns a shared pointer to the graph for the user to then make calls to AddXXXLayer().
    /// @param[in] idGenerator Reference to IdGenerator of the Module used to generate consistent IDs for the Layers.
    /// @param[in] name std::string that will be used as the name of the graph.
    explicit Graph(IdGenerator& idGenerator, const std::string& name = std::string())
        : m_IdGenerator(idGenerator)
        , m_GraphId(m_IdGenerator.GetNextId())
        , m_Name(name.empty() ? "Graph_" + std::to_string(m_GraphId.Get()) : name)
    {};

    /// Private function used to reorder the graph outputs
    /// @param[in] vector of graph outputs whose ResId values we intend to reorder
    void ReorderGraphOutputs(std::vector<GraphIOInfo>&);

    std::vector<Operator> m_Operators;
    std::vector<ResId> m_GraphOutputs;

    IdGenerator& m_IdGenerator;
    ResId m_GraphId;
    const std::string m_Name;
};

} // namespace graphbuilder

} // namespace tosa2spirv