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

	char simbol;
	char string[TAM_MAX_STRING] = "";
	while (!feof(infile)) {
		/* lendo próximo símbolo */
		simbol = fgetc(infile);

		if (!feof(infile)) {
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
	}

	/* TODO inserir ultima string */
}

static void write_code(int code, int size) {
	static unsigned long int bit_buffer = 0L;
	static int bit_count = 0;

	bit_buffer |= (unsigned long int)code << (LONG_INT_SIZE - bit_count - size);
	bit_count += size;

	while (bit_count >= CHAR_SIZE) {
		fputc(bit_buffer >> (LONG_INT_SIZE - CHAR_SIZE), codedfile);

		bit_buffer <<= CHAR_SIZE;
		bit_count -= CHAR_SIZE;
	}

	/* TODO esvaziar buffer na última passada */
}

void encode_file() {
	node *list;
	initialize_list(&list);

	create_dictionary(&list);

	node *p;
	while (list != NULL) {
		p = list;

#ifdef DEBUG
	printf("Escrevento elemento %d \"%s\" (%d|\"%c\") ...\n", p->index, p->string, p->prefix, p->new_simbol);
#endif

		if (p->index == 2) {
			write_code(p->prefix, 1);
		} else if (p->index > 2) {
			int tam_indice = ceil(log2(p->index - 1));
			write_code(p->prefix, tam_indice);
		}
		write_code(p->new_simbol, CHAR_SIZE);

		list = p->next;
		free(p);
	}
}
