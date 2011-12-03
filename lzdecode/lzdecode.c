/*
 * lzdecode.c
 *
 *  Created on: Nov 17, 2011
 *      Author: ronie
 */

#include "lzdecode.h"

/*
 * Define o tamanho em bytes dos tipos de dados para ajudar na gerência do buffer.
 */
#define INT_SIZE sizeof(int) * 8
#define CHAR_SIZE sizeof(char) * 8
#define LONG_INT_SIZE sizeof(long int) * 8

/*
 * Define o tamanho máximo de cada nova string encontrada.
 */
#define STRING_MAX_SIZE 64

struct nd {
	int index;
	char string[STRING_MAX_SIZE];
	int prefix;
	char new_simbol;
	struct nd *left;
	struct nd *right;
} typedef node;

static node *dictionary;
static node *end_dictionary;

static unsigned long int bit_buffer = 0L;
static int bit_count = 0;

/*
 * Inicializa o dicionário.
 */
static void initialize_list() {
	dictionary = end_dictionary = NULL;
}

/*
 * Insere uma nova string no dicionário.
 */
static void insert_node(node *n) {
	if (dictionary == NULL && end_dictionary == NULL) {
		dictionary = end_dictionary = n;
	} else if (dictionary == end_dictionary) {
		dictionary->right = end_dictionary = n;
		end_dictionary->left = dictionary;
	} else {
		n->left = end_dictionary;
		end_dictionary->right = n;
		end_dictionary = end_dictionary->right;
	}
}

/*
 * Constrói a string a partir do novo caractere pesquisando no dicionário.
 */
static void build_string(char string[], int prefix) {
	node *p = dictionary;

	while (p->index < prefix) {
		p = p->right;
	}

	while (p->index > prefix) {
		p = p->left;
	}

	if (p->prefix > 0) {
		build_string(string, p->prefix);
	}

	strncat(string, &(p->new_simbol), 1);
}

/*
 * Lê uma sequência de bits do buffer.
 */
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

/*
 * Cria o dicionário para um bloco do arquivo.
 */
static void create_dictionary() {
	int index = 1;

	/* lendo o tamanho do dicionario */
	int dictionary_size = (int)read_code(INT_SIZE);

	node *n = NULL;
	char string[STRING_MAX_SIZE] = "";

	/* lendo o primeiro elemento do dicionário */
	/* o primeiro elemento nao precisa de prefixo */
	n = calloc(1, sizeof(node));
	n->index = index++;
	n->prefix = 0;
	n->new_simbol = (char)read_code(CHAR_SIZE);
	strncpy(n->string, &(n->new_simbol), 1);

	insert_node(n);
	string[0] = '\0';

	/* lendo o segundo elemento do dicionário */
	/* o prefixo do segundo elemento precisa de apenas 1 bit */
	n = calloc(1, sizeof(node));
	n->index = index++;
	n->prefix = (int)read_code(1);
	n->new_simbol = (char)read_code(CHAR_SIZE);
	if (n->prefix > 0) {
		build_string(string, n->prefix);
	}
	strcpy(n->string, strncat(string, &(n->new_simbol), 1));

	insert_node(n);
	string[0] = '\0';

	/* lendo o restante do dicionário */
	while (index <= dictionary_size) {
		n = calloc(1, sizeof(node));
		n->index = index;
		n->prefix = (int)read_code(ceil((float)(log(n->index - 1) / log(2))));
		n->new_simbol = (char)read_code(CHAR_SIZE);
		if (n->prefix > 0) {
			build_string(string, n->prefix);
		}
		strcpy(n->string, strncat(string, &(n->new_simbol), 1));

		insert_node(n);

		/* limpando a string */
		string[0] = '\0';

		index++;
	}
}

/*
 * Concentra toda a lógica de decodificação.
 */
void decode_file() {
	int number_of_blocks = 0;
	fread(&number_of_blocks, sizeof(int), 1, codedfile);

	int i;
	for (i = 0; i < number_of_blocks; i++) {
		initialize_list();
		create_dictionary();

		node *p;
		while (dictionary != NULL) {
			p = dictionary;

			fputs(p->string, outfile);

			dictionary = p->right;
			free(p);
		}
	}
}
