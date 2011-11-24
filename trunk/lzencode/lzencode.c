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

//static char last_string[STRING_MAX_SIZE] = "";

static char *file = NULL;
static char *begin_file = NULL;

static void read_file() {
	fseek(infile, 0, SEEK_END);
	long file_size = ftell(infile);
	rewind(infile);

	file = calloc(file_size, sizeof(char));
	fread(file, sizeof(char), file_size, infile);
	begin_file = file;
}

static void initialize_list() {
	list = end_list = NULL;
}

static void insert_node(node *n) {
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
	while (!feof(infile)) {
		/* lendo próximo símbolo */
		simbol = fgetc(infile);

		if (!feof(infile)) {
			/* concatenado símbolo à string */
			string[strlen(string)+1] = '\0';
			string[strlen(string)] = simbol;

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
	}

//	strcpy(last_string, string);
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
	read_file();
	initialize_list();
	create_dictionary();

	// Escrevendo o tamanho do dicionario
	int dictionary_size = end_list->index;
	fwrite(&dictionary_size, sizeof(int), 1, codedfile);

	node *p = NULL;
	// O primeiro elemento nao precisa de prefixo
	p = list;
	write_code(p->new_simbol, CHAR_SIZE);
#ifdef DEBUG
	printf("Escrevendo elemento %d \"%s\" (%d|\"%c\") ...\n", p->index, p->string, p->prefix, p->new_simbol);
#endif
	list = p->next;
	free(p);

	// O prefixo do segundo elemento precisa de apenas 1 bit
	p = list;
	write_code(p->prefix, 1);
	write_code(p->new_simbol, CHAR_SIZE);
#ifdef DEBUG
	printf("Escrevendo elemento %d \"%s\" (%d|\"%c\") ...\n", p->index, p->string, p->prefix, p->new_simbol);
#endif
	list = p->next;
	free(p);

	while (list != NULL) {
		p = list;
		write_code(p->prefix, ceil(log2(p->index - 1)));
		write_code(p->new_simbol, CHAR_SIZE);
#ifdef DEBUG
	printf("Escrevendo elemento %d \"%s\" (%d|\"%c\") ...\n", p->index, p->string, p->prefix, p->new_simbol);
#endif
		list = p->next;
		free(p);
	}

	if (bit_count > 0) {
#ifdef DEBUG
	printf("Esvaziando buffer...\n");
#endif
		fputc(bit_buffer >> (LONG_INT_SIZE - CHAR_SIZE), codedfile);
	}

	/* TODO inserir ultima string "last_string" */
}
