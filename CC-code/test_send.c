#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <string.h>
#include <time.h>
#include<fcntl.h>
#include<unistd.h>
#include "send.h"

int main(int argc, char *argv[]) {
	int rank,namelen,Ncluster;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	char file[100];
	char filename[100];
    	
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
//		strcpy(file,"rook.mkv");
		int cnt[Ncluster];
		for(i=0;i<Ncluster;i++)
			cnt[i]=0;
		send0(Ncluster, request, status, cnt, file);				
		printf("file distributed\n");
		strcpy(filename,"file/");
		strcat(filename,file);
		if(remove(filename)==0)
			printf("file removed %s\n",filename);
	}
	else {
		send1(rank, Ncluster, request, status, processor_name);
	}
	PMPI_Finalize();
	return 0;
}
