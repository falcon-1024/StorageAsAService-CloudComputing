#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <string.h>
#include <time.h>
#include<fcntl.h>
#include<unistd.h>
#include "rec.h"

int main(int argc, char *argv[]) {
	int rank,namelen,Ncluster;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	char file[100];
    	
	PMPI_Init(&argc,&argv);
	PMPI_Comm_size(MPI_COMM_WORLD,&Ncluster);
	PMPI_Comm_rank(MPI_COMM_WORLD,&rank);
	PMPI_Get_processor_name(processor_name,&namelen);
	MPI_Request request;
	MPI_Status status;
	
	fprintf(stderr,"Process %d on %s\n",rank,processor_name);

	if(rank==0) {
		int i=0, d;
		int fptr;
		fptr = open("output.txt",O_RDONLY);
		d = read(fptr, &file[i], sizeof(char));
		while(d>0) {
			i++;
			d = read(fptr, &file[i], sizeof(char));
		}
		file[i] = '\0';
		close(fptr);
		int cnt[Ncluster];
		// reset
		for(i=0;i<Ncluster;i++)
			cnt[i]=0;
		
		rec0(Ncluster, request, status, cnt, file);
		printf("file created as file/%s\n",file);
	}
	else {
		rec1(rank, Ncluster, request, status, processor_name);
	}
	PMPI_Finalize();
	return 0;
}
