# Crimp Language
The specification aims to precisely-define the language, such that it can be used with no ambiguity as to what behavior will be for a particular construct, and such that a compiler for the language can also be implemented without ambiguity.

## Guiding principles
* simple to understand memory model
* simple to understand what behavior will happen for particular code construct
* performant
* c ffi as first-class citizen

## Influences
Most of crimp's influences come from go, because of go's committment to simplicity. Unlike go, it has exceptions, and it uses C#'s approach of making classes implicitly be reference types. This is nice for ergonomics. I'm not exactly sure what influences it follows for operator overloading... maybe C# again?

## Identifiers
Identifiers follow c's naming rules: [_a-zA-Z][_a-zA-Z0-9]*

## Namespacing
Every type and function will exist within some namespace.

## Exceptions
Exceptions form a hierarchy. Every function implicitly returns an exception (explicit on the c side of things, unsurprisingly). If the exception returned is null, that means "no exception", otherwise it returns the exception information. Exceptions are a special type of class.

## Traits vs Interfaces
Traits are a clever solution to the problem of polymorphism, but unfortunately, add a lot of complexity which is at odds with the language (and especially with its cffi). Interfaces can solve all the problems that need to be solved.

## Function/method overloading
Disallowed, although you can kind of get around this with generics (type parameters on functions). That being said, generics work quite differently than overloading, so it isn't really a workaround - it's really just a feature which has its own purpose.

## Operator overloading
I was thinking about implementing through interfaces, but then it will simply not be possible to do appropriate operator overloading without method overloading in the language. It wouldn't be possible to implement IAdd[int] and IAdd[double], because that would require implementing int Add(int) and double Add(double).

So, instead, this will need to be implemented through special operator overloading logic.

Programmers are encouraged to limit the use of operator overloading to only logic that match the semantics of the operator:

Addition ( + ): numerical addition. It should have the symmetric property: a + b should yield the same result as b + a
Subtraction ( - ): numerical subtraction. It should have the property that (a - b) + b == a, and that (a + b) - b == a (not considering overflow/underflow)
Multiplication ( * ): numerical multiplication. It should have the symmetric property
Floating division ( / ): numerical division. It should have the property that (a * b) / b == a, and that (a / b) * b == a
Integer division ( // ): division with integer rules (division with truncation). Because of the truncation, it doesn't need to have any invariants combined with multiplication
Modulo ( % ): remainder after division. It should have the property that (a // b) * b + (a % b) == a

Unary negation ( - ): mathematical negation. It should have the property that -(-a) == a
Note that using -- in the language is a syntax error (as a special case, to highlight bugs when the programmer thought they were using decrement).

Greater than ( > ): if one value logically has a greater value than another. This should only be used when that has a clear meaning (for example, it's not entirely clear what it would mean for a string or an IP address)
Less than ( < ): similar as greater than. It should have the invariant that a > b implies b < a
Equal to ( == ): if two values are logically equal
Note that if equal to is implemented, then not equal to ( != ) is automatically implemented through its negation.

Concatenation ( ++ ): combines two values together through concatenation, returning a new value which is the result of combining the two together. Logically, the concatenation should be able to be undone as long as you know the concatenation point. So, for example, it makes sense to concatenate two strings or two lists, but it doesn't make sense to concatenate two sets or two dictionaries.

Boolean operators ( && , || , ! ) are not overloadable. They should always operate on bools.

Bitwise operators ( &, |, ~, ^ , >> , << ) don't exist in the language as such, and are instead functions.

Array index [] get and set. These are a special case, because even though they're binary, they don't have an rhs variant.

For binary operators, the resolution happens like:

1. Look for specific type, for lhs interface
2. Look for specific type, for rhs interface
3. If both are implemented, compile-time error (ambiguous parsing)
4. If exactly one is implemented, use that
5. Look for generic types, for lhs interface
6. If there are multiple, and one is strictly more concrete than the others, then that is the selection for lhs interface
7. If there are multiple, and there isn't one which is strictly the most concrete, then compile time error (ambiguous parsing)
8. Perform steps 5-7, except for rhs interface instead
9. Take the results from 5-8. If there was exactly 1 implementation selected for lhs and none for rhs (or vice versa), then use that interface
10. Else, if there was one selected for lhs and 1 selected for rhs, then compile-time error (ambiguous parsing)
11. Else, if there were none selected for lhs or rhs, then compile-time error (no operator overload available)

For unary operators, do the same, but only pay attention to the lhs rules (ignore rhs, which is not applicable).

## Implicit conversion
The language has some builtin conversion around (strictly) widening numeric types, but otherwise it's not allowed.

## Numeric literals
I'm not sure how to handle this, especially in the case of overloading. It should be at least somewhat like c#. A numeric literal should be able to shape-shift based on context, and it should always coalesce into a specific form. But, when using the literal with an overload, how to determine which form to coalesce into? Hm... this will require some more thought.

## Generics
Generics take type parameters, which are optionally constrained. A constraint is always an interface. For example:

class Foo[T] {...
class Bar[T: IQuux] {...

Functions and methods can take type parameters.

## Monomorphization
There are 3 ways that a parameter can be monomorphized:

Value-type parameters are monomorphized with a length, and an explicit type adapter passed in function calls. The length is the length in bytes of the value.
Concrete reference-type parameters are monomorphized as a value-type parameter with length 8 (well, the word size for that architecture).
Interface parameters are monomorphized as a value-type parameter with length 16. The type parameter is the adapter for the interface fat pointer. When calling a method on the type, it does a double lookup: it calls a lookup into the adapter, which does the appropriate lookup into the fat pointer's vtable.

This means that whenever monomorphizing any function, it will always perform dynamic dispatch, even for known types (although minimal dynamic dispatch in those instances).

Note that default monomorphization of any function always provides 2 instances: all parameters with length 8, and all parameters with length 16. This means that there's always an instance that uses all reference types (boxing value types if necessary), and always an instance that uses all dynamic interface types. I think this is required for reflection? If it's not, then we can probably get rid of this requirement (and always monomorphize only 1 instance by default, or even 0).

## Classes
Classes always have a fixed-size. They cannot be dynamically-sized. The only dynamically-sized type is the heap array.

## Inheritance
Classes don't have inheritance. I'm not even sure if I can do what go does, with structure embedding...

I need to figure out how to solve the problems that other languages solve with inheritance, in a fairly-performant way. What I don't want to have is that it becomes typical in the language to have long chains of objects, instead of long chains of inheritance. The layers of indirection could kill performance. Need to come up with different cases for this, and analyze.

## Constructors
Constructors are all named (generally, static members of their type, generally prefixed by "new"), and implicitly take a "self" parameter. In c, you'd construct a class kinda like this:

Foo_hBuilder* f_hb = Foo_hBuilder_create();
unwrap(  crimp_alloc(Foo_TypeInfo, sizeof(Foo), &f_hb->slot)  );
Foo_handle* f_h = Foo_hBuilder_complete(f_hb);
unwrap(  Foo_new(f_h->data, 1, 2, 3)  );
// do stuff with f_h
Foo_handle_release(f_h);

You don't really want to stack allocate anything in c, because it's hard to track references, but this is what the generated code will look like:
(Note that it's very similar to heap allocation in generated code)

frame* fr = frame_alloc(2);
Foo foo = {0}; // or whatever the syntax for 0-initialization is
Exception* e = crimp_stackAlloc(Foo_TypeInfo, sizeof(Foo), &foo);
// handle e
Foo** foo_ptr = frame_get(fr, 0);
*foo_ptr = &foo; // need to track foo before construction
e = Foo_new(&foo, 1, 2, 3);
// handle e
// do stuff with foo
frame_release(fr);

The point of constructors taking a reference is it allows the compiler to stack-allocate objects when it knows they will never escape the stack. It's a nice little optimization whose only cost is a slightly more confusing c interface. Maybe when we export to c, we wrap everything in a heap-allocation interface that hides the allocation and simply returns the heap-allocated object.

## Heap array
Heap arrays work like in c#

## Slices
I'm not sure what I think about slices. Since they're opaque to the parent, it could become difficult to remember who the parent is, and keep track of conflicting modifications.

## Fixed-sized arrays
I think I want this? But, specifically, as members of structs and classes, and they don't have value semantics associated with them (that is, you can't copy a full array with assignment).

## Const
We won't be doing const classes like in c++. Rather, a const class just means the pointer to the class can't change. If you want an immutable class, you define it through its interface. Const value types does mean the value type can't change, but that's fine.

## Struct methods and interfaces
A struct method or interface takes the struct by value. I'm not sure how working with a boxed value through its interface will work. Maybe there will be an adapter monomorphization, which performs automatic unboxing and boxing on its inputs and outputs.

## Unsafe
Unsafe is required in order to work with pointers in any capacity.

## Name mangling
Although it's technically possible to store unmangled identifiers in the object file, it's infeasible to do so while using c as a transpiler target. Instead, perform minimal name mangling, and compile with a demangler lookup table (ordered by hash), which performs a binary search to find the correct entry. Then, demangle types when printing backtraces.

Name mangling will be like:

Name.Space.Type[Foo, Bar].function
becomes
Name_Space_Type_OPEN_8_8_CLOSE_function_YXNkZmJmZXdrZmJ3ZQ

And because type parameters monomorphize to their size, then complex type parameters condense down:

System.Generics.Dictionary[System.String, System.Generics.List[Foo.Bar]]
becomes
System_Generics_Dictionary_OPEN_8_8_CLOSE_MzQ1MzQ4NTM5ODQ1Cg

The hash on the end tries to ensure a unique function name, since there's otherwise no guarantee. Naming conventions recommend no identifiers use camel_case, but a user can disregard if they want.

## Reflection
Reflection is a key aspect of the langauge. In languages like rust, things like serialization are performed using compile-time code execution (in some form or another), or traits + monomorphization. However, to keep compile times short, crimp doesn't have that functionality. So, instead, reflection will be the only tool available. Because of this, reflection must be easy to use and as performant as possible.

## RTTI
Runtime type information should be available to use directly. You should be able to pass types around, and treat them like objects. Type information objects should be a special type of class which doesn't get traced for garbage collection, because it's not possible to create new type objects at runtime. They are all only constructed at compile time.

## Builtin types with special handling
heap arrays (constructor-defined length)
strings (special type of heap arrays)
numeric & bool primitives
type types (aren't collected, because they cannot be created or freed).
exceptions (have some kind of hierarchy)
