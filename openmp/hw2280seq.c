#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
	This is a sequential implementation of Lab2 histogram problem
	The input, number of threads, has no impact on this program
*/
int main(int argc, char* argv[]){
	char filename[100] = ""; // the file name
	unsigned int b;
	unsigned int i = 0;
	FILE * fp; // for opening the input file
	float num_float;


	if(argc != 4){
	  printf("usage:  ./histogram b t filename\n");
	  printf("b: is the number of bins, 0 < b <= 50\n");
	  printf("t: number of threads, 0 < t <= 100\n");
	  printf("filename: the name of the text file that contains the floating point numbers\n");
	  exit(1);
	}

	b = (unsigned int)atoi(argv[1]); 
	// t = (unsigned int)atoi(argv[2]); 
	strcpy(filename, argv[3]);

	unsigned int bin[b];
	memset(bin, 0, b*sizeof(unsigned int));
	
	float range = 100.00/b;


	if( !(fp = fopen(filename,"r")))
	{
	  printf("Cannot open file %s\n", filename);
	  exit(1);
	}

	// get the total number of floats
	unsigned int count = 0;
	fscanf(fp, "%d,", &count);

	// process each float 
	unsigned int bin_i;
	for(; i < count; i++){
		fscanf(fp, "%f,", &num_float);
		
		bin_i = num_float/range;
		bin[bin_i] += 1;
	}

	// print the result
	for(bin_i = 0; bin_i < b; bin_i++){
		printf("bin[%d] = %d\n", bin_i, bin[bin_i]);
	}

	fclose(fp);

	return 0;
}
