#ifndef NOMAKE_H_
#define NOMAKE_H_
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

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



nomake_cvec* nomake_cvec_init();
void nomake_cvec_push_back(nomake_cvec* v,nomake_command c);
void nomake_free_cvec(nomake_cvec* vec);



nomake_builder* nomake_builder_init();
void nomake_free_builder(nomake_builder* b);
void nomake_select_compiler(nomake_builder* b,const char* cc);
void nomake_add_flag(nomake_builder* b,const char* cmd);
void nomake_compile(nomake_builder* b);


//-------------------IMPLEMENTATION-------------------

#ifdef NOMAKE_IMPLEMENTATION

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
    b->cc = strdup(cc);
}

void nomake_add_flag(nomake_builder* b,const char* flag)
{
	nomake_log_info("added flag '%s'",flag);
	nomake_cvec_push_back(b->data,strdup(flag));
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

#endif
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif // !NOMAKE_H_
