# Lession 1 - General knowledge

## 1 - Makefile
A Makefile is a file that automates repetitive tasks like compiling code, running scripts, or managing dependencies. It is commonly used in C/C++ projects, but it can be used for other tasks as well.  

The make program will read the content of the Makefile and execute it.  

Names of makefile are usually: Makefile, makefile, *.mk.  (When three makefile names are in the same project, if the execute ```make``` command, makefile will have priority to be executed first. However, normally, we name our makefile as "Makefile" with capital M to highlight the file amongs other files. Also, if no ```–f``` option is present, make will look for the makefile with name is Makefile or makefile.)

#### A Makefile structure

![alt text](_assets/makefileStructure.png "Makefile structure")


| Component    | Description                                                            |
| ------------ | ---------------------------------------------------------------------- |
| Target       | The file or action to create (e.g., an executable program).            |
| Dependencies | The files needed to build the target (e.g., source code files).        |
| Action       | The shell command(s) to run (must be indented with a TAB, not spaces). |

```
make [ -f makefile ] [ options ] ... [ targets ] ...
```
```
[options]
+-f file, --file=file, --makefile=FILE
    Use file as a makefile.
-i, --ignore-errors
    Ignore all errors in commands executed to remake files.
-n, --just-print, --dry-run, --recon
    Print the commands that would be executed, but do not execute them.
```
```
[targets]
all: make all the top-level targets the make file know about
info: Information of program
clean: Delete all files that are normally crated by running make
install: Copy the executable file into directory that users
typically search for commands
uninstall: Vice versa
tar: Create a tar file of the source files
test: Perform self tests on the program this Makefile builds.
```

#### Automatic variables

| Variable | Meaning                             |
| -------- | ----------------------------------- |
| $@       | The target filename.                |
| $<       | The first dependency.               |
| $^       | All dependencies.                   |
| $?       | Dependencies newer than the target. |
| $*       | The target without an extension.    |

#### Variables assignment

| Use Case                                                       | Best Operator               |
| -------------------------------------------------------------- | --------------------------- |
| When a variable needs to be evaluated every time it’s used     | = (Recursive Assignment)    |
| When a variable should be set once and not change              | := (Simple Assignment)      |
| When a variable should only be set if it’s not already defined | ?= (Conditional Assignment) |

#### Example

Make file contains the following information

```mk
var := "value equals 1"
var3 := "hello"

var1 = $(var)
var2 := $(var)
var3 ?= $(var)

var := "value changes to 2"

all: a.c b.c
    @echo $@
    @echo $<
    @echo $?
    @echo $^

rule3:
    @echo "$(var1)"
    @echo "$(var2)"
    @echo "$(var3)"
```

When we execuse the command:  
```
make all
```

We will have the following output
```
all
a.c
a.c b.c
a.c b.c
```

We can observe the following:  
- ```$@``` prints target of make command, in this case target is "all"
- ```$<``` prints the first dependency, which is "a.c" in this case
- ```$?``` and ```$^``` print all dependencies.

When we execuse the command:  
```
make rule3
```

We will have the following output

```
value changes to 2
value equals 1
hello
```

- Since ```var1``` uses recursive assignment, eventhough ```var``` is updated to "value changes to 2" after assigning ```var``` to ```var1```, ```var1``` still has the latest value of ```var```
- ```var2``` uses simple assignment, so its value is set once. So it prints "value equals 1"
- ```var3``` uses conditional assignment, so program will check if ```var3``` already had any values, if yes, ```var3 ?= $(var)``` will not be executed, and ```var3``` has value assigned to it from the begining. Otherwise, ```var3 ?= $(var)``` will become recursive assignment.

### 2 - The Process of Compiling C Program

![alt text](_assets/compilation-process-in-c.png "Compilation process in C")

#### 1. Preprocessing: The First Step
At the outset, the C preprocessor comes into play. It handles preprocessor directives, which begin with a '#' symbol, and includes header files in the code. When you use #include <stdio.h>, for example, the preprocessor replaces it with the content of the stdio.h file. This step generates an intermediate code with all the necessary declarations and macros.

#### 2. Compilation: Translating to Assembly
Once preprocessing is complete, the actual compilation commences. The compiler takes the C code and translates it into assembly code, specific to the target platform. Assembly code represents a low-level view of the program, consisting of instructions that the processor can execute.

#### 3. Assembly: From Assembly to Machine Code
Next, the assembler converts the assembly code into machine code, also known as object code. The object code consists of binary representations of instructions and data. This step is a crucial bridge between human-readable code and the language the computer understands.

#### 4. Linking: Bringing It All Together
Now that we have object files with machine code, the linker takes center stage. It resolves external dependencies, such as functions from libraries or other object files, and combines all the object files into a single executable. The linker ensures that the final program is self-contained and ready to run.

### 3 - Static library & Shared library

- **A static library** is a collection of precompiled code (functions, classes, etc.) that is directly included in your program when you compile it. When you compile your program, the code from the static library is copied into your executable file. This makes your executable file larger, but it doesn't need the library to run.  
**Example**: Imagine you have a book (your program) and you want to include a chapter from another book (the static library). You photocopy that chapter and add it to your book. Now, your book has everything it needs, and you don't need the other book anymore.

- **A shared library** is also a collection of precompiled code, but it is not included in your program when you compile it. Instead, your program links to the shared library at runtime. When you compile your program, it only includes references to the shared library, not the actual code. The shared library is loaded into memory when your program runs, and multiple programs can use the same shared library at the same time.  
**Example**: Imagine you have a book (your program) and you want to reference a chapter from another book (the shared library). Instead of copying the chapter, you just write down where to find it. When someone reads your book, they need to have the other book available to read that chapter.

|              | Static library                                                                                              | Shared library                                                                                                                                                      |
| ------------ | ----------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Size         | Static libraries make your executable larger because the library code is included in it.                    | Shared libraries keep your executable smaller because the library code is separate.                                                                                 |
| Dependencies | With static libraries, you don't need the library to run your program because the code is already included. | With shared libraries, you need the library to be available when your program runs.                                                                                 |
| Updates      | If a static library is updated, you need to recompile your program to include the new version.              | With shared libraries, you can just update the library, and all programs using it will automatically use the new version (as long as the interface doesn't change). |

**Static Library Example**:
You write a program my_program.c that uses a function from a static library libmath.a. When you compile my_program.c, the code from libmath.a is copied into the executable my_program. You can run my_program without needing libmath.a anymore.

**Shared Library Example**:
You write a program my_program.c that uses a function from a shared library libmath.so. When you compile my_program.c, the executable my_program only includes a reference to libmath.so. To run my_program, libmath.so must be available on your system.

- **Runtime** refers to the entire period when a program is running, from the moment it starts until it finishes. During runtime:
    - The program is loaded into memory.
    - Any shared libraries or dependencies are resolved and loaded.
    - The program executes its instructions.
    - The program interacts with the operating system, hardware, or other resources.
    - The program eventually terminates.
This is when the program is actively using the CPU, memory, and other resources. Runtime is about the environment and behavior of the program while it’s running.
- **Execution time** refers to the actual time taken by a program to run and complete its tasks. It is the period when the CPU is actively executing the program's instructions. Execution time is about performance measurement—how long it takes for the program to complete its tasks.
- **Compile time** is the phase when your source code (written in a high-level language like C, C++, or Java) is translated into machine code (binary code) by a compiler.

