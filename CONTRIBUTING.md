# Contributor Guide

The tosa_for_spirv_codegen project is open for external
contributors and welcomes contributions. tosa_for_spirv_codegen is
licensed under the Apache-2.0 and all accepted contributions must have the same
license.

## Developer Certificate of Origin (DCO)

Before the tosa_for_spirv_codegen project accepts your
contribution, you need to certify its origin and give us your permission. To
manage this process we use Developer Certificate of Origin (DCO) V1.1
(<https://developercertificate.org/>).

To indicate that you agree to the the terms of the DCO, you "sign off" your
contribution by adding a line with your name and e-mail address to every git
commit message:

Signed-off-by: John Doe <john.doe@example.org>

You must use your real name, no pseudonyms or anonymous contributions are
accepted.

## C++ Coding Style

Changes to tosa_for_spirv_codegen should follow the
[CppCoreGuidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md)

Use clang-format and cppcheck to check your changes. They can be install by
running:

```bash
apt-get install -y clang-format cppcheck
```