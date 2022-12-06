// must compile with: mpicc  -std=c99 -Wall -o checkdiv 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mpi.h>


int main(int argc, char *argv[]){
  
unsigned int x, A, B;
unsigned int i; //loop index
FILE * fp; //for creating the output file
char filename[100]=""; // the file name
unsigned int * numbers; //the numbers in the range [2, N]

double start_p1, end_p1, start_p2, end_p2;
double time_for_p1; 



/////////////////////////////////////////


start_p1 = clock();
// Check that the input from the user is correct.
if(argc != 4){

  printf("usage:  ./checkdiv A B x\n");
  printf("A: the lower bound of the range [A,B]\n");
  printf("B: the upper bound of the range [A,B]\n");
  printf("x: divisor\n");
  exit(1);
}  

A = (unsigned int)atoi(argv[1]); 
B = (unsigned int)atoi(argv[2]); 
x = (unsigned int)atoi(argv[3]);
 

// The arguments to the main() function are available to all processes and no need to send them from process 0.
// Other processes must, after receiving the variables, calculate their own range.


/////////////////////////////////////////
int my_rank, comm_sz;

MPI_Init (&argc, &argv);
MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
MPI_Comm_size (MPI_COMM_WORLD, &comm_sz);

/////////////////////////////////////////
//start of part 1
start_p1 = MPI_Wtime();
// The main computation part starts here
unsigned int workload = (B-A+1) / comm_sz;
i = workload*my_rank+A; // starting index in the current proc
unsigned int bound = 0; // excluding bound

// Determine the boundary
if (my_rank != (comm_sz-1)){
  bound = i + workload;
}else{
  // last proc
  bound = B + 1;
}

// make a large enough heap to store numbers
// considering x is at least 2, (workload+1) should be large enough for all procs
unsigned int n_size = workload+1;
numbers = (unsigned int *)malloc(n_size*sizeof(unsigned int));

// put all divisible numbers into "numbers"
for(unsigned int n_i = 0; n_i < n_size; n_i++){
  if(i % x != 0){
    i = ((i/x)+1)*x;
  }

  if(i < bound){
    numbers[n_i] = i;
    i += x;
  }else{
    numbers[n_i] = 1; // dummy value
  }
}

// end of the main compuation part
end_p1 = MPI_Wtime();
// Use reduction operation to get MAX of (end_p1 - start_p1) among processes 
// and send it to process 0 as time_for_p1
double local_time_for_p1 = end_p1 - start_p1;
MPI_Reduce(&local_time_for_p1, &time_for_p1, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);


if (my_rank != 0){
  // send it to proc 0
  MPI_Send(numbers, n_size, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD);

  free(numbers);
  MPI_Finalize ();

  // other processes have nothing left to do, then return directly
  return 0;
}
// else we are in proc 0 and maintain numbers

//end of part 1
/////////////////////////////////////////


/////////////////////////////////////////
//start of part 2
// Writing the results in the file


//forming the filename

start_p2 = MPI_Wtime();

strcpy(filename, argv[2]);
strcat(filename, ".txt");

if( !(fp = fopen(filename,"w+t")))
{
  printf("Cannot create file %s\n", filename);
  exit(1);
}

//Write the numbers divisible by x in the file as indicated in the lab description.

// receive numbers from other procs and write them to the file
for(i = 0; i < comm_sz; i++){
  if(i != 0)
    MPI_Recv(numbers, n_size, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  for(unsigned int n_i = 0; n_i < n_size; n_i++){
    unsigned int curr = numbers[n_i];
    if(curr != 1){
      fprintf(fp, "%u\n", curr);
    }else{
      break;
    }
  }
}


fclose(fp);

end_p2 = MPI_Wtime();

free(numbers);
MPI_Finalize ();


//end of part 2
/////////////////////////////////////////

/* Print  the times of the three parts */
printf("time of part1 = %lf s    part2 = %lf s\n", 
       (double)(time_for_p1),
       (double)(end_p2-start_p2) );
return 0;
}

