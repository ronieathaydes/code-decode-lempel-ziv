/*
 * lzencode.c
 *
 *  Created on: Oct 13, 2011
 *      Author: ronie
 */

#include "lzencode.h"

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

/*
 * Define o tamanho dos blocos em que o arquivo vai ser dividido.
 * Foi escolhido o tamanho de 100Kb pelo bom desempenho demontrado nos testes.
 */
#define BLOCK_SIZE 102400

struct nd {
	int index;
	char string[STRING_MAX_SIZE];
	int prefix;
	char new_simbol;
	struct nd *right;
} typedef node;

static node *dictionary;
static node *end_dictionary;

static unsigned long int bit_buffer = 0L;
static int bit_count = 0;

static long file_size = 0;
static char *file_block = NULL;
static long file_block_size = 0;

/*
 * Carrega um bloco do arquivo na memória para a criação do dicionário.
 */
static void load_file() {
	if (file_size < BLOCK_SIZE) {
		file_block = calloc(file_size, sizeof(char));
		fread(file_block, sizeof(char), file_size, infile);
		file_block_size = file_size;
		file_size -= file_size;
	} else {
		file_block = calloc(BLOCK_SIZE, sizeof(char));
		fread(file_block, sizeof(char), BLOCK_SIZE, infile);
		file_block_size = BLOCK_SIZE;
		file_size -= BLOCK_SIZE;
	}
}

/*
 * Inicializa o dicionário.
 */
static void initialize_dictionary() {
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
	} else {
		end_dictionary->right = n;
		end_dictionary = end_dictionary->right;
	}
}

/*
 * Verifica se a string já foi vista anteriormente.
 */
static int is_string_found(char *string, int *last_found_string_prefix) {
	node *p = dictionary;

	if (p == NULL) {
		return 0;
	} else {
		while (p != NULL) {
			if (strcmp(p->string, string) == 0) {
				*last_found_string_prefix = p->index;
				return 1;
			}

			p = p->right;
		}

		return 0;
	}
}

/*
 * Cria o dicionário para um bloco do arquivo.
 */
static void create_dictionary() {
	int index = 1;

	char simbol;
	char string[STRING_MAX_SIZE] = "";

	int last_found_string_prefix = 0;

	int i;
	for (i = 0; i < file_block_size; i++) {
		/* lendo próximo símbolo */
		simbol = file_block[i];

		/* concatenado símbolo à string */
		strncat(string, &simbol, 1);

		if (!is_string_found(string, &last_found_string_prefix)) {
			node *n = calloc(1, sizeof(node));
			n->index = index++;
			strcpy(n->string, string);
			n->prefix = last_found_string_prefix;
			n->new_simbol = simbol;
			insert_node(n);

			/* limpando a string */
			string[0] = '\0';
			/* limpando último prefixo encontrado */
			last_found_string_prefix = 0;
		}
	}

	free(file_block);
}

/*
 * Escreve uma sequência de bits no buffer.
 */
static void write_code(int code, int size) {
	bit_buffer |= (unsigned long int)code << (LONG_INT_SIZE - bit_count - size);
	bit_count += size;

	while (bit_count >= CHAR_SIZE) {
		fputc(bit_buffer >> (LONG_INT_SIZE - CHAR_SIZE), codedfile);

		bit_buffer <<= CHAR_SIZE;
		bit_count -= CHAR_SIZE;
	}
}

/*
 * Concentra toda a lógica de codificação.
 */
void encode_file() {
	fseek(infile, 0, SEEK_END);
	file_size = ftell(infile);
	rewind(infile);

	int number_of_blocks = ceil((float)file_size / BLOCK_SIZE);
	fwrite(&number_of_blocks, sizeof(int), 1, codedfile);

	while (file_size > 0) {
		load_file();
		initialize_dictionary();
		create_dictionary();

		/* escrevendo o tamanho do dicionario */
		int dictionary_size = end_dictionary->index;
		write_code(dictionary_size, INT_SIZE);

		node *p = NULL;

		/* escreve o primeiro elemento do dicionário */
		/* o primeiro elemento não precisa de prefixo */
		p = dictionary;
		write_code(p->new_simbol, CHAR_SIZE);
		dictionary = p->right;
		free(p);

		/* escreve o segundo elemento do dicionário */
		/* o prefixo do segundo elemento precisa de apenas 1 bit */
		p = dictionary;
		write_code(p->prefix, 1);
		write_code(p->new_simbol, CHAR_SIZE);
		dictionary = p->right;
		free(p);

		/* escreve o restante do dicionário */
		while (dictionary != NULL) {
			p = dictionary;
			write_code(p->prefix, ceil((float)(log(p->index - 1) / log(2))));
			write_code(p->new_simbol, CHAR_SIZE);
			dictionary = p->right;
			free(p);
		}
	}

	/* escrevendo o que sobrou no buffer */
	if (bit_count > 0) {
		fputc(bit_buffer >> (LONG_INT_SIZE - CHAR_SIZE), codedfile);
	}
}
