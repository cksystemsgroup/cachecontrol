/*
 * =====================================================================================
 *
 *       Filename:  cachetest.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/29/2012 04:42:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Martin Aigner (), martin@maigner.net
 *        Company:  
 *
 * =====================================================================================
 */

int main(int argc, char **argv) {


	int i, j;
	char memory[1024];

	for (i = 0; i < 1000; ++i) {
		for (j = 0; j < 1023; ++j) {
			memory[j] = memory[j+1]+1;
		}
	}
	return 0;
}



