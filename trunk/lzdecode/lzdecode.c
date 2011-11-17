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

static unsigned long read_code(int size) {
	static unsigned long bit_buffer = 0L;
	static int bit_count = 0;

	unsigned long return_value = 0;

	while (bit_count <= 24)	{
		bit_buffer |= (unsigned long)fgetc(codedfile) << (24 - bit_count);
		bit_count += 8;
	}

	return_value = bit_buffer >> (32 - size);
	bit_buffer <<= size;
	bit_count -= size;

	return return_value;
}

static void create_dictionary(node **list) {
	static int index = 1;

	while (!feof(codedfile)) {
		node *n = calloc(1, sizeof(node));
		n->index = index;

		n->prefix = 0;
		if (index == 2) {
			n->prefix = (int)read_code(1);
		} else if (index > 2) {
			int tam_indice = ceil(log2(index - 1));
			n->prefix = (int)read_code(tam_indice);
		}
		n->new_simbol = (char)read_code(sizeof(char) * 8);

		/* TODO inserir string inteira */

		insert_node(list, n);
		index++;
	}
}

void decode_file() {
	node *list;
	initialize_list(&list);

	create_dictionary(&list);
}
