## nomake - Simplified C/C++ Build Configuration

nomake is a header-only C/C++ library designed to simplify the build configuration process. 
With nomake, you can easily configure your project build, select compilers, add compilation flags,
and manage other build-related tasks directly in your code. 
This lightweight library streamlines the build process without the need for external build files.

## Getting Started

### Download and Integration

Begin by downloading the [nomake.h]("https://github.com/Byte-White/MAX7219-Dev/blob/main/nomake.h") and add it to your directory.

### Build File Setup

Create a build file,such as `build.c`, define `NOMAKE_IMPLEMENTATION` macro to include the library's implementation and include `nomake.h` header
```c
#define NOMAKE_IMPLEMENTATION
#include "nomake.h"


int main(int argc, char *argv[])
{
	nomake_builder* build = nomake_builder_init();
	nomake_select_compiler(build,"g++");
	nomake_add_flag(build,"main.cpp -o app.exe");
	nomake_compile(build);
	nomake_free_builder(build);
	return 0;
}
```

`main.cpp` contains a simple Hello World program.
```cpp
#include <iostream>

int main (int argc, char *argv[]) 
{
        std::cout << "hello world!";
	return 0;
}
```

compile your build file and run the `build` executable

`gcc build.c -o build`

![image](https://github.com/Byte-White/MAX7219-Dev/assets/51212450/64f3bacc-a8c4-4015-a813-d11edb55f8aa)
