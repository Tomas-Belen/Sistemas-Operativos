#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>


#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"

#include "obfuscated.h"

static char cur_directory[1100];

#include <stdio.h>
#include <unistd.h>

static void show_prompt(void) {
    char hostname[256];
    char *username = getenv("USER");
    
    // Verifica si `username` está disponible
    if (username == NULL) {
        username = getenv("LOGNAME"); // Alternativa en algunos sistemas
    }

    // Obtén el directorio actual
    char *prompt = getcwd(cur_directory, sizeof(cur_directory));
    
    // Obtén el nombre del host
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        hostname[0] = '\0'; // En caso de error, usamos una cadena vacía
    }

    // Verifica si `prompt` o `username` son NULL
    if (prompt == NULL || username == NULL) {
        printf("\x1b[31mERROR\x1b[0m $ ");
    } else {
        // Mostrar el nombre de usuario en verde, el nombre del host en cyan y el directorio en azul
        printf("\x1b[32m%s\x1b[0m@\x1b[36m%s\x1b[0m:\x1b[34m%s\x1b[0m$ ", username, hostname, cur_directory);
    }

    fflush(stdout);
}



int main(int argc, char *argv[]) {
    pipeline pipe;
    Parser input;
    bool quit = false;

    input = parser_new(stdin);
    while (!quit)  {
        //ping_pong_loop(NULL);
        show_prompt();
        pipe = parse_pipeline(input);
        /* 
        Hay que salir luego de ejecutar? 
        No hay que salir despues de ejecutar
        */
       if(pipe!=NULL) {
            execute_pipeline(pipe);
            pipeline_destroy(pipe);
        }
        quit = parser_at_eof(input);
    }
    parser_destroy(input); 
    input = NULL;
    return EXIT_SUCCESS;
}
