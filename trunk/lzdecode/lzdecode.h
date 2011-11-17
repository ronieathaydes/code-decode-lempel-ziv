/*
 * lzdecode.h
 *
 *  Created on: Nov 17, 2011
 *      Author: ronie
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define TAM_MAX_STRING 128

/*
 * Variáveis globais
 */
FILE *codedfile;
FILE *outfile;

/*
 * Função externa à biblioteca
 */
void decode_file();
