/*
 * lzencode.c
 *
 *  Created on: Oct 13, 2011
 *      Author: ronie
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

#define TAM_MAX_STRING 128

struct no {
	int indice;
	char string[TAM_MAX_STRING];
	int prefixo;
	char novo_simbolo;
	struct no *prox;
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

void imprimir_lista_strings(node *lista) {
	node *p = lista;

	while (p != NULL) {
		printf("|#%d\"%s\"(%d%c)| -> ", p->indice, p->string, p->prefixo, p->novo_simbolo);
		p = p->prox;
	}
}

int main(int argc, char *argv[]) {

	/*
	if(argc != 2) {
		printf("Parâmetros inválidos.\n");
		exit (EXIT_FAILURE);
	}
	*/

	argv[1] = "/home/rfmenezes/ronie/teste_arquivos/alice30.txt";

	FILE *infile;
	if ((infile = fopen(argv[1], "r")) == NULL) {
		printf("O arquivo de entrada não pode ser aberto.\n");
		exit(EXIT_FAILURE);
	}

	argv[2] = "/home/rfmenezes/ronie/codedfile.txt";

	FILE *codedfile;
	if ((codedfile = fopen(argv[2], "w")) == NULL) {
		printf("O arquivo de saída não pode ser criado.\n");
		exit(EXIT_FAILURE);
	}

	node *lista;
	iniciar_lista(&lista);

	/*
	node *n = calloc(1, sizeof(node));
	n->indice = 0;
	strcpy(n->string, "");
	n->prefixo = 0;
	n->novo_simbolo = '$';

	inserir_elemento(&lista, n);
	*/

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
			/*n->indice = contar_elementos(lista) + 1;*/
			n->indice = indice++;
			strcpy(n->string, string);
			n->prefixo = recuperar_codigo(lista, get_prefixo(string));
			n->novo_simbolo = simbolo;
			inserir_elemento(&lista, n);

			/* limpando a string */
			string[0] = '\0';
		}
	}

	//TODO inserir ultima string

	node *p;
	while (lista != NULL) {
		p = lista;
		fwrite(&(p->prefixo), sizeof(int), 1, codedfile);
		fwrite(&(p->novo_simbolo), sizeof(char), 1, codedfile);

		lista = p->prox;
		free(p);
	}
	free(lista);

	fclose(infile);
	fclose(codedfile);
	exit(EXIT_SUCCESS);
}
