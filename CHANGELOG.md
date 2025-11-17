# Changelog

Tosa-for-spirv-codegen follows the [Semantic Versioning](https://semver.org/) specification for releases

## Upcoming Release

# v1.0.0

* Graphbuilder API for TOSA 1.0
* TosaSerializationParser
* tosa_regen library for creating TOSA flatbuffers from SPIR-V
* VGFWriter library for dumping .vgf files
* Python scripts for generating front end API
* Support for following operators:
	* Abs
	* Add
	* Argmax
	* ArithmeticRightShift
	* AvgPool2d
	* BitwiseAnd
	* BitwiseNot
	* BitwiseOr
	* BitwiseXor
	* Cast
	* Ceil
	* Clamp
	* Clz
	* Concat
	* Const
	* Conv2d
	* Conv3d
	* DepthwiseConv2d
	* Dim
	* Equal
	* Erf
	* Exp
	* Fft2d
	* Floor
	* FullyConnected
	* Gather
	* Greater
	* GreaterEqual
	* Intdiv
	* Log
	* LogicalAnd
	* LogicalLeftShift
	* LogicalNot
	* LogicalRightShift
	* LogicalOr
	* LogicalXor
	* Matmul
	* MaxPool2d
	* Maximum
	* Minimum
	* Mul
	* Negate
	* Pad
	* Pow
	* Reciprocal
	* ReduceAll
	* ReduceAny
	* ReduceMax
	* ReduceMin
	* ReduceProduct
	* ReduceSum
	* Rescale
	* Reshape
	* Resize
	* Reverse
	* Rfft2d
	* Rsqrt
	* Scatter
	* Select
	* Sigmoid
	* Slice
	* Softmax
	* Sub
	* Table
	* Tanh
	* Tile
	* Transpose
	* TransposeConv2d
* NOTE: There is a known issue where .vgf files produced by tosa-for-spirv-codegen represent rank-0 tensors as rank-1. This follows SPIR-V conventions but is inconsistent with .vgf, which expects rank-0. As a result, some CTS tests may fail due to shape mismatches in .npy files; values are unaffected.
