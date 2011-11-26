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
	char string[STRING_MAX_SIZE];
	int prefix;
	char new_simbol;
	struct nd *next;
} typedef node;

static node *list;
static node *end_list;

static unsigned long int bit_buffer = 0L;
static int bit_count = 0;

static char *file = NULL;
static char *begin_file = NULL;
static long file_size = 0;

static void load_file() {
	fseek(infile, 0, SEEK_END);
	file_size = ftell(infile);
	rewind(infile);

	file = calloc(file_size, sizeof(char));
	fread(file, sizeof(char), file_size, infile);
	begin_file = file;
}

static void initialize_list() {
	list = end_list = NULL;
}

static void insert_node(node *n) {
#ifdef DEBUG
	printf("Escrevendo elemento %d \"%s\" (%d|\"%c\") ...\n", n->index, n->string, n->prefix, n->new_simbol);
#endif
	if (list == NULL && end_list == NULL) {
    	list = end_list = n;
    } else if (list == end_list) {
    	list->next = end_list = n;
    } else {
    	end_list->next = n;
    	end_list = end_list->next;
    }
}

static int is_string_found(char *string) {
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

static int find_index(char *string) {
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

static void create_dictionary() {
	static int index = 1;

	char simbol;
	char string[STRING_MAX_SIZE] = "";

	int i;
	for (i = 0; i < file_size; i++) {
		/* lendo próximo símbolo */
		simbol = file[i];

		/* concatenado símbolo à string */
		strncat(string, &simbol, 1);

		if (!is_string_found(string)) {
			node *n = calloc(1, sizeof(node));
			n->index = index++;
			strcpy(n->string, string);
			n->prefix = find_index(get_prefix(string));
			n->new_simbol = simbol;
			insert_node(n);

			/* limpando a string */
			string[0] = '\0';
		}
	}

	free(file);
}

static void write_code(int code, int size) {
	bit_buffer |= (unsigned long int)code << (LONG_INT_SIZE - bit_count - size);
	bit_count += size;

	while (bit_count >= CHAR_SIZE) {
		fputc(bit_buffer >> (LONG_INT_SIZE - CHAR_SIZE), codedfile);

		bit_buffer <<= CHAR_SIZE;
		bit_count -= CHAR_SIZE;
	}
}

void encode_file() {
	load_file();
	initialize_list();
	create_dictionary();

	/* escrevendo o tamanho do dicionario */
	int dictionary_size = end_list->index;
	fwrite(&dictionary_size, sizeof(int), 1, codedfile);

	node *p = NULL;

	/* o primeiro elemento nao precisa de prefixo */
	p = list;
	write_code(p->new_simbol, CHAR_SIZE);
	list = p->next;
	free(p);

	/* o prefixo do segundo elemento precisa de apenas 1 bit */
	p = list;
	write_code(p->prefix, 1);
	write_code(p->new_simbol, CHAR_SIZE);
	list = p->next;
	free(p);

	while (list != NULL) {
		p = list;
		write_code(p->prefix, ceil(log2(p->index - 1)));
		write_code(p->new_simbol, CHAR_SIZE);
		list = p->next;
		free(p);
	}

	/* escrevendo o que sobrou no buffer */
	if (bit_count > 0) {
		fputc(bit_buffer >> (LONG_INT_SIZE - CHAR_SIZE), codedfile);
	}
}
