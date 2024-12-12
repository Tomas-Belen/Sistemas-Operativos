#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include "builtin.h"
#include "parsing.h"
#include "command.h"
#include "tests/syscall_mock.h"

#define cmdopsamount 3

bool builtin_is_internal(scommand cmd) {
    assert(cmd != NULL);
    bool b = false;
    char *nc = NULL;

    const char *cmdoptions[] = {"cd", "exit","help"}; 
    if(!scommand_is_empty(cmd)) {
        nc = scommand_front(cmd);

        for(size_t i = 0u; i < cmdopsamount && !b; i++) {
            b = b || !strcmp(cmdoptions[i], nc);
        }

    }


    return b;
}



bool builtin_alone(pipeline p) {
    assert(p != NULL);
    
    bool b;    
    b = pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p));
    
    return b;
}


static void built_cd(scommand c) {
    scommand_pop_front(c);  

    char home_dir[] = "/home/";                      

    if (scommand_length(c) == 0) {
        char * user_id = getlogin();
        char * user_dir = malloc(sizeof(home_dir) + sizeof(user_id));

        strcpy(user_dir, home_dir);
        strcat(user_dir, user_id);

        scommand_push_back(c, user_dir);
    }

    int chdir_result = chdir(scommand_front(c));

    if (chdir_result == -1) {
        switch (errno) {
            case(EACCES): fprintf(stderr,"Permission denied on directory open\n");
                            break;
		
		    case(ENOENT): fprintf(stderr,"No such file or directory\n");
                            break;
		
		
		    case(ENOTDIR): fprintf(stderr,"Path does not refer to a directory\n");
                            break;

            case(EFAULT): fprintf(stderr, "This path is over the range of the memory\n");
                            break;
	
            default: fprintf(stderr,"Unknown error, unable to change directory\n");
                            break;
        }
    }
    


    
}

static void built_help(void) {
		fprintf(stdout,"Shell made by OPERATIVOS: Belen Tomas, Figueroa Jeremias, Ramirez Diego Alexis, Ferreyra Joaquin Ignacio\n"
				"This shell includes 3 basic commands whose ID's and utilities are:\n"
                "cd <directory>: Access to <directory>\n"
				"exit: Puts an end to the interaction between user and shell\n"
                "help: Provides information about the shell commands\n" 		
		);
        return;
}


static void built_exit(void) {
	fprintf(stdout,"Exiting shell...\n");
    exit(EXIT_SUCCESS);
}



void builtin_run(scommand cmd) {
    assert(builtin_is_internal(cmd));
    
    char *nc = scommand_front(cmd);
    
    if(!strcmp(nc,"cd") ) { 
    	built_cd(cmd);
    }
    else if(!strcmp(nc,"help")) {
    	built_help();
    }
    else if(!strcmp(nc,"exit")) {
    	built_exit();
    }

}
