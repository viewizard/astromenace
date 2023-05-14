# Contribution License Agreement

If you want/plan to contribute, we ask you to sign a [CLA](https://cla-assistant.io/viewizard/astromenace) (Contribution License Agreement). A friendly bot will remind you about it when you submit a pull-request.

# AstroMenace Coding Standards & Guidelines

This document defines the AstroMenace coding standards and style, all code contributions are expected to adhere to the rules described below.

## Coding Standard

AstroMenace is written under **[ISO/IEC 14882:2011 (C++11)][ISO/IEC/C++11]** without GNU/GCC specific extensions (i.e. AstroMenace should compile with `CXXFLAGS` set to `-std=c++11`, regardless of if `-std=gnu+11` is being used during compile time). Source code must be able to be compiled under multiple different compilers and operating systems including but not limited to GCC and Clang on Microsoft Windows, Apple OS X and GNU/Linux-based derivatives.

In addition to the base language, the following additional restrictions are imposed:

### No Exceptions

AstroMenace is compiled without support for [C++11 exceptions][Exceptions], meaning that any code contribution or dependency cannot throw a C++ exception at runtime or else the application will crash. For code present in the AstroMenace repository, this is enforced by the use of the `-fno-exceptions` flag in the CMake configuration.

### No RTTI

AstroMenace is compiled without support for [RTTI], as such code contributions using `dynamic_cast()` or `std::dynamic_pointer_cast()` may fail to compile and may be rejected on this basis. The implications of this are that the signature of all polymorphic types must be known at compile time or stored in an implementation-specific way. In essence, if a substitution from `dynamic_cast()` to `static_cast()` can be performed without affecting program correctness, the construct in question is valid.

### No GOTO statement

GOTO statement not allowed. Code logic should not demand long jumps. Functions should be short and clear, so, if you need GOTO in AstroMenace code - you are doing something wrong. For some cases, GOTO could be replaced by another C++ features, for example, named Lambda functions.

### No CONTINUE statement, avoid BREAK statement

CONTINUE and BREAK statements in the middle of cycle could act like a trap. Try to avoid their usage by altering cycle logic and/or cycle conditions.

### Dynamic memory management

Only [Smart Pointers] are allowed. Manual memory management are prohibited. Even if you need small temporary memory buffer, use std::vector or std::unique_ptr instead of new/delete.

## AstroMenace Coding Style

AstroMenace coding style based on K&R coding style derivatives (line of text in code could be up to 200 characters long).

<!-- Markdown links -->
[ISO/IEC/C++11]: https://www.iso.org/standard/50372.html
[Exceptions]: https://en.wikipedia.org/wiki/C%2B%2B#Exception_handling
[RTTI]: https://en.wikipedia.org/wiki/Run-time_type_information
[Smart Pointers]: http://en.cppreference.com/w/cpp/memory
