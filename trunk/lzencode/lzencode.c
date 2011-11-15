/*
 * lzencode.c
 *
 *  Created on: Oct 13, 2011
 *      Author: ronie
 */

#include "lzencode.h"

struct nd {
	int index;
	char string[TAM_MAX_STRING];
	int prefix;
	char new_simbol;
	struct nd *next;
} typedef node;

void initialize_list(node **list) {
	*list = NULL;
}

void insert_node(node **list, node *n) {
#ifdef DEBUG
	printf("Inserindo elemento %d \"%s\" (%d|\"%c\") ...\n", n->index, n->string, n->prefix, n->new_simbol);
#endif
	node *p = *list;

    if (p == NULL) {
    	*list = n;
    } else {
    	while (p->next != NULL) {
    		p = p->next;
    	}

    	p->next = n;
    }
}

int is_string_found(node *list, char *string) {
	node *p = list;

	if (p == NULL) {
		return 0;
	} else {
		while (p != NULL) {
			if (strcmp(p->string, string) == 0) {
				return 1;
			}

			p = p->next;
		}

		return 0;
	}
}

int find_index(node *list, char *string) {
	node *p = list;

	if (p == NULL) {
		return 0;
	} else {
		while (p != NULL) {
			if (strcmp(p->string, string) == 0) {
				return p->index;
			}

			p = p->next;
		}

		return 0;
	}
}

char* get_prefix(char string[]) {
	string[strlen(string)-1] = '\0';
	return string;
}

void create_dictionary(node **list) {
	static int index = 1;

	initialize_list(list);

	char simbol;
	char string[TAM_MAX_STRING] = "";
	while (!feof(infile)) {
		/* lendo próximo símbolo */
		fread(&simbol, sizeof(char), 1, infile);

		/* concatenado símbolo à string */
		string[strlen(string)+1] = '\0';
		string[strlen(string)] = simbol;

		if (!is_string_found(*list, string)) {
			node *n = calloc(1, sizeof(node));
			n->index = index++;
			strcpy(n->string, string);
			n->prefix = find_index(*list, get_prefix(string));
			n->new_simbol = simbol;
			insert_node(list, n);

			/* limpando a string */
			string[0] = '\0';
		}
	}

	/* TODO inserir ultima string */
}

void to_buffer(int code, int size) {
	static int bit_count = 0;
	static unsigned long bit_buffer = 0L;

	bit_count += size;
	bit_buffer |= (unsigned long)code << (32 - bit_count - size);

	while (bit_count >= 8) {
		fputc(bit_buffer >> 24, codedfile);

		bit_count -= 8;
		bit_buffer <<= 8;
	}

	/* TODO esvaziar buffer na última passada */
}

void encode_file() {
	node *lista;
	create_dictionary(&lista);

	node *p;
	while (lista != NULL) {
		p = lista;

		int tam_indice = ceil(log2(p->index - 1));
		if (tam_indice > 0) {
			to_buffer(p->prefix, tam_indice);
		}
		to_buffer(p->new_simbol, sizeof(char));

		lista = p->next;
		free(p);
	}
}
