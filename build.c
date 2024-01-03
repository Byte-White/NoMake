#define NOMAKE_IMPLEMENTATION
#include "nomake.h"


int main(int argc, char *argv[])
{
	const char* filelist[] = {
		"example.cpp",
		"build.c"
	};
	nomake_fwatcher* filewatcher = nomake_fwatcher_init();
	nomake_watch_files(filelist,2,filewatcher);
	
	nomake_builder* build = nomake_builder_init();
	nomake_select_compiler(build,"g++");
	nomake_add_flag(build,"example.cpp -o example.exe");
	nomake_compile(build);
	nomake_free_builder(build);
	return 0;
}
