#include <stdlib.h>
#include <stdbool.h>

#include "parsing.h"
#include "parser.h"
#include "command.h"

static scommand parse_scommand(Parser p) {
    scommand nc = scommand_new();
    arg_kind_t arg_type;
    char *arg = parser_next_argument(p, &arg_type);
    
    while (arg != NULL) {
       
        if (arg_type == ARG_NORMAL) {
            scommand_push_back(nc, arg);
        }
        else if (arg_type == ARG_INPUT)
        {
            scommand_set_redir_in(nc, arg);
        }
        else if (arg_type == ARG_OUTPUT)
        {
           scommand_set_redir_out(nc, arg);
        }
        
        arg = parser_next_argument(p, &arg_type);
    }
    
    //contemplamos en el caso de tener redirecciones vacias
    if(arg_type == ARG_INPUT && arg == NULL)
    {
        fprintf(stderr,"Error in the redirection of the input file\n");
        scommand_destroy(nc);
        return NULL;
    }
    
    
    if(arg_type == ARG_OUTPUT && arg == NULL)
    {
        fprintf(stderr,"Error in the redirection of the output file\n");
        scommand_destroy(nc);
        return NULL;
    }

    if (arg_type == ARG_INPUT || arg_type == ARG_OUTPUT || scommand_is_empty(nc)) {
        nc = scommand_destroy(nc);
        nc = NULL;
    }

    return nc;
}

pipeline parse_pipeline(Parser p) {
    if (parser_at_eof(p) || p == NULL) {
        fprintf(stderr, "Unable to start de parsing from sdtin");
        exit(EXIT_FAILURE);
    }

    pipeline result = pipeline_new();
    scommand cmd = NULL;
    bool error = false, another_pipe=true;
    bool is_background; //Bool opcional para background
    
    while (another_pipe && !error) {

        cmd = parse_scommand(p);
        error = (cmd==NULL); //Comando inv�lido al empezar
    
        if (!error) {
            pipeline_push_back(result, cmd);

            parser_op_pipe(p, &another_pipe);
        } 
        
        // Contempla el error de un & luego de un |
        if(pipeline_length(result) == 1 && another_pipe)
        {
            parser_op_background(p, &is_background);
            if(is_background)
            {
                 fprintf(stderr,"This error again? please the Syntax\n");
                 result = pipeline_destroy(result);
                 result = NULL;
                 return result;
             }
         }
    }

    //Si no termina, busca un &
    if (!parser_at_eof(p)) {
            
        parser_op_background(p, &is_background);

        //Se indica si debe esperar
        pipeline_set_wait(result, !is_background); 

        //Una vez pasado el &,se borran los espacios en blanco
        parser_skip_blanks(p);
        
        // Revisamos si aparece un | luego de un &
        if(pipeline_length(result) == 1 && is_background)
        {
            parser_op_pipe(p, &another_pipe);
            if(another_pipe)
            {
                 fprintf(stderr,"Invalid command syntax, please respect the syntax\n");
                 result = pipeline_destroy(result);
                 result = NULL;
                 return result;
             }
         }
        parser_garbage(p, &error);
    }
    // Contemplamos el error de un pipe en solitario
    if(pipeline_length(result) == 1 && another_pipe)
        {    
                 fprintf(stderr,"Invalid command syntax, please respect the syntax\n");
                 result = pipeline_destroy(result);
                 result = NULL;
                 return result;
         }
    
    
    if (error || pipeline_is_empty(result)) {
        result = pipeline_destroy(result);
        result = NULL;
    }

    return result;
}


