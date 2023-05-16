#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

void mkdict(char* filename, uint32_t nsymbols)
{
	FILE* file = fopen(filename, "w");
	if(file == NULL) {
		printf("Can't make file\n");
		exit(1);
	}

	for(int j = 0; j < nsymbols; j++) {
		fprintf(file, "%c", rand() % 26 + 97);
	}
		
	fclose(file);
}

int main(int argc, char* argv[])
{
	if(argc != 3) {
		printf("Wrong number of args\n");
		exit(1);
	}
	mkdict(argv[1], (uint32_t) atoi(argv[2]));
	printf("Succesfull!\n");
	return 0;
}
