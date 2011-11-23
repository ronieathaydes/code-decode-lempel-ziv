/*
 * lzdecode.c
 *
 *  Created on: Nov 17, 2011
 *      Author: ronie
 */

#include "lzdecode.h"

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

static void initialize_list() {
	list = end_list = NULL;
}

static void insert_node(node *n) {
#ifdef DEBUG
	printf("Lendo elemento %d \"%s\" (%d|\"%c\") ...\n", n->index, n->string, n->prefix, n->new_simbol);
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

static void build_string(char string[], int prefix) {
	node *p = list;

	while (p->index < prefix) {
		p = p->next;
	}

	if (p->prefix > 0) {
		build_string(string, p->prefix);
	}

	string[strlen(string)+1] = '\0';
	string[strlen(string)] = p->new_simbol;
}

static unsigned long int read_code(int size) {
	unsigned long int return_value;

	while (bit_count <= LONG_INT_SIZE - CHAR_SIZE)	{
		unsigned long int byte_read =
				(unsigned long int)fgetc(codedfile) << (LONG_INT_SIZE - CHAR_SIZE - bit_count);

		if (!feof(codedfile)) {
			bit_buffer |= byte_read;
			bit_count += CHAR_SIZE;
		} else {
			break;
		}
	}

	return_value = bit_buffer >> (LONG_INT_SIZE - size);
	bit_buffer <<= size;
	bit_count -= size;

	return return_value;
}

static void create_dictionary() {
	static int index = 1;

	int dictionary_size = 0;
	fread(&dictionary_size, sizeof(int), 1, codedfile);

	while (index <= dictionary_size) {
		node *n = calloc(1, sizeof(node));
		n->index = index;

		//TODO eliminar if

		n->prefix = 0;
		if (index == 2) {
			n->prefix = (int)read_code(1);
		} else if (index > 2) {
			int tam_indice = ceil(log2(index - 1));
			n->prefix = (int)read_code(tam_indice);
		}

		n->new_simbol = (char)read_code(CHAR_SIZE);

		char string[STRING_MAX_SIZE] = "";
		if (n->prefix > 0) {
			build_string(string, n->prefix);
		}
		string[strlen(string)+1] = '\0';
		string[strlen(string)] = n->new_simbol;
		strcpy(n->string, string);

		insert_node(n);
		index++;
	}
}

void decode_file() {
	initialize_list();

	create_dictionary();

	node *p;
	while (list != NULL) {
		p = list;

		fputs(p->string, outfile);

		list = p->next;
		free(p);
	}
}
