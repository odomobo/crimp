# Introduction
The basic premise of crimp is that it's a compiled, statically-typed c-like language with easy, zero-cost native c ffi. The language has a few key features/attributes:

* automatic memory management via gc
* classes (reference types), structs (value types), pointers to value types (unmanaged reference types)
* no semantics around object lifetimes (no destructors, classes are just memory)
* defer keyword (for cleaning up resources, in absense of destructors/raii)
* exceptions (because go is awful)
* eventually, generics (because every language needs them)

The syntax should mostly look like c, c#, or go, with tweaks primarily to make parsing clean and unambiguous.

The plan for language implementation is to define a minimum viable implementation of the language, and then add features on top of this. The number and complexity of the features should be balanced with the complexity they bring to the language. It needs to strike a balance between a language which is easy to use, easy to read, unencumbered, but also minimal in features and easy to fully understand.

## C ffi
One of the key features of the language is simple, low-cost, low-friction interfacing with c. There are a few strategies and decisions which enable this:

First off, exposing all types to c. The language's semantics mostly map directly onto c's. The one major exception from this is memory management, in that all classes are automatically managed. In order to use classes in c, the c code needs to explicitly request handles to the individual class objects, and needs to release the handle when it's finished. This lets the garbage collector see when c code is using class objects.

Secondly, exposing all functions to c. The c variant of functions look a little different than the signatures look in crimp. Let me show a few examples:

// crimp
void foo();
// c
Exception* foo();

// crimp
void foo(BarClass obj);
// c
Exception* foo(BarClass* obj);

// crimp
BazClass foo(i32 count);
// c
Exception* foo(BazClass_WriteHandle* ret, int32_t count);

Note how all crimp functions return an exception pointer (which they must always check for nullability to see if an exception has been thrown). This makes it such that all return values must be out parameters.

## Garbage collection

TODO