SSLC
----

This is a modified copy of sslc, that has a few bugfixes from the original, that will recognise and compile the additional script functions provided by sfall, that can also understand some additional syntax, and that has an integrated preprocessor and optimizer.

Unlike the original script compiler, this has not been compiled as a DOS program. When using this in place of the original `compile.exe` but still using `p.bat`, you need to get rid of the `dos4gw.exe` reference from `p.bat`.

When compiling global or hook scripts for sfall 3.4 or below, you _must_ include the line `procedure start;` before any `#include` files that define procedures to avoid a few weird problems. This is no longer required starting from 3.5.

This version of compiler was designed primarily for new sfall functions, but it can be safely used (and is recommended) with non-sfall scripts as well.

The original unmodified sslc source is over here: [https://teamx.ru/site_arc/utils/index.html](https://teamx.ru/site_arc/utils/index.html)

### Command line options

```
-q  don't wait for input on error
-n  no warnings
-b  use backward compatibility mode
-l  no logo
-p  preprocess source
-P  preprocess only (don't generate .int)
-F  write full file paths in "#line" directives
-O  optimize code (full optimization, see optimization.md)
-O<N>  set specific level of optimization (0 - none, 1 - basic, 2 - full, 3 - experimental)
-d  print debug messages
-D  output an abstract syntax tree of the program
-o  set output path (follows the input file name)
-s  enable short-circuit evaluation for all AND, OR operators
-m<macro>[=<val>]  define a macro named "macro" for conditional compilation
-I<path>  specify an additional directory to search for include files
```

The original command line option `-w` to turn on warnings no longer has an effect, since warnings are now on by default

---

### Additional supported syntax

Syntax which requires sfall for compiled scripts to be interpreted is marked by asterisk (*).

- Optional arguments in user-defined procedures. You can only use constants for default values. It basically puts those constants in place of omitted arguments.

  - new:
    ```
    procedure test(variable x, variable y := 0, variable z := -1) begin
    ...
    end
    ...
    call test("value");
    ```
  - old:
    ```
    procedure test(variable x, variable y, variable z) begin
    ...
    end
    ...
    call test("value", 0, -1);
    ```

- New logical operators `AndAlso`, `OrElse` for short-circuit evaluation of logical expressions.
  Using these operators allow the right part of logical expressions not to be evaluated (executed, computed) if the result is already known. This can improve the performance of running scripts.

  Example: `if (obj andAlso obj_pid(obj) == PID_STIMPAK) then ...`

  If `obj` is `null`, the second condition will not be checked and your script won't fail with "obj is null" error in debug.log

  This also has an effect that a value of last computed argument is returned as a result of whole expressions, instead of always `false` (0) or `true` (1):
  ```
  obj := false;
  display_msg(obj orElse "something");  // will print "something"
  ```
  You can also use the `-s` option to enable short-circuit evaluation for all the `AND`, `OR` operators in the script.

  __NOTE:__ Be aware that it may break some old scripts because operators behavior is changed slightly.

- Conditional expressions (Python-inspired), also known as ternary operator:
  - new:
    ```
    X := value1 if (condition) else value2
    ```
  - old:
    ```
    if (condition) then
      X := value1;
    else
      X := value2;
    ```

- To assign values, you can use the alternative assignment operator from **C/Java** instead of **Pascal** syntax.
  - new:
    ```
    x = 5;
    ```
  - old:
    ```
    x := 5;
    ```

- Multiple variable declaration: Multiple variables can be declared on one line, separated by commas. This is an alternative to the ugly begin/end block, or the bulky single variable per line style.
  - new:
    ```
    variable a, b, c;
    ```
  - old:
    ```
    variable begin a; b; c; end
    ```

- Variable initialization with expressions: You can now initialize local variables with complex expressions instead of constants.
  - new:
    ```
    variable tile := tile_num(dude_obj);
    ```
  - old:
    ```
    variable tile;
    tile := tile_num(dude_obj);
    ```
  __NOTE:__ If your expression starts with a constant (eg. `2 + 2`), enclose it in parentheses, otherwise compiler will be confused and give you errors.

- Hexadecimal numerical constants: Simply prefix a number with `0x` to create a hexadecimal. The numbers 0 to 9 and letters A to F are allowed in the number. The number may not have a decimal point.
  - new:
    ```
    a := 0x1000;
    ```
  - old:
    ```
    a := 4096;
    ```

- Increment/decrement operators: `++` and `--` can be used as shorthand for `+= 1` and `-= 1` respectively. They are merely a syntactic shorthand to improve readability, and so their use is only allowed where `+= 1` would normally be allowed.
  - new:
    ```
    a++;
    ```
  - old:
    ```
    a += 1;
    ```

- `break` & `continue` statements: They work just like in most high-level languages. `break` jumps out of the loop. `continue` jumps right to the beginning of the next iteration (see `for` and `foreach` sections for additional details).
  - new:
    ```
    while (i < N) begin
      // ...
      if (/* some condition */) then break;
      // ...
    end
    ```
  - old:
    ```
    while (i < N and not(breakFlag)) begin
      // ...
      if (/* condition */) then breakFlag := true;
      // ...
    end
    ```
  - new:
    ```
    for (i := 0; i < N; i++) begin
      // ...
      if (/* condition */) then begin
        // action
        continue;
      end
      // else actions
    end
    ```
  - old:
    ```
    for (i := 0; i < N; i++) begin
      // ...
      if (/* condition */) then begin
        // action
      end else begin
        // else actions
      end
    end
    ```

- `for` loops: Another piece of syntactic shorthand, to shorten `while` loops in many cases. Parentheses around the loop statements are recommended but not required (when not using parentheses, a semicolon is required after the 3rd loop statement).
  - new:
    ```
    for (i := 0; i < 5; i++) begin
      display_msg("i = " + i);
    end
    ```
  - old:
    ```
    i := 0;
    while (i < 5) do begin
      display_msg("i = " + i);
      i++;
    end
    ```
  __NOTE:__ `continue` statement in a `for` loop will recognize increment statement (third statement in parentheses) and will execute it before jumping back to the beginning of loop. This way you will not get an endless loop.

- `switch` statements: A shorthand way of writing big `if then else if...` blocks
  - new:
    ```
    switch get_attack_type begin
      case ATKTYPE_PUNCH: display_msg("punch");
      case ATKTYPE_KICK: display_msg("kick");
      default: display_msg("something else");
    end
    ```
  - old:
    ```
    variable tmp;
    tmp := get_attack_type;
    if tmp == ATKTYPE_PUNCH then begin
      display_msg("punch");
    end else if tmp == ATKTYPE_KICK then begin
      display_msg("kick");
    end else begin
      display_msg("something else");
    end
    ```

- Procedure stringify operator `@`: Designed to make callback-procedures a better option and allow for basic functional programming. Basically it replaces procedure names preceded by `@` by a string constant.
  - old:
    ```
    callbackVar := "Node000";
    call callbackVar;
    ```
  - new:
    ```
    callbackVar := @Node000;
    call callbackVar;
    ```
  Not many people know that since vanilla Fallout you can call procedures by "calling a variable" containing it's name as a string value. There was a couple of problems using this:
  - optimizer wasn't aware that you are referencing a procedure, and could remove it, if you don't call it explicitly (can be solved by adding making procedure `critical`)
  - you couldn't see all references of a procedure from a Script Editor
  - it was completely not obvious that you could do such a thing, it was a confusing syntax


- (*) **Arrays**: In vanilla Fallout, arrays had to be constructed by reserving a block of global/map variables. Since sfall 2.7, specific array targeted functions have been available, but they are fairly messy and long winded to use. The compiler provides additional syntactic shorthand for accessing and setting array variables, as well as for array creation. When declaring an array variable, put a constant integer in `[]`` to give the number of elements in the array. (before sfall 3.4 you had to specify size in bytes for array elements, now it's not required, see **arrays.md** for more information)
  - new:
    ```
    procedure bingle begin
      variable a[2];
      a[0] := 5;
      a[a[0] - 4] := a[0] + 4;
      display_msg("a[0]=" + a[0] + ", a[1]=" + a[1]);
    end
    ```
  - old:
    ```
    procedure bingle begin
      variable a;
      a := temp_array(2, 4);
      set_array(a, 0, 5);
      set_array(a, get_array(a, 0) - 4, get_array(a, 0) + 4);
      display_msg("a[0]=" + get_array(a, 0) + ", a[1]=" + get_array(a, 1));
    end
    ```

- (*) **Array expressions**: Sometimes you need to construct an array of elements and you will probably want to do it in just one expression. This is now possible:
  - new:
    ```
    list := ["A", "B", "C", "D"];
    ```
  - old:
    ```
    list := temp_array(4, 2);
    list[0] := "A";
    list[1] := "B";
    list[2] := "C";
    list[3] := "D";
    ```
  Syntax specific for associative arrays is also available. (see **arrays.md** for full introduction to this type of arrays).

- (*) **Map array expressions**:
  ```
  map := {5: "five", 10: "ten", 15: "fifteen", 20: "twelve"};
  ```

- (*) The dot `.` syntax to access elements of associative arrays and allow to work with arrays like objects:
  ```
  trap.radius := 3;
  trap.tile := tile_num(dude_obj);
  ```
  You can chain dot and bracket syntax to access elements of multi-dimensional arrays:
  ```
  collectionList[5].objectList[5].name += " foo";
  ```
  __NOTE:__ When using incremental operators like `+=`, `*=`, `++`, `--` compiler will use additional temp variable to get an array at penultimate level in order to avoid making the same chain of `get_array` calls twice.

- (*) `foreach` loops: A shorthand method of looping over all elements in an array. Syntax is `foreach (<symbol> in <expression>)`. You can declare variables in place by adding the `variable` keyword before the symbol name.
  - new:
    ```
    procedure bingle begin
      variable critter;
      foreach (critter in list_as_array(LIST_CRITTERS)) begin
        display_msg("" + critter);
      end
    end
    ```
  - old:
    ```
    procedure bingle begin
      variable begin critter; array; len; count; end
      array := list_as_array(LIST_CRITTERS);
      len := len_array(array);
      count := 0;
      while count < len do begin
        critter := array[count];
        display_msg("" + critter);
      end
    end
    ```

  If you want an index array element (or key for "maps") at each iteration, use syntax: `foreach (<symbol>: <symbol> in <expression>)`
  ```
  foreach (pid: price in itemPriceMap) begin
    if (itemPid == pid) then
      itemPrice := price;
  end
  ```

  If you want to add additional condition for continuing the loop, use syntax: `foreach (<symbol> in <expression> while <expression>)`. In this case loop will iterate over elements of an array until last element or until "while" expression is true (whatever comes first).

  __NOTE:__ Just like `for` loop, `continue` statement will respect increments of a hidden counter variable, so you can safely use it inside `foreach`.

---

### Fixes

- `playmoviealpharect` was using the token for `playmoviealpha`, breaking both functions in the process.
- `addbuttonflag` had an entry in the token table, and could be parsed, but was missing an entry in the emit list. This resulted in the compiler accepting it as a valid function, but not outputting any code for it into the compiled script.
- The function `tokenize` was missing an entry in the token table, and so would not be recognised by the compiler.
- Fixed the check for the `call "foo"` syntax so that non-string constants will no longer be accepted.

### Backward compatibility

There are several changes in this version of sslc which may result in problems for previously working scripts. A new command line option `-b` is available, which will turn off all new fixes and features which have the possibility of causing a previously compiling script to change its behaviour. (And only those features; anything which would not compile under the old sslc is not affected.) This includes the following:
- Since `for`, `foreach`, `break`, `continue`, `in` and `tokenize` are now hardcoded names, existing scripts that use any of them as a variable or procedure name will no longer compile.
- Missing a semicolon after a variable declaration is now a hard error. (Originally sslc would check for the semicolon, but would not complain if it was missing.)
- The function `addbuttonflag` used to be recognised by the compiler, but would not emit any code into the int file.
- The function `playmoviealpharect` compiled as `playmoviealpha`.

### int2ssl note

**int2ssl** by Anchorite (TeamX) is included in sfall modderspack package. It was updated to support all additional opcodes of sfall, along with some syntax features. You can use it to decompile any sfall or non-sfall script.

---

### Changelog

**sfall 4.4.7:**
- fixed leftover stack data caused by the `break` statement
- added Linux & WebAssembly builds

**sfall 4.4.4:**
- fixed stringified procedure names shifting when the same name is called more than once
- syntax improvements:
  - `foreach`: allow to declare variables for key & value header
  - `foreach`: allow to use expressions after `in` that start with a symbol without mandatory parentheses
  - variables: allow to declare variables anywhere a statement is expected
  - expressions: display useful error messages when an expression is expected but nothing is parsed

**sfall 4.4:**
- fixed compiler crash when trying to define an exported procedure with variables
- fixed compiler giving "symbol or string expected" error when trying to call procedure using a string literal
- fixed optimizer not treating `call string_variable` as variable use
- fixed unused arguments in a procedure being removed incorrectly by the optimizer
- fixed unused string literals in an optimized-out procedure not being removed by the optimizer
- fixed `get_array` syntax not working for exported variables
- fixed optional arguments not working for imported procedures
- fixed compiler crash when an exported procedure has no body

**sfall 4.2.9:**
- added support for additional universal opcodes `sfall_func7` and `sfall_func8`
- fixed a compilation error when the script has a UTF-8 BOM

**sfall 4.2.7:**
- added ability to declare local variables anywhere in the procedure body

**sfall 4.2.3:**
- fixed compiler giving "assignment operator expected" error when a variable-like macro is not defined properly
- added new logical operators `AndAlso`, `OrElse` for short-circuit evaluation of logical expressions
- added an alternative (C/Java-style) assignment operator `=`
- added support for new `div` operator (unsigned integer division)

**sfall 4.2.2:**
- added support for new opcode `reg_anim_callback`

**sfall 4.2.1:**
- the basic optimization is now enabled by default when not specifying any optimization options
- added `-m<macro[=val]>` option to define a macro named "macro" for conditional compilation
- added `-I<path>` option to specify an additional directory to search for include files
- unreferenced `critical` procedures and procedures with the names `Node998` and `Node999` are now removed by the optimizer

**sfall 4.2:**
- now it is possible to run preprocess or optimization passes in backward compatibility mode
- added support for new opcode `register_hook_proc_spec`

**sfall 4.0:**
- enabled code for `ceil` math function
- fixed missing argument for `how_much` function
- added `desc_p_proc` (from Fallout 1) to protected procedures that should not be removed by the optimizer
- fixed compiler giving "division by zero" error when using zero as the second factor in multiplication

**sfall 3.8:**
- added support for new universal opcodes `sfall_funcX`

**sfall 3.6:**
- added Python-style ternary operator (conditional expression)
- added `-s` short-circuit evaluation option for `AND`, `OR` expressions
- **int2ssl** will detect and decompile conditional expressions and short-circuit logical operators
- added `-F` option to include full file paths in `#line` directives after preprocessing
- added `-D` option to write abstract syntax tree into `.txt` file
- fixed compiler crash when number of arguments in procedure declaration does not match definition
- fixed incorrect constant folding of `bwnot` operator
- fixed more invalid results in constant folding
- implemented optional arguments for user-defined procedures
- implemented stringify procedure names using `@` operator, which is helpful to pass procedures around to call them from variables (it will properly handle references)
- logic for procedures passed as arguments to script functions was moved from code generation to parsing stage
- now it is possible to call user-defined procedures inside argument list of script functions, without compiler attempting to treat them as procedure references and probably fail (procedures will still be passed, but only to appropriate script functions at appropriate argument positions)
- **int2ssl** will now place empty parentheses after a call to user-defined procedure - this will distinct calls from passing procedures to some script functions (like `giq_option`)
- fixed `inline` procedure "calls" not working when optimization is enabled
- added column numbers to error/warning output
- added code to simplify adding sfall opcodes into compiler (need to add code in 3 places, instead of 7 places for each opcode)
- fixed bug when initializing variable with expression starting from a symbol
- added division by zero constant check

**sfall 3.5:**
- completed namespace compression optimization with respect to imported/exported variables
- changed `for` and `foreach` syntax to allow parentheses which are easier to read IMHO
- heavy code refactoring - split "parse.c" into several files, replaced all dirty workaround code in "lex()" (some syntax features) with parser-level equivalents
- added syntax to reference elements in multi-dimensional arrays (unlimited sequence of brackets `[]` and dots `.`)
- added fully featured `break` and `continue` statements for loops
- moved some optimizations (namely constant propagation and variable reuse) to "experimental" because they were breaking my scripts
- added ability to initialize variables with expressions

**sfall 3.4:**
- added `foreach .. while ..` syntax
- added array expressions for lists and maps
- added `foreach (key: value in ...)` syntax for convenience
- fixed crash bug in `for` loop parsing function
- added ability to access array elements with string keys using OOP-like dot `.` syntax
