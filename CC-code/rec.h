int rec0(int Ncluster, MPI_Request request, MPI_Status status, int* cnt, char* file) {
	int rank=0;
	FILE *fp1;
	int fptr;
	long int size;

	char filename[100];
	double startwtime = 0.0, endwtime;
	char str[1024];
	startwtime = PMPI_Wtime();
		
	int tmp, i, d, sta=0;
	char b, c;
	long int val=1;
	
	char name[Ncluster-1][20];
	char temp[Ncluster];
	temp[0] = 'a';
	for(i=1;i<Ncluster;i++) {
		MPI_Irecv(&temp[i],1,MPI_BYTE,i,cnt[i],MPI_COMM_WORLD,&request);
		MPI_Wait(&request,&status);
		cnt[i]++;
//		printf("%c\n",temp[i]);
	}
	strcpy(str,"");

	strcpy(filename,"data/");
	strcat(filename,file);
	strcat(filename,"-index");
	char a[6];

//	printf("fptr = %d\n",fptr);

	fptr = open(filename, O_RDONLY);
	
	strcpy(filename,"file/");
	strcat(filename,file);
	fp1 = fopen(filename,"wb");

	d = read(fptr, a, sizeof(char)*5);
	a[5] = '\0';
	d = read(fptr, &b, sizeof(char));
	d = read(fptr, &c, sizeof(char));

//	printf("fptr = %d d = %d a = %s b= %c c = %c\n",fptr, d,a,b,c);

	do {
		
//		printf("\n\n\n%s %c %c\n",a, b, c);

		if(b=='a')
			tmp=0;
		else if(b=='k') {
			for(i=1;i<Ncluster;i++) {
				if(temp[i]=='k') {
					tmp=i;
					break;
				}
			}
			if(i==Ncluster) {
				for(i=0;i<Ncluster;i++) {
					if(temp[i]==c) {
						tmp=i;
						break;
					}
				}
			}
		}
		else if(b=='s') {
			for(i=1;i<Ncluster;i++) {
				if(temp[i]=='s') {
					tmp=i;
					break;
				}
			}
			if(i==Ncluster) {
				for(i=0;i<Ncluster;i++) {
					if(temp[i]==c) {
						tmp=i;
						break;
					}
				}
			}
		}
		else if(b=='d') {
			for(i=1;i<Ncluster;i++) {
				if(temp[i]=='d') {
					tmp=i;
					break;
				}
			}
			if(i==Ncluster) {
				for(i=0;i<Ncluster;i++) {
					if(temp[i]==c) {
						tmp=i;
						break;
					}
				}
			}
		}
			
		if(tmp!=0) {
			val=1;
//			printf("tmp %d\n",tmp);
			strcpy(filename,"data/");
			strcat(filename,file);
			
			strcpy(str,filename);
			char* ch = strcat(str,"-");
			ch = strcat(ch,a);
//			printf("%s\n",ch);

			MPI_Isend(&val,1,MPI_LONG,tmp,cnt[tmp],MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt[tmp]++;
			
			MPI_Isend(ch,1024,MPI_BYTE,tmp,cnt[tmp],MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt[tmp]++;
			
			MPI_Irecv(&size,1,MPI_LONG,tmp,cnt[tmp],MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt[tmp]++;
			while(size>1024) {
				MPI_Irecv(str,1024,MPI_BYTE,tmp,cnt[tmp],MPI_COMM_WORLD,&request);
				MPI_Wait(&request,&status);
				cnt[tmp]++;
				fwrite(str,sizeof(str),1,fp1);
				size-=1024;
			}
			MPI_Irecv(str,size,MPI_BYTE,tmp,cnt[tmp],MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt[tmp]++;
			fwrite(str,size,1,fp1);
							
		}
		else {
			FILE* fp2;
			strcpy(filename,"data/");
			strcat(filename,file);
			
			strcpy(str,filename);
			char* ch = strcat(str,"-");
			ch = strcat(ch,a);
//			printf("%s\n",ch);
			fp2 = fopen(ch,"rb");
			fseek(fp2, 0, SEEK_END);
			size = ftell(fp2);
			fseek(fp2, 0, SEEK_SET);
			while(size>1024) {
				fread(str,sizeof(str),1,fp2);
				fwrite(str,sizeof(str),1,fp1);
				size-=1024;
			}
			fread(str,size,1,fp2);
			fwrite(str,size,1,fp1);
			fclose(fp2);
		}
		d = read(fptr, a, sizeof(char)*5);
		d = read(fptr, &b, sizeof(char));
		d = read(fptr, &c, sizeof(char));
	}while(d>0);
	
	val=0;
	for(i=1;i<Ncluster;i++) {
		MPI_Isend(&val,1,MPI_LONG,i,cnt[i],MPI_COMM_WORLD,&request);
		MPI_Wait(&request,&status);
		cnt[i]++;
	}		
	fclose(fp1);
	close(fptr);
	endwtime = PMPI_Wtime();
	printf("wall clock time = %f\n", endwtime-startwtime);
	return 0;
}


int rec1(int rank, int Ncluster, MPI_Request request, MPI_Status status, char* processor_name) {
	FILE *fp;
	char str[1024];
	int cnt=0;
	long int i;
	long int size;
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
		
		fp = fopen(str,"rb");
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		
		MPI_Isend(&size,1,MPI_LONG,0,cnt,MPI_COMM_WORLD,&request);
		MPI_Wait(&request,&status);
		cnt++;
		while(size>1024) {
			fread(str,sizeof(str),1,fp);
			size-=1024;
			MPI_Isend(str,1024,MPI_BYTE,0,cnt,MPI_COMM_WORLD,&request);
			MPI_Wait(&request,&status);
			cnt++;
		}
		fread(str,size,1,fp);
		MPI_Isend(str,size,MPI_BYTE,0,cnt,MPI_COMM_WORLD,&request);
		MPI_Wait(&request,&status);
		cnt++;
		fclose(fp);
		MPI_Irecv(&i,1,MPI_LONG,0,cnt,MPI_COMM_WORLD,&request);
		MPI_Wait(&request,&status);
		cnt++;
	}
	return 0;
}

