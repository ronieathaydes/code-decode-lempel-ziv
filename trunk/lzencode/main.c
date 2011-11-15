/*
 * main.c
 *
 *  Created on: Oct 13, 2011
 *      Author: ronie
 */

#include "lzencode.h"

int main(int argc, char *argv[]) {

	/*if(argc != 3) {
		printf("Parâmetros inválidos.\n");
		printf("Uso: lzencode [infile.txt] [codedfile.lz]\n");
		exit (EXIT_FAILURE);
	}*/

	argv[1] = "/home/rfmenezes/ronie/teste_arquivos/alice30.txt";
	argv[2] = "/home/rfmenezes/ronie/codedfile.txt";

	if ((infile = fopen(argv[1], "r")) == NULL) {
		printf("O arquivo de entrada não pode ser aberto.\n");
		exit(EXIT_FAILURE);
	}

	if ((codedfile = fopen(argv[2], "wb")) == NULL) {
		printf("O arquivo de saída não pode ser criado.\n");
		exit(EXIT_FAILURE);
	}

	encode_file();

	fclose(infile);
	fclose(codedfile);
	exit(EXIT_SUCCESS);
}

