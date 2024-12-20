# haste-lang

![turbo](./imgs/turbo.png)

## SWITCHED TO ZIG
long story short, C++ sucks so i switched to something Zig.

## How to try haste?

1. You got to clone this repo and navigate to where you cloned it
```sh
git clone https://github.com/mega-haste/haste-lang
cd haste-lang
```

2. You should be already installed [zig](https://ziglang.org/) in order to try haste
```sh
zig build run
```

3. Now create a `main.haste`
```sh
touch main.haste
```

4. Edite `main.haste` file you created earlier and add these folowing lines
```haste
func main() {
    let a: int = 50; # Expected warning
}
```

## Language progress

- [x] Scanner
- [ ] Parser
  - [ ] Expressions
    - [x] Primaries
    - [ ] Math (+, -, *, **, /, %)
    - [ ] Comparition (==, !=, <, >, >=, =<)
    - [ ] Logical (not, and, or)
    - [ ] Bitewise (&, |, ~, <<, >>)
    - [ ] inline if (if .. then .. else ..)
    - [ ] as (type casting `expr as Type`)
    - [ ] call expression
    - [ ] member access
    - [ ] Subscript (`expr[v]`)
    - [ ] Scope resolusion
    - [ ] Tuple
    - [ ] assignment
    - [ ] referencing and dereferencing (`&expr` and `*expr`)
  - [ ] Types
    - [ ] Built-in types (**int**, **uint**, **float**, **string**, **void**, **bool**)
    - [ ] Slices (`T[int]`, `T[]`)
    - [ ] Tuple (`(T, R, Y, X, ...)`)
    - [ ] Generics aka. templates in c++ (`T<Args>`)
  - [ ] Statements
    - [ ] returns
    - [ ] if
    - [ ] let
    - [ ] functions (needs to add generecs to it)
  - [ ] Semantic Analysis
    - [ ] Added basic types (arrays, functions, native types eg. bool, int, uint, char, string)
    - [ ] Working immutability
    - [ ] Function calls
    - [ ] Subscripting
    - [ ] Member accessing
    - [ ] Scope resolusion
    - [ ] Warnings (unused variables, unused mut)
    - [ ] tuples and multi dimentional arrays
    - [ ] Custom Defined types
    - [ ] Generics
    - [ ] Secret 🤫

## Hello World
```haste
func main() {
    # no working hello world yet :(
    # but we got variables :D
    let x = 5;
    let mut y = 9;
    y += 1;
}
```

```haste
# My desired hello world
func main() {
    @println("Hello World");
}
```

## My goals

- [ ] Nicer Error messages
- [ ] Adding a good type-checking & Semantic analysis
- [ ] Traspiling haste to c (I'm planing on using llvm to compile haste directly to machine code)

## Something

I dropped the idea of tags, sorry [Amir](https://github.com/Ameeer1)
