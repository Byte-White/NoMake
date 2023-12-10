#define NOMAKE_IMPLEMENTATION
#include "nomake.h"


int main(int argc, char *argv[])
{
	nomake_builder* build = nomake_builder_init();
	nomake_select_compiler(build,"g++");
	nomake_add_flag(build,"example.cpp -o example.exe");
	nomake_compile(build);
	nomake_free_builder(build);
	return 0;
}
