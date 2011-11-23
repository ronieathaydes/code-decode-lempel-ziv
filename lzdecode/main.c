/*
 * main.c
 *
 *  Created on: Nov 17, 2011
 *      Author: ronie
 */

#include "lzdecode.h"

int main(int argc, char *argv[]) {

	/*if(argc != 3) {
		printf("Parâmetros inválidos.\n");
		printf("Uso: lzencode [infile.txt] [codedfile.lz]\n");
		exit (EXIT_FAILURE);
	}*/

	argv[1] = "/home/padawan/ronie/codedfile.lz";
	argv[2] = "/home/padawan/ronie/outfile.txt";

	if ((codedfile = fopen(argv[1], "rb")) == NULL) {
		printf("O arquivo de entrada nao pode ser aberto.\n");
		exit(EXIT_FAILURE);
	}

	if ((outfile = fopen(argv[2], "w")) == NULL) {
		printf("O arquivo de saída nao pode ser criado.\n");
		exit(EXIT_FAILURE);
	}

	decode_file();

	fclose(codedfile);
	fclose(outfile);

	exit(EXIT_SUCCESS);
}
