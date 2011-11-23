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

#define CHAR_SIZE sizeof(char) * 8
#define LONG_INT_SIZE sizeof(long int) * 8

#define STRING_MAX_SIZE 128

/*
 * Variáveis globais
 */
FILE *codedfile;
FILE *outfile;

/*
 * Função externa à biblioteca
 */
void decode_file();