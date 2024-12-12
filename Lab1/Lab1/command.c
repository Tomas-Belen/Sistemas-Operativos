#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <stdbool.h>

#include "command.h"
#include "strextra.h"


struct scommand_s {
	GSList * commd;
	char * input;
	char * output;
	unsigned int length;
};

scommand scommand_new(void) {
	scommand new = malloc(sizeof(struct scommand_s));
	assert(new != NULL);
	new->commd = NULL;
	new->input = NULL;
	new->output = NULL;
	new->length = 0u;
	assert(new != NULL && scommand_is_empty(new) && scommand_get_redir_in (new) == NULL && scommand_get_redir_out (new) == NULL);
	
    return new;
}

scommand scommand_destroy(scommand self) {
	assert(self != NULL);
	g_slist_free_full(self->commd, g_free);
	free(self->input);
	free(self->output);
	free(self);
	self = NULL;
	assert(self == NULL);
	
	return self;
}

bool scommand_is_empty(const scommand self) {
	assert(self != NULL);
	bool b = (self->length == 0);
	
	return b;
}

void scommand_push_back(scommand self, char * argument) {
	assert(self != NULL && argument != NULL);

	self->commd = g_slist_append(self->commd, argument);
	++(self->length);
}


void scommand_pop_front(scommand self) {
	assert(self != NULL && !scommand_is_empty(self));

	gpointer killme = g_slist_nth_data(self->commd, 0);
	self->commd = g_slist_remove(self->commd, killme);
	free(killme);
	--(self->length);
}

void scommand_set_redir_in(scommand self, char * filename) {
	assert(self != NULL);

	free(self->input);
	self->input = filename;
}


void scommand_set_redir_out(scommand self, char * filename) {
	assert(self != NULL);

	free(self->output);
	self->output = filename;
}

unsigned int scommand_length(const scommand self) {
	assert(self != NULL);

	unsigned int lenght = self->length;
	assert((lenght == 0) == scommand_is_empty(self));
	
	return lenght;
}

char * scommand_front(const scommand self) {
	assert(self != NULL && !scommand_is_empty(self));

	char * res = g_slist_nth_data(self->commd, 0);

	assert(res != NULL);

	return res;
}

char * scommand_get_redir_in(const scommand self) {
	assert(self != NULL);

	return self->input;
}

char * scommand_get_redir_out(const scommand self) {
	assert(self != NULL);

	return self->output;
}

char * scommand_to_string(const scommand self) {
	assert(self != NULL);

	unsigned int res_length = 0u;
	//Se crea este arreglo para guardar el largo de las cadenas de los comandos
	unsigned int * chain_length = calloc(self->length, sizeof(unsigned int));

	GSList * com_elem = g_slist_copy(self->commd);

	for (unsigned int i = 0; i < self->length; ++i) {
		chain_length[i] = strlen((char*)g_slist_nth_data(com_elem, 0));

		res_length += chain_length[i] + 1;
		com_elem = g_slist_remove(com_elem, g_slist_nth_data(com_elem, 0));
	}

	g_slist_free_full(com_elem, g_free);

	unsigned int input_long = 0u;
	unsigned int output_long = 0u;

	if (self->input) {
		input_long = strlen(self->input);

		res_length += input_long + 3;
	}

	if (self->output) {
		output_long = strlen(self->output);

		res_length += output_long + 3;
	}

	//Si no hay comando, se retorna un string vacío 
	if (res_length == 0) {
		free(chain_length);
		return calloc(1, sizeof(char));
	}

	char * com_to_string = calloc(res_length, sizeof(char));

	unsigned int copy = 0u;
	com_elem = g_slist_copy(self->commd);
	for (unsigned int i = 0; i < self->length; ++i) {
		char * com_data = (char*)g_slist_nth_data(com_elem,0);
		strcpy(com_to_string + copy, com_data);
		copy += chain_length[i] + 1;
		com_to_string[copy - 1] = ' ';
		com_elem = g_slist_remove(com_elem, g_slist_nth_data(com_elem, 0));
	}

	g_slist_free_full(com_elem, g_free);

	if (self->input) {
		com_to_string[copy] = '<';
		com_to_string[copy + 1] = ' ';
		strcpy(com_to_string + copy + 2, self->input);
		copy += input_long + 3;
		com_to_string[copy - 1] = ' ';
	}

	if (self->output) {
		com_to_string[copy] = '>';
		com_to_string[copy + 1] = ' ';
		strcpy(com_to_string + copy + 2, self->output);
		copy += output_long + 3;
		com_to_string[copy - 1] = ' ';
	}

	com_to_string[copy-1] = 0;

	free(chain_length);
	assert(scommand_is_empty(self) ||
    scommand_get_redir_in(self)==NULL || scommand_get_redir_out(self)==NULL ||
    strlen(com_to_string)>0);
	
	return com_to_string;
}


struct pipeline_s {
	GSList * commds;
	bool go_backgr;
	unsigned int length;
};

pipeline pipeline_new(void) {
	pipeline new_pipe = malloc(sizeof(struct pipeline_s));
	assert(new_pipe != NULL);
	new_pipe->commds = NULL;
	new_pipe->go_backgr = true;
	new_pipe->length = 0u;
	assert(pipeline_is_empty(new_pipe) && pipeline_get_wait(new_pipe));
	
	return new_pipe;
}

pipeline pipeline_destroy(pipeline self) {
	assert(self != NULL);
	
	while(self->commds != NULL) {
		scommand tmp = g_slist_nth_data(self->commds, 0);
		tmp = scommand_destroy(tmp);
		self->commds = g_slist_remove(self->commds, g_slist_nth_data(self->commds, 0));	
	}

	g_slist_free(self->commds);
	free(self);
	self = NULL;
	assert(self == NULL);
	
	return self;
}

void pipeline_push_back(pipeline self, scommand sc) {
	assert(self != NULL && sc != NULL);

	self->commds = g_slist_append(self->commds, sc);
	self->length++;
}

bool pipeline_is_empty(const pipeline self) {
	assert(self != NULL);
	
	bool b = (self->length == 0);
	return b;
}

unsigned int pipeline_length(const pipeline self) {
	assert(self != NULL);

	unsigned int length = self->length;

	assert((self->length == 0) == pipeline_is_empty(self));

	return length;
}

scommand pipeline_front(const pipeline self) {
	assert(self != NULL && !pipeline_is_empty(self));

	scommand command = g_slist_nth_data(self->commds, 0);

	assert(command != NULL);

	return command;
}

bool pipeline_get_wait(const pipeline self) {
	assert(self != NULL);

	return self->go_backgr;
}

void pipeline_set_wait(pipeline self, const bool w) {
	assert(self != NULL);

	self->go_backgr = w;
}

void pipeline_pop_front(pipeline self) {
	assert(self != NULL && !pipeline_is_empty(self));

	scommand command_to_kill = g_slist_nth_data(self->commds, 0);
	self->commds = g_slist_remove(self->commds, command_to_kill);

	scommand_destroy(command_to_kill);
	--(self->length);
}

char * pipeline_to_string(const pipeline self) {
	assert(self != NULL);

	//Si no hay pipe se devuelve un string vacío 
	if (self->length == 0) {
		return calloc(1, sizeof(char));
	}

	//Se guardan todos los comandos
	char ** comand_to_string = calloc(self->length, sizeof(char*));

	unsigned int pos = 0u;

	GSList * command = g_slist_copy(self->commds);

	for (unsigned int i = 0; i < self->length; ++i) {
		comand_to_string[i] = scommand_to_string(g_slist_nth_data(command, 0));
		command = g_slist_remove(command, g_slist_nth_data(command, 0));
		pos += strlen(comand_to_string[i]);
	}

	g_slist_free_full(command, g_free);

	//Se ve si la pipeline requiere ir a ejecutarse en background

	pos += !(self->go_backgr) * 2;

	char * result = calloc(pos + (self->length-1)*2 + 1, sizeof(char));
	//Ahora transformo la pipeline a una "emulación" de como debería verse en la shell
	for (unsigned int i = 0; i < self->length; ++i) {
		char * tmp = result;
		//mezclo lo que hay en result con los comandos guardados
		result = strmerge(tmp, comand_to_string[i]);
		//Libero la variable temporal
		free(tmp);

		//Si el último comando no agrego una pipe "|"
		if (i != self->length - 1) {
			tmp = result;
			result = strmerge(tmp, "|");
			free(tmp);
		} else if (!self->go_backgr) {
			//Si se da este caso, agrego "&" para que corra en background
			tmp = result;
			result = strmerge(tmp, "&");
			free(tmp);
		}

		free(comand_to_string[i]);
	}

	free(comand_to_string);
	assert(pipeline_is_empty(self) || pipeline_get_wait(self) || strlen(result)>0);
	
	return result;
}