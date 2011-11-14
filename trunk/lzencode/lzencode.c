/*
 * lzencode.c
 *
 *  Created on: Oct 13, 2011
 *      Author: ronie
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEBUG

#define TAM_MAX_STRING 128

struct nd {
	int indice;
	char string[TAM_MAX_STRING];
	int prefixo;
	char novo_simbolo;
	struct nd *prox;
} typedef node;

void iniciar_lista(node **lista) {
	*lista = NULL;
}

void inserir_elemento(node **lista, node *n) {
#ifdef DEBUG
	printf("Inserindo elemento %d \"%s\" (%d|\"%c\") ...\n", n->indice, n->string, n->prefixo, n->novo_simbolo);
#endif
	node *p = *lista;

    if (p == NULL) {
    	*lista = n;
    } else {
    	while (p->prox != NULL) {
    		p = p->prox;
    	}

    	p->prox = n;
    }
}

int contar_elementos(node *lista) {
	node *p = lista;

    if (p == NULL) {
    	return 0;
    } else {
		int qtd_nos = 1;
    	while (p->prox != NULL) {
    		qtd_nos++;
    		p = p->prox;
    	}

    	return qtd_nos;
    }
}

int is_string_encontrada(node *lista, char *string) {
	node *p = lista;

	if (p == NULL) {
		return 0;
	} else {
		while (p != NULL) {
			if (strcmp(p->string, string) == 0) {
				return 1;
			}

			p = p->prox;
		}

		return 0;
	}
}

int recuperar_codigo(node *lista, char *string) {
	node *p = lista;

	if (p == NULL) {
		return 0;
	} else {
		while (p != NULL) {
			if (strcmp(p->string, string) == 0) {
				return p->indice;
			}

			p = p->prox;
		}

		return 0;
	}
}

char* get_prefixo(char string[]) {
	string[strlen(string)-1] = '\0';
	return string;
}

void decimal_to_binario(int num) {
	while (num % 2 > 1) {

	}
}

void escrever_binario(char bit) {
	static int count_buffer = 0;
	static unsigned long buffer = 0L;

	buffer = (buffer << 1) | ((bit << 7) >> 7);
	count_buffer++;

	if (count_buffer == 32) {
		/* TODO escrever buffer */

		count_buffer = 0;
		buffer = 0L;
	}
}

void imprimir_lista_strings(node *lista) {
	node *p = lista;

	while (p != NULL) {
		printf("|#%d\"%s\"(%d%c)| -> ", p->indice, p->string, p->prefixo, p->novo_simbolo);
		p = p->prox;
	}
}

int main(int argc, char *argv[]) {

	/*if(argc != 3) {
		printf("Parâmetros inválidos.\n");
		exit (EXIT_FAILURE);
	}*/

	argv[1] = "/home/rfmenezes/ronie/teste_arquivos/alice30.txt";
	argv[2] = "/home/rfmenezes/ronie/codedfile.txt";

	FILE *infile;
	if ((infile = fopen(argv[1], "r")) == NULL) {
		printf("O arquivo de entrada não pode ser aberto.\n");
		exit(EXIT_FAILURE);
	}

	FILE *codedfile;
	if ((codedfile = fopen(argv[2], "wb")) == NULL) {
		printf("O arquivo de saída não pode ser criado.\n");
		exit(EXIT_FAILURE);
	}

	node *lista;
	iniciar_lista(&lista);

	/* "static" fará sentido quando refatorar para um função separada */
	static int indice = 1;

	char simbolo;
	char string[TAM_MAX_STRING] = "";
	while (!feof(infile)) {
		/* lendo próximo símbolo */
		fread(&simbolo, sizeof(char), 1, infile);

		/* concatenado símbolo à string */
		string[strlen(string)+1] = '\0';
		string[strlen(string)] = simbolo;
		
		if (!is_string_encontrada(lista, string)) {
			node *n = calloc(1, sizeof(node));
			n->indice = indice++;
			strcpy(n->string, string);
			n->prefixo = recuperar_codigo(lista, get_prefixo(string));
			n->novo_simbolo = simbolo;
			inserir_elemento(&lista, n);

			/* limpando a string */
			string[0] = '\0';
		}
	}

	/* TODO inserir ultima string */

	node *p;
	while (lista != NULL) {
		p = lista;

		int tam_indice;
		tam_indice = ceil(log2(p->indice - 1));

		while (tam_indice > 0) {

		}

		lista = p->prox;
		free(p);
	}

	fclose(infile);
	fclose(codedfile);
	exit(EXIT_SUCCESS);
}
