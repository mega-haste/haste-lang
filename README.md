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
        - [x] Math (+, -, *, **, /, %)
        - [x] Comparition (==, !=, <, >, >=, =<)
        - [x] Logical (not, and, or)
        - [x] Bitewise (&, |, ~, <<, >>)
        - [x] inline if (if .. then .. else ..)
        - [x] as (type casting `expr as Type`)
        - [x] call expression
        - [x] member access
        - [ ] Subscript (`expr[v]`)
        - [ ] Scope resolusion
    - [ ] Types
        - [x] Built-in types (**int**, **uint**, **float**, **string**, **void**, **bool**)
        - [x] Slices (`T[int]`, `T[]`)
        - [ ] Tuple (`(T, R, Y, X, ...)`)
        - [ ] Generics aka. templates in c++ (`T<Args>`)

# Credits
Thanks to [Amir](https://github.com/Ameeer1) for leting me to steal the idea of `tags`
