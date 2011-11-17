/*
 * lzdecode.c
 *
 *  Created on: Nov 17, 2011
 *      Author: ronie
 */

#include "lzdecode.h"

#define DEBUG

struct nd {
	int index;
	char string[TAM_MAX_STRING];
	int prefix;
	char new_simbol;
	struct nd *next;
} typedef node;

void decode_file() {

}
