#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>
#include <sys/types.h> 
#include <sys/wait.h> 

#define _GNU_SOURCE
#define DEFAULTLIB "/usr/lib/libc.so" //BSD
#define SCRIPT "/path/to/maildir_creator"

int (*old_chdir)(const char *path);
void *hh;
int flag = 0;

void _init()
{
   	hh = dlopen(DEFAULTLIB,RTLD_LAZY);
   	old_chdir = (int (*)(const char *)) dlsym(hh,"chdir");
}

void _fini()
{
	dlclose(hh);
}

int chdir(const char *path) {
	pid_t pid;
	int status, died;
	if (!flag) {
		if ( access( path, F_OK ) == -1 ) {
			switch (pid = fork()) {
				case -1: perror("fork");
				case 0: execl(SCRIPT,SCRIPT,path,NULL);
				default: died = wait(&status);
					 flag=!flag;
			}
			flag=!flag;
		}
	}
	return (*old_chdir)(path);
}
