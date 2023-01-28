int send0(int Ncluster, MPI_Request request, MPI_Status status, int* cnt,char* file) {
	FILE *fp;
	int fp1;
	char filename[100];
	double startwtime = 0.0, endwtime;
	srand(time(0));
	char str[1024];
	startwtime = PMPI_Wtime();
	int i,tmp1,tmp2;
	long int val=0;

	strcpy(filename,"file/");
	strcat(filename,file);
	fp = fopen(filename,"rb");
	fseek(fp, 0, SEEK_END);
	long int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	long int blockNo=0;
	long int size1;
	char c[6];
	char ch[100];
	val=1;
	char temp[Ncluster];
	temp[0] = 'a';
	for(i=1;i<Ncluster;i++) {
		MPI_Irecv(&temp[i],1,MPI_BYTE,i,cnt[i],MPI_COMM_WORLD,&request);
		MPI_Wait(&request,&status);
		cnt[i]++;
	}
	strcpy(filename,"data/");
	strcat(filename,file);
	strcat(filename,"-index");

	fp1 = creat(filename,0667);

	strcpy(filename,"data/");
	strcat(filename,file);
	while(size>0) {
		if(size>2097152)
			size1=2097152;
		else
			size1=size;
		strcpy(str,filename);
		sprintf(c, "%05ld", blockNo);
		c[5] = '\0';
		strcpy(ch, str);
		strcat(ch,"-");
		tmp1 = rand()%Ncluster;
		tmp2 = rand()%Ncluster;
		if(tmp1==tmp2)
			if(tmp1<Ncluster-1)
				tmp2 = Ncluster-1;
			else
				tmp2 = 1;
		write(fp1, c, sizeof(char)*5);
		write(fp1, &temp[tmp1], sizeof(char));
		write(fp1, &temp[tmp2], sizeof(char));
		strcat(ch,c);
		if(tmp1!=0 && tmp2!=0) {
			MPI_Isend(&val,1,MPI_LONG,tmp1,cnt[tmp1],MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt[tmp1]++;
			MPI_Isend(&val,1,MPI_LONG,tmp2,cnt[tmp2],MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt[tmp2]++;

			MPI_Isend(ch,1024,MPI_BYTE,tmp1,cnt[tmp1],MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt[tmp1]++;
			MPI_Isend(ch,1024,MPI_BYTE,tmp2,cnt[tmp2],MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt[tmp2]++;

			MPI_Isend(&size1,1,MPI_LONG,tmp1,cnt[tmp1],MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt[tmp1]++;
			MPI_Isend(&size1,1,MPI_LONG,tmp2,cnt[tmp2],MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt[tmp2]++;

			while(size1>1024) {
				fread(str,sizeof(str),1,fp);
				size1-=1024;
				MPI_Isend(str,1024,MPI_BYTE,tmp1,cnt[tmp1],MPI_COMM_WORLD,&request);
				MPI_Wait(&request,&status);
				cnt[tmp1]++;
				MPI_Isend(str,1024,MPI_BYTE,tmp2,cnt[tmp2],MPI_COMM_WORLD,&request);
				MPI_Wait(&request,&status);
				cnt[tmp2]++;
			}

			fread(str,size1,1,fp);
			MPI_Isend(str,size1,MPI_BYTE,tmp1,cnt[tmp1],MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt[tmp1]++;
			MPI_Isend(str,size1,MPI_BYTE,tmp2,cnt[tmp2],MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt[tmp2]++;
		}
		else {
		FILE *fp2;
			if(tmp2==0);
			else
				tmp1=tmp2;
			
			MPI_Isend(&val,1,MPI_LONG,tmp1,cnt[tmp1],MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt[tmp1]++;

			MPI_Isend(ch,1024,MPI_BYTE,tmp1,cnt[tmp1],MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt[tmp1]++;
			fp2 = fopen(ch,"wb");

			MPI_Isend(&size1,1,MPI_LONG,tmp1,cnt[tmp1],MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt[tmp1]++;

			while(size1>1024) {
				fread(str,sizeof(str),1,fp);
				fwrite(str,sizeof(str),1,fp2);
				size1-=1024;
				MPI_Isend(str,1024,MPI_BYTE,tmp1,cnt[tmp1],MPI_COMM_WORLD,&request);
				MPI_Wait(&request,&status);
				cnt[tmp1]++;
			}

			fread(str,size1,1,fp);
			fwrite(str,size1,1,fp2);
			fclose(fp2);
			MPI_Isend(str,size1,MPI_BYTE,tmp1,cnt[tmp1],MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt[tmp1]++;
			
		}
		size-=2097152;
		blockNo++;
	}
	fclose(fp);
	close(fp1);
	val=0;
	for(i=1;i<Ncluster;i++) {
		MPI_Isend(&val,1,MPI_LONG,i,cnt[i],MPI_COMM_WORLD,&request);
		MPI_Wait(&request,&status);
		cnt[i]++;
	}
	endwtime = PMPI_Wtime();
	printf("wall clock time = %f\n",endwtime-startwtime);
	return 0;
}

int send1(int rank, int Ncluster, MPI_Request request, MPI_Status status,char * processor_name) {
	FILE *fp;
	int val,cnt=0;
	long int size;
	long int i=1;
	char str[1024];
	char temp;
	if(strcmp(processor_name,"aryastark")==0)
		temp = 'k';
	else if(strcmp(processor_name,"saloni")==0)
		temp = 's';
	else if(strcmp(processor_name,"dhruvi")==0)
		temp = 'd';

	MPI_Isend(&temp,1,MPI_BYTE,0,cnt,MPI_COMM_WORLD,&request);
	MPI_Wait(&request,&status);
	cnt++;
	
	MPI_Irecv(&i,1,MPI_LONG,0,cnt,MPI_COMM_WORLD,&request);
	MPI_Wait(&request,&status);
	cnt++;
	while(i!=0) {
		MPI_Irecv(str,1024,MPI_BYTE,0,cnt,MPI_COMM_WORLD,&request);
		MPI_Wait(&request,&status);
		cnt++;
		fp = fopen(str,"wb");
		MPI_Irecv(&size,1,MPI_LONG,0,cnt,MPI_COMM_WORLD,&request);
		MPI_Wait(&request,&status);
		cnt++;
		while(size>1024) {
			MPI_Irecv(str,1024,MPI_BYTE,0,cnt,MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt++;
			fwrite(str,sizeof(str),1,fp);
			size-=1024;
		}
		MPI_Irecv(str,size,MPI_BYTE,0,cnt,MPI_COMM_WORLD,&request);
		MPI_Wait(&request,&status);
		cnt++;
		fwrite(str,size,1,fp);
		fclose(fp);
		MPI_Irecv(&i,1,MPI_LONG,0,cnt,MPI_COMM_WORLD,&request);
		MPI_Wait(&request,&status);
		cnt++;		
	}
	return 0;
}
