#ifndef NOMAKE_H_
#define NOMAKE_H_
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>  
#include <inttypes.h>

#ifdef _WIN32
#include <windows.h>
#endif // windows


#ifdef __cplusplus
extern "C" {
#endif


void nomake_log_info(const char* format,...);
void nomake_log_error(const char* format,...);
void nomake_log_warn(const char* format,...);

#define NOMAKE_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            nomake_log_error("%s", message); \
            exit(EXIT_FAILURE); \
        } \
    } while(0)


typedef struct nomake_builder nomake_builder;
typedef  char* nomake_command;//typedef struct nomake_command nomake_command;
typedef struct nomake_cvec nomake_cvec;
typedef struct nomake_fwatcher nomake_fwatcher;
//typedef struct nomake_fwatcherpayload nomake_fwatcherpayload;



nomake_cvec* nomake_cvec_init();
void nomake_cvec_push_back(nomake_cvec* v,nomake_command c);
void nomake_free_cvec(nomake_cvec* vec);



nomake_builder* nomake_builder_init();
void nomake_free_builder(nomake_builder* b);
void nomake_select_compiler(nomake_builder* b,const char* cc);
void nomake_add_flag(nomake_builder* b,const nomake_command* cmd);
void nomake_compile(nomake_builder* b);

nomake_fwatcher* nomake_fwatcher_init();
nomake_fwatcher* nomake_free_fwatcher();
// changes the members of the structure
void nomake_parsewatchedfileslist(nomake_fwatcher* fwatcher,const char* line);
// changes the members of the structure
void nomake_checkwatchedfiles(nomake_fwatcher* fwatcher,int count);
// returns the number of changed files if there are any
// returns -1 on error
int nomake_watch_files(const char** files,int count,nomake_fwatcher* fwatcher);
//nomake_fwatcherpayload nomake_getnombuildpayload();

void nomake_generatenombuild(nomake_fwatcher* fwatcher,int count);

// gets the size of content of a file in bytes
// returns -1 on error
long nomake_getfilesize(const char* filename);

// reads the whole file, writes it to a buffer and returns the size of the file
int nomake_read_whole_file(const char* filename,char** buf);


#ifdef __cplusplus
} /* extern "C" */
#endif // /NOMAKE_IMPLEMENTATION

#endif // !NOMAKE_H_


//-------------------IMPLEMENTATION-------------------

#ifdef NOMAKE_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

/* might be used in the future
struct nomake_command{
	char* flags;
};
*/

struct nomake_cvec{
	nomake_command* array;
	size_t capacity;
	size_t count;
};

struct nomake_fwatcher{
	char** watched_files;
	char** changed_files;
	int watched_count, changed_count, should_build;
};

struct nomake_builder {
	char* cc;
	nomake_cvec* data;
};



void nomake_log_info(const char* format,...)
{
	va_list args;
	va_start(args,format);
	
	
    	time_t rawtime;
    	struct tm* timeinfo;
    	time(&rawtime);
    	timeinfo = localtime(&rawtime);

	printf("\x1B[32m[%02d:%02d:%02d](nomake_log): ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	vprintf(format,args);
	printf("\x1B[0m\n"); // reset color
	va_end(args);
}


void nomake_log_error(const char* format,...)
{
	va_list args;
	va_start(args,format);
	
	
    	time_t rawtime;
    	struct tm* timeinfo;
    	time(&rawtime);
    	timeinfo = localtime(&rawtime);

	printf("\x1B[31m[%02d:%02d:%02d](nomake_error): ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	vprintf(format,args);
	printf("\x1B[0m\n"); // reset color
	va_end(args);
}


void nomake_log_warn(const char* format,...)
{
	va_list args;
	va_start(args,format);
	
	
    	time_t rawtime;
    	struct tm* timeinfo;
    	time(&rawtime);
    	timeinfo = localtime(&rawtime);

	printf("\x1B[33m[%02d:%02d:%02d](nomake_warning): ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	vprintf(format,args);
	printf("\x1B[0m\n"); // reset color
	va_end(args);
}

// string util functions

char *strdublicate(const char *s) {
    size_t size = strlen(s) + 1;
    char *p = malloc(size);
    if (p) {
        memcpy(p, s, size);
    }
    return p;
}


int nomake_run_cmd(const char* cmd)
{
    int exit_code = -1; 
	#ifdef _WIN32
		exit_code = system(cmd);
	#elif __unix__
		exit_code = system(cmd);
	#else 
	#error "unknown os."
	#endif
	return exit_code;
}

nomake_cvec* nomake_cvec_init()
{
	nomake_cvec* vec = malloc(sizeof(nomake_cvec));
	vec->array=malloc(sizeof(nomake_command));
	NOMAKE_ASSERT(vec->array!=0,"failed to allocate memory for commands vector!");
	vec->capacity = 1;
	vec->count = 0;
	return vec;
}

void nomake_cvec_push_back(nomake_cvec* v,nomake_command c)
{
	if(v->count==v->capacity)
	{ //reallocate data
		v->capacity *= 2;
		v->array = (nomake_command*)realloc(v->array,v->capacity*sizeof(nomake_command));
		NOMAKE_ASSERT(v->array!=0,"failed to reallocate memory for commands vector!");
	}
	v->array[v->count++] = c;
}

// void nomake_vec_pop_back() // not needed

void nomake_free_cvec(nomake_cvec* vec)
{
	for(int i = 0;i<vec->count;i++)
		free(vec->array[i]);

	free(vec->array);
	free(vec);
}

// substring until new line
void substringnl(const char *input, char *output, size_t output_size) 
{
    const char *newline_pos = strchr(input, '\n');

    if (newline_pos != NULL) {
        size_t substring_length = newline_pos - input;

        if (substring_length < output_size) {
            strncpy(output, input, substring_length);
            output[substring_length] = '\0';  
        } else {
            nomake_log_error("Output buffer is too small. (substringnl)");
            strncpy(output, input, output_size - 1);
            output[output_size - 1] = '\0';  
        }
    } else {
        strncpy(output, input, output_size - 1);
        output[output_size - 1] = '\0';  
    }
}

//----------------------------------------------

nomake_builder* nomake_builder_init()
{
	nomake_builder* b = (nomake_builder*)malloc(sizeof(nomake_builder));
	b->data = nomake_cvec_init();
	return b;
}

void nomake_free_builder(nomake_builder* b)
{
    nomake_free_cvec(b->data);
	free(b->cc);
    free(b);
}


void nomake_select_compiler(nomake_builder* b,const char* cc)
{
	nomake_log_info("selected compiler '%s'",cc);
    b->cc = strdublicate(cc);
}

void nomake_add_flag(nomake_builder* b,const nomake_command* flag)
{
	nomake_log_info("added flag '%s'",flag);
	nomake_cvec_push_back(b->data,strdublicate(flag));
}

void nomake_compile(nomake_builder* b)
{
	nomake_log_info("building project");
	nomake_cvec* itr = b->data;
	for(int i = 0;i<itr->count;i++)
	{
		char* cmd = malloc(strlen(b->cc) + strlen(itr->array[i]) + 2); // 2 character for the space inbetween and null character
		NOMAKE_ASSERT(cmd!=0,"couldn't allocate memory for cmd.");
		sprintf(cmd,"%s %s",b->cc,itr->array[i]);
		int r = nomake_run_cmd(cmd);
		if(r!=0)
		{
			nomake_log_error("command '%s' failed with code %f",cmd,r);
			exit(1);
		}
		
		free(cmd);
	}
}
// ------------------------- CRC-64 -------------------------

#define NOMAKE_CRC_POLY 0xC96C5795D7870F42ULL // CRC-64-ISO polynomial

uint64_t nomake_crc64(const void *data, size_t size) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint64_t crc = 0xFFFFFFFFFFFFFFFFULL;

    for (size_t i = 0; i < size; ++i) {
        crc ^= bytes[i];
        for (int j = 0; j < 8; ++j) {
            crc = (crc >> 1) ^ ((crc & 1) ? NOMAKE_CRC_POLY : 0);
        }
    }

    return crc ^ 0xFFFFFFFFFFFFFFFFULL;
}
// ------------------------/ CRC-64 /------------------------

nomake_fwatcher* nomake_fwatcher_init()
{
	nomake_fwatcher* fwatcher = (nomake_fwatcher*)malloc(sizeof(nomake_fwatcher));
	fwatcher->changed_count = 0;
	fwatcher->changed_files = 0;
	fwatcher->should_build = 0;
	fwatcher->watched_count = 0;
	fwatcher->changed_count = 0;

	return fwatcher;
}

long nomake_getfilesize(const char* filename)
{
	FILE* file = fopen(filename,"r");
	if(file)
	{
		fseek(file, 0, SEEK_END);
		long size = ftell(file);
		fclose(file);
		return size;
	}
	else 
		nomake_log_error("file '%s' not found.",filename);
	
	return -1;
}

void nomake_generatenombuild(nomake_fwatcher* fwatcher, int count)
{
	nomake_log_info("generating a '%s' file.",".nombuild");
	FILE* file = fopen(".nombuild","w");
	if(!file) 
	{
		nomake_log_error("could not write to file '.nombuild'.");
		return;
	}
	for(int i = 0; i<count;i++)
	{
		if(i!=count-1) // TODO: add %s:%d| where %d is nomake_getfilesize(fwatcher->watched_files[i])
			fprintf(file,"%s|",fwatcher->watched_files[i]);
		else
			fprintf(file,"%s",fwatcher->watched_files[i]);
	}

	for(int i = 0;i<count;i++)
	{
		char* content;
		if(nomake_read_whole_file(fwatcher->watched_files[i],content)==-1)
		{
			nomake_log_error("bad file '%s' (nomake_generatenombuild)",fwatcher->watched_files[i]);
			return;
		}
		fprintf(file,"%llu ",nomake_crc64(content,strlen(content)));
	}
	fclose(file);
}

// checks '.nombuild' for files
// will reset the file if count is different
void nomake_checkwatchedfiles(nomake_fwatcher* fwatcher, int count)
{
	FILE* file = fopen(".nombuild","r");
	char buffer[512];
	if(fgets(buffer, sizeof(buffer), file)!=NULL)
	{
		char* ptr;
		ptr = strtok(buffer, "|");
		int index = 0;
		int regen = 0; // 1 if file should be regenerated
		while (ptr != NULL)
		{
			if(index>fwatcher->watched_count) 
			{
				regen = 1;
			}
			if(regen==0 &&(strlen(fwatcher->watched_files[index]) == strlen(ptr)))
			{
				if(strncmp(fwatcher->watched_files[index],ptr,strlen(ptr))==0) regen = 1;
			}
			else regen = 1; // if strings have different length
			ptr = strtok (NULL, "|");
			index++;
		}  
		if(regen == 1)
		{
			// index should be the count of files
			nomake_generatenombuild(fwatcher,index);
			fwatcher->should_build = 1;
		}
	}
	else
	{
		nomake_log_error("bad '.nombuild' file check.");
	}
}


void nomake_parsewatchedfileslist(nomake_fwatcher* fwatcher,const char* line)
{
	char *ptr;
	int size = 0;
	ptr = strtok(line, "|");
	fwatcher->watched_files = malloc(fwatcher->watched_count * sizeof(char*));
	int index = 0;
	while (ptr != NULL)
	{
		fwatcher->watched_files[index] = strdublicate(ptr);
		ptr = strtok (NULL, "|");
		index++;
	}  
	fwatcher->watched_files;
}


int nomake_watch_files(const char** files,int count,nomake_fwatcher* fwatcher)
{
	char* content;
	int result = nomake_read_whole_file(".nombuild",&content);
	// file is not created
	if(result == -1)
	{
		fwatcher->watched_files = files;
		nomake_generatenombuild(fwatcher,count);
		return 0;
	}
	else
	{
		char line[512];

		substringnl(result,line,512);
		nomake_parsewatchedfileslist(fwatcher,line);
		nomake_checkwatchedfiles(fwatcher,count);

		// offset to file content
		char* crc_filecontent = result + strlen(line);
		uint64_t crc;
		for(int i = 0;i<fwatcher->watched_count;i++)
			{
				sscanf(crc_filecontent,"%llu",&crc);
				char* filecontent;
				nomake_read_whole_file(fwatcher->watched_files[i],filecontent);
				if(crc != nomake_crc64(filecontent,strlen(filecontent)))
				{
					// TODO: add files to fwatcher->changed_files
				}
			}
	}
	return 0;
}


// returns -1 if file could not be opened
// returns 0 on other errors
// allocates memory
int nomake_read_whole_file(const char* filename,char** buf)
{
	FILE* file;
	file = fopen(filename, "r");

	if(file == NULL)
	{
		nomake_log_warn("could not load file '%s'.",filename);
		return -1;
	}
    // Seek to the end of the file to get its size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
	if (file_size == -1) 
	{
        nomake_log_error("Error getting size of '%s'.",filename);
        fclose(file);
        return 0;
    }
	char* str = (char*)malloc(sizeof(char)*(file_size+1));

    size_t read_size = fread(str, sizeof(char), file_size, file);
	if(read_size != file_size)
	{
		nomake_log_warn("could not read the whole file. %d bytes read, %d bytes expected.",read_size,file_size);
        fclose(file);
        free(str);
        return 0;
	}
	fclose(file);
	*buf = str;
	return file_size;
}



#ifdef __cplusplus
} /* extern "C" */
#endif // /NOMAKE_IMPLEMENTATION
#endif
