# libcxxbase

    This library provides cross-platform basic operations for general purpose C++ programming, written in C++ 11. Do note that some syntaxes used in this library rely on GCC features, any other compiler may not have a correct behavior. libcxxbase is consists of some functionality-independent parts, building each part independently or the whole is both feasible. When building independently, the dependent libraries need to be determined, some of which are depended upon by others. If build the whole library, these parts can be archived into a universal library file.<br>

## cxxtricks

Header-only library. Provide template meta programming, macro meta programming and related tricks.<br>
macros.h	macro meta programming, also available for C.<br>
types.h	template meta programming.<br>
access.h	member detect and access tricks.<br>
callable.h	callable tricks.<br>

## cxxllo

Low-level operations for C++ at runtime.<br>

## cxxcomm

Common utils and functions, classes for general purpose programming.<br>

## cxxsci

Implementions for Science calculation and algorithms.<br>