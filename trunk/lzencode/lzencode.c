/*
 * lzencode.c
 *
 *  Created on: Oct 13, 2011
 *      Author: ronie
 */

#include "lzencode.h"

#define DEBUG

struct nd {
	int index;
	char string[TAM_MAX_STRING];
	int prefix;
	char new_simbol;
	struct nd *next;
} typedef node;

static void initialize_list(node **list) {
	*list = NULL;
}

static void insert_node(node **list, node *n) {
#ifdef DEBUG
	printf("Lendo elemento %d \"%s\" (%d|\"%c\") ...\n", n->index, n->string, n->prefix, n->new_simbol);
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

static int is_string_found(node *list, char *string) {
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

static int find_index(node *list, char *string) {
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

static char* get_prefix(char string[]) {
	string[strlen(string)-1] = '\0';
	return string;
}

static void create_dictionary(node **list) {
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

static void to_buffer(int code, int size) {
	static unsigned long bit_buffer = 0L;
	static int bit_count = 0;

	bit_buffer |= (unsigned long)code << (32 - bit_count - size);
	bit_count += size;

	while (bit_count >= 8) {
		fputc(bit_buffer >> 24, codedfile);

		bit_buffer <<= 8;
		bit_count -= 8;
	}

	/* TODO esvaziar buffer na última passada */
}

void encode_file() {
	node *lista;
	create_dictionary(&lista);

	node *p;
	while (lista != NULL) {
		p = lista;
#ifdef DEBUG
	printf("Escrevento elemento %d \"%s\" (%d|\"%c\") ...\n", p->index, p->string, p->prefix, p->new_simbol);
#endif

		if (p->index == 2) {
			to_buffer(p->prefix, 1);
		} else if (p->index > 2) {
			int tam_indice = ceil(log2(p->index - 1));
			to_buffer(p->prefix, tam_indice);
		}
		to_buffer(p->new_simbol, sizeof(char) * 8);

		lista = p->next;
		free(p);
	}
}
