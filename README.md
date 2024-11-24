# haste-lang

![turbo](./imgs/turbo.png)

## How to try haste?

1. You got to clone this repo and navigate to where you cloned it
```sh
git clone https://github.com/mega-haste/haste-lang
cd haste-lang
```

2. You should be already installed [premake](https://premake.github.io/) in order to run this command
```sh
premake5 gmake --cc=clang
```

> NOTE: Since i switched to premake testing the scanner isn't avialable yet cuz I'm too lazy to do that

3. Now go to `build/` folder and run make
```sh
cd build/
make
```

4. Now create a `main.haste` file inside `build/`
```sh
touch main.haste
```

5. Edite `main.haste` file you created earlier and add these folowing lines
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
    - [x] Math (+, -, *, **, /, %)
    - [x] Comparition (==, !=, <, >, >=, =<)
    - [x] Logical (not, and, or)
    - [x] Bitewise (&, |, ~, <<, >>)
    - [x] inline if (if .. then .. else ..)
    - [x] as (type casting `expr as Type`)
    - [x] call expression
    - [x] member access
    - [x] Subscript (`expr[v]`)
    - [x] Scope resolusion
    - [x] Tuple
    - [x] assignment
    - [ ] referencing and dereferencing (`&expr` and `*expr`)
  - [ ] Types
    - [x] Built-in types (**int**, **uint**, **float**, **string**, **void**, **bool**)
    - [x] Slices (`T[int]`, `T[]`)
    - [ ] Tuple (`(T, R, Y, X, ...)`)
    - [ ] Generics aka. templates in c++ (`T<Args>`)
  - [ ] Statements
    - [x] returns
    - [x] if
    - [x] let
    - [ ] functions (needs to add generecs to it)
  - [ ] Semantic Analysis
    - [x] Added basic types (arrays, functions, native types eg. bool, int, uint, char, string)
    - [x] Working immutability
    - [ ] Function calls
    - [ ] Subscripting
    - [ ] Member accessing
    - [ ] Scope resolusion
    - [x] Warnings (unused variables, unused mut)
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

- [x] Nicer Error messages
- [ ] Adding a good type-checking & Semantic analysis
- [ ] Traspiling haste to c (I'm planing on using llvm to compile haste directly to machine code)

## Something

I dropped the idea of tags, sorry [Amir](https://github.com/Ameeer1)
