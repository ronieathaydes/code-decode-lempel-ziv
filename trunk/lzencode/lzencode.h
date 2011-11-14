/*
 * lzencode.h
 *
 *  Created on: Nov 13, 2011
 *      Author: ronie
 */

#define TAM_MAX_STRING 128

struct nd {
	int indice;
	char string[TAM_MAX_STRING];
	int prefixo;
	char novo_simbolo;
	struct nd *prox;
} typedef node;
