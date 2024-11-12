# haste-lang

# How to try haste?
1. you got to install the repo first and navigate to where you clone the repo
```sh
$ git clone https://github.com/mega-haste/haste-lang
$ cd haste-lang
```
2. You should be already installed cmake and run this command
```sh
$ cmake -S . -G "Unix Makefiles" -B build
```
3. Finally you can run haste (as a test)
```sh
$ make
$ ./haste-test
```

# Language progress
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
    - [ ] Types
        - [x] Built-in types (**int**, **uint**, **float**, **string**, **void**, **bool**)
        - [x] Slices (`T[int]`, `T[]`)
        - [ ] Tuple (`(T, R, Y, X, ...)`)
        - [ ] Generics aka. templates in c++ (`T<Args>`)
    - [ ] Statements
        - [x] if
        - [x] let
        - [ ] functions (needs to add generecs to it)

# Something
I dropped the idea of tags, sorry [Amir](https://github.com/Ameeer1)
