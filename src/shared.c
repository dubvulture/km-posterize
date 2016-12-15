#include <stdlib.h>

#include "shared.h"



/**
 *	Return the number of different items in the same position of given arrays
 */
int difference(int *arr1, int *arr2, int k)
{
	int count = 0;
	for (int i=0 ; i<k ; ++i) {
		if (arr1[i] != arr2[i])
			count++;
	}

	return count;
}


/**
 * Shame on me, I copied this from the internet
 * https://stackoverflow.com/questions/5064379/generating-unique-random-numbers-in-c
 * n >= 0		==>		initialize
 * n == -22		==>		free pool
 * n < 0		==>		get random
 */
int stolen_random(int size)
{
	int i, n;
	static int numNums = 0;
	static int *numArr = NULL;

	/* Initialize with a specific size */
	if (size >= 0) {
		if (numArr != NULL)
			free(numArr);
		if ((numArr = malloc(sizeof(int) * size)) == NULL)
			return -2; /* out of memory */
		for (i = 0; i  < size; i++)
			numArr[i] = i;
		numNums = size;
	}

	if (size == -22) {
		free(numArr);
		return -2;
	}

	/* Error if no numbers left in pool */
	if (numNums == 0)
		return -1;

	/* Get random number from pool and remove it (rnd in this
	 * case returns a number between 0 and numNums-1 inclusive)
	 */
	n = rand() % numNums;
	i = numArr[n];
	numArr[n] = numArr[numNums-1];
	numNums--;
	if (numNums == 0) {
		free (numArr);
		numArr = 0;
	}

	return i;
}
