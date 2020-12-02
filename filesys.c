#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
//#include <io.h>

//define allocate base unit SECTOR size, pay attention,actual is 252, because the last four bytes are used to index the next sector number.
#define SECTOR_SIZE 256

//for simple reasion, our file system only can store 256 files
#define MAX_FILES 256

#define DISK_SIZE 1024*1024*4   //4M disk space

char sectorFree[1024*1024*4/256]={0};
char virtualFileName[256]="virtualDisk";
void createVirtualDisk(void);
typedef struct _fileItem{
  char fileName[8];
  int len;
  int start;
}FILEITEM;
FILEITEM files[256];

void writeFileItem()
{
	FILE *fp;
	fp =fopen(virtualFileName,"w");
	if(fp==NULL)
	{
		printf("open vritual disk failure\n");
		return;
	}
	fwrite(&files[0],sizeof(FILEITEM),256,fp);
	fclose(fp);
}

void readFileItem(){
	FILE *fp;
	fp =fopen(virtualFileName,"r");
	if(fp==NULL)
	{
		printf("open vritual disk failure\n");
		return;
	}
	fread(&files[0],sizeof(FILEITEM),256,fp);
	fclose(fp);
}

void writeSectorIdleState(){
	FILE *fp;
	fp =fopen(virtualFileName,"w");
	if(fp==NULL)
	{
		printf("open vritual disk failure\n");
		return;
	}
	fseek(fp,256*8,SEEK_SET);
	fwrite(&sectorFree[0],16384,1,fp);
	fclose(fp);
}

void readSectorIdleState(){
	FILE *fp;
	fp =fopen(virtualFileName,"r");
	if(fp==NULL)
	{
		printf("open vritual disk failure\n");
		return;
	}
	fseek(fp,256*8,SEEK_SET);
	fread(&sectorFree[0],16384,1,fp);
	fclose(fp);
}

void openDisk(){
	if(access(virtualFileName,0)==-1)
	{
		createVirtualDisk();
		return ;
	}
	readSectorIdleState();
	readFileItem();	
}

void readSector(int secIndex,char *buf)
{
	FILE *fp;
	fp =fopen(virtualFileName,"r");
	if(fp==NULL)
	{
		printf("open vritual disk failure\n");
		return;
	}
	fseek(fp,secIndex*256,SEEK_SET);
	fread(buf,256,1,fp);
	fclose(fp);
}

void writeSector(int secIndex,char *buf){
	FILE *fp;
	fp =fopen(virtualFileName,"w");
	if(fp==NULL)
	{
		printf("open vritual disk failure\n");
		return;
	}
	fseek(fp,secIndex*256,SEEK_SET);
	fwrite(buf,256,1,fp);
	fclose(fp);
}

int findFreeSector(){
	int index=0;
	int i;
	for( i=72;i<16384;i++)
	{
		if(sectorFree[i]==0)
		{
			index =i;
			sectorFree[i]=1;
			break;
		}
	}
	if(index==0)
		printf("no more disk space \n");	
//	writeSectorIdleState();
	return index;
}
void resetFreeSector(int index){		
	sectorFree[index]=0;
//	writeSectorIdleState();
}

void listFiles(){
	int i;
	printf("Files are:\n");
	for(i=0;i<256;i++)
	{
		if(strcmp(files[i].fileName,"")!=0)
			printf("%s\tlengh is:%d\tstore at%d sector\n",files[i].fileName,files[i].len,files[i].start);
	}
}

int getFileSize(const char *path){
	int filesize = -1;	
	struct stat statbuff;
	if(stat(path, &statbuff) < 0){
		return filesize;
	}else{
		filesize = statbuff.st_size;
	}
	return filesize;
}

int findFreeFileItem(){
	int index=0;
	int i;
	for(i=0;i<256;i++){
		if(strcmp(files[i].fileName,"")==0)
		{
			printf("%s\n",files[i].fileName);
			index =i;
			break;
		}
	}
	if(i>255) 
		printf("cannot add more file to virtul disk\n");
	return index;
}

void addFileToVirtualDisk()
{
	FILE *fp;
	unsigned int len;
	int sectors=0;
	char filename[256];
	int i;
	int freeitem;
	int start=0;
	printf("input the exits filename\n");
	fflush(stdin);
	scanf("%s",filename);
	if(access(filename,0)!=0){
		printf("cannot find the file\n");
		return;
	}
	len=getFileSize(filename);
	if(len<=0)
		printf("The file is empty\n");
	sectors=len/252;
	if(len%252)
		sectors+=1;
	fp=fopen(filename,"r");
	if(fp==NULL){
		printf("open file error\n");
		return ;
	} 
	for(i=0;i<sectors;i++){
		int f1;
		char buf[256]={0};		
		fread(buf,252,1,fp);
		f1=findFreeSector();
		*((int*)&buf[252])=f1;
		writeSector(f1,buf);	
		if(i==0)
			start=f1;
	}
	fclose(fp);
	freeitem=findFreeFileItem();
	strcpy(files[freeitem].fileName,filename);
	files[freeitem].len=len;
	files[freeitem].start=start;
	writeFileItem();	
}

void createVirtualDisk(void)
{
//create a file to simulate the virtual disk, the disk size is 4mb
	char  *ch;
	int i;
	FILEITEM t;
	FILE * fp ;
	ch=(char*)malloc(1024*1024*4);
	if(ch==NULL)
	{
		printf("error");
		return;
	}
	strcpy(t.fileName,"");
	t.len=0;
	t.start=0;
	fp =fopen(virtualFileName,"w+");
	if(fp==NULL)
	{
		printf("create vritual disk failure\n");
		return;
	}
	fwrite(ch,1024*1024*4,1,fp);
//write 256 file item at the begin of the virtual disk
	fseek(fp,0,SEEK_SET);
	for(i=0;i<256;i++)
	{
		strcpy(files[i].fileName,"");
		files[i].len=0;
		files[i].start=0;
	}
	fwrite(&files[0],sizeof(FILEITEM),256,fp);
//write sectorFree information after the file item
//first 72 secotrs are used by fileitem and sector idle state
	for(i=0;i<72;i++)
	{
		sectorFree[i]=1;
	}
	fwrite(&sectorFree[0],16384,1,fp);	
	fclose(fp);
}
int showMenu()
{
	char cmd[256];
	printf("\n--------------Virtual Disk, a simple file system--------------\n");
	printf("Copy a file into the disk by type COPY\n");
 	printf("Terminate program by type EXIT\n");
 	printf("List files in the disk by type LIST\n");
	 printf("Delete files in the disk by type DELETE\n");//MODIFIED
	printf("VirtualDisk>");
	fflush(stdin);
	scanf("%s",cmd);
	if(strcmp(cmd,"COPY")==0)
		return 0;
	if(strcmp(cmd,"LIST")==0)
		return 1;
	if(strcmp(cmd,"EXIT")==0)
		return 2;
	if(strcmp(cmd,"DELETE")==0) //MODIFIED
		return 3;
	return -1;
}
//MODIFIED
void deleteFile(){
	FILE *fp;
	int status;
	char fname[8];
	printf("\n\n Remove a file from the disk :\n");
	printf("----------------------------------\n"); 	
	fp =fopen(virtualFileName,"w");
	printf(" Input the name of file to delete : ");
	scanf("%s",fname);
	for(int i=0;i<256;i++)
	{
		if(strcmp(files[i].fileName,fname)==0){
			printf("%s\tlengh is:%d\tstore at%d sector\n",files[i].fileName,files[i].len,files[i].start);
			strcpy(files[i].fileName,"");
			files[i].len=0;
			files[i].start=0;
			break;
		}
	}
	fclose(fp);
	//printf("%d",index);
	if(status==0)
	{
		printf(" The file %s is deleted successfully..!!\n\n",fname);
	}
	else
	{
		printf(" Unable to delete file %s\n\n",fname);
	}
}//MODIFIED
int main(){
	openDisk();
	do{
		int func;
		func=showMenu();
		switch(func)
{
case 0:
addFileToVirtualDisk();
break;
case 1:
listFiles();
break;
case 2:
break;
case 3:
deleteFile();
break;
default:
break;
}
	if(func==2) 
		break;
	  }while(1);
	addFileToVirtualDisk();
	return 0;
}