/*
 * main.c
 *
 *  Created on: Nov 17, 2011
 *      Author: ronie
 */

#include "lzdecode.h"

int main(int argc, char *argv[]) {

	if(argc != 3) {
		printf("Parâmetros inválidos.\n");
		printf("Uso: lzencode [codedfile.lz] [outfile.txt]\n");
		exit (EXIT_FAILURE);
	}

	if ((codedfile = fopen(argv[1], "rb")) == NULL) {
		printf("O arquivo de entrada não existe ou não pode ser aberto.\n");
		exit(EXIT_FAILURE);
	}

	if ((outfile = fopen(argv[2], "w")) == NULL) {
		printf("O arquivo de saída não pode ser criado.\n");
		exit(EXIT_FAILURE);
	}

	decode_file();
	printf("A descompactação foi concluída com sucesso.\n");

	fclose(codedfile);
	fclose(outfile);

	exit(EXIT_SUCCESS);
}
