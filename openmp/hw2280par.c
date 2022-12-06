#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h> 
#include <sys/stat.h>

/*
	This is a parallel implementation of Lab2 histogram problem
*/
int main(int argc, char* argv[]){
	char filename[100] = ""; // the file name
	unsigned int b, t;


	if(argc != 4){
	  printf("usage:  ./histogram b t filename\n");
	  printf("b: is the number of bins, 0 < b <= 50\n");
	  printf("t: number of threads, 0 < t <= 100\n");
	  printf("filename: the name of the text file that contains the floating point numbers\n");
	  exit(1);
	}

	b = (unsigned int)atoi(argv[1]); 
	t = (unsigned int)atoi(argv[2]); 
	strcpy(filename, argv[3]);

	// bins
	unsigned int bin[b];
	memset(bin, 0, b*sizeof(unsigned int));
	
	// the range for each bin
	float range = 100.00/b;

	// retrieve the file size 
	// for later dividing work among threads
	struct stat st;
	stat(filename, &st);
	long size = st.st_size;


	// parallel part with each thread responsible for a subset of the numbers
	#pragma omp parallel num_threads(t) reduction(+:bin[:b])
	{
		FILE * fp;// for opening the input file

		if( !(fp = fopen(filename,"r")))
		{
		  printf("Cannot open file %s\n", filename);
		  exit(1);
		}

		
		int my_rank = omp_get_thread_num();

		
		long curr_pos = ftell(fp);// get the current position
		long workload = (size-curr_pos)/t;// calculate the workload
		
		// find the starting position in the file specific to the current thread
		long start_i = my_rank*workload + curr_pos;
		long end_i; // the including end position
		if(my_rank == t-1){
			end_i = size-1;
		}else{
			end_i = (my_rank+1)*workload+curr_pos;
		}
		// REMARK: each thread works on the range[start_i+1, end_i] including

		// jump to the starting position
		fseek( fp, start_i, SEEK_SET);
		
		/* if the start_i is a space,
			then the following floating num belongs to this thread
		   else
		    the current floating num belongs the previous thread
		    and we skip to the next white space
		*/
		char c = fgetc(fp);
		while (c != ' '){
			c = fgetc(fp);
		}


		unsigned int bin_i;
		float num_float;

		while(ftell(fp) <= end_i){
			// check whether the last char in the working range is space
			if(ftell(fp) == end_i){
				c = fgetc(fp);
				if(c == ' '){
					break;
				}else{
					fseek(fp, ftell(fp)-1, SEEK_SET);
				}
			}

			fscanf(fp, "%f,", &num_float);			

			bin_i = num_float/range;
			bin[bin_i] += 1;
		}

		fclose(fp);

	}

	unsigned int bin_i;
	// print the result
	for(bin_i = 0; bin_i < b; bin_i++){
		printf("bin[%d] = %d\n", bin_i, bin[bin_i]);
	}

	return 0;
}
