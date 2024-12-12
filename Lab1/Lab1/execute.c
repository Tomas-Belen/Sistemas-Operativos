#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "tests/syscall_mock.h" //Para realizar los tests
#include <glib.h>

#include "execute.h"
#include "builtin.h"
#include "command.h"

static void execute_command (scommand comm) {
  assert (comm !=NULL);
  
  unsigned int length_comm = scommand_length (comm); //Cantidad argumentos
  unsigned int comml = strlen (scommand_front (comm)) + 1; //Longitud del argumento
  char **argv = calloc (length_comm + 1, sizeof (char *)); //Espacio de la cadena/arreglo para guardar el comando
  unsigned int i = 0; //Indice para desplazarse por el arreglo
  char * redir_out= scommand_get_redir_out(comm);
  char * redir_in = scommand_get_redir_in(comm);  //Aprende usar dup ()

  if (scommand_is_empty (comm)) {
    fprintf (stderr, "Porfavor inserte un comando.\n");
    return;
  }

  
  while (!scommand_is_empty(comm)) {
    argv [i] = calloc (comml , sizeof(char*)); //Asignacion de la memoria para el comm
    strcpy (argv [i], scommand_front (comm) ); //Copia con nulo, (destino, origen)
    scommand_pop_front (comm);
    i++;
  }

  argv [length_comm] = NULL; //Para el execvp
 
  if (redir_in) {
     int ri = open(redir_in, O_RDONLY, S_IRWXU); 
        close(STDIN_FILENO);   // open input redir instead of stdin
        dup2(ri, STDIN_FILENO);
        close(ri);
  }    

  if (redir_out) {
    int ro = open(redir_out, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);    
    close(STDOUT_FILENO);
    dup2(ro, STDOUT_FILENO);
    close (ro);
  }

  execvp (argv [0], argv);
}


void execute_pipeline(pipeline apipe) {
  assert (apipe!=NULL);

  unsigned int pipelength = pipeline_length (apipe); //Cuantos comandos hay

  if (pipeline_is_empty (apipe)) {
      return;
  }
  if (builtin_alone(apipe)) {
    if (builtin_is_internal(pipeline_front(apipe))) {
      builtin_run(pipeline_front(apipe));
      return;
    } else {
      execute_command(pipeline_front(apipe));
      return;
    }
  }
  
  bool background = !pipeline_get_wait(apipe);

  if (background) {
    signal(SIGCHLD, SIG_IGN); 
  }
  
  unsigned int * child_pid = malloc(pipelength * sizeof(unsigned int));
  //Se crea este arreglo para ir guardando los id de los procesos hijos

  int tmp[2];
  int fileno[2];

  for (unsigned int i = 0; i < pipelength ; ++i) {

    if (builtin_is_internal(pipeline_front(apipe))) {
      builtin_run(pipeline_front(apipe));
      pipeline_pop_front(apipe);
      return;
    }
    
    if (i != 0) {
      tmp[0] = fileno[0];
      tmp[1] = fileno[1];
    }
    if (i != pipelength - 1) {
            pipe(fileno);
    }

    int rc = fork();

    if (rc < 0) {
        fprintf(stderr, "FORK FAILED.\n");
        return;
    } else if (rc == 0) {
        if (i != pipelength - 1) {
          close(fileno[0]);
          close(STDOUT_FILENO);
          dup(fileno[1]);
          close(fileno[1]);
        }
        if (i != 0) {
          close(tmp[1]);
          close(STDIN_FILENO);
          dup(tmp[0]);
          close(tmp[0]);
        }
        char* command_str = scommand_to_string(pipeline_front(apipe));
        execute_command(pipeline_front(apipe));
        fprintf(stderr, "Error executing: %s\n", command_str);
        exit(EXIT_FAILURE);
        
    } else {
        if (i != 0) {
          close(tmp[0]);
          close(tmp[1]);
        }
        child_pid[i] = rc;
        pipeline_pop_front(apipe);
    }
  }

  if (!background) {
      for (unsigned int i = 0; i < pipelength; ++i){
            waitpid(child_pid[i], NULL, 0);
      }
    }

  free(child_pid);   
}
