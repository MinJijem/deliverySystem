#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

/* 
  definition of storage cell structure ----
  members :
  int building : building number of the destination
  int room : room number of the destination
  int cnt : number of packages in the cell
  char passwd[] : password setting (4 characters)
  char *contents : package context (message string)
*/
typedef struct {
	
	int building;
	int room;
	int cnt;
	char passwd[PASSWD_LEN+1];
	
	char *context;
} storage_t;


static storage_t** deliverySystem; 			//deliverySystem
static int storedCnt = 0;					//number of cells occupied
static int systemSize[2] = {0, 0};  		//row/column of the delivery system
static char masterPassword[PASSWD_LEN+1];	//master password




// ------- inner functions ---------------

//print the inside context of a specific cell
//int x, int y : cell to print the context
static void printStorageInside(int x, int y) {
	printf("\n------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n");
	if (deliverySystem[x][y].cnt > 0)
		printf("<<<<<<<<<<<<<<<<<<<<<<<< : %s >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", deliverySystem[x][y].context);
	else
		printf("<<<<<<<<<<<<<<<<<<<<<<<< empty >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
		
	printf("------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n\n");
}

//initialize the storage
//set all the member variable as an initial value
//and allocate memory to the context pointer
//int x, int y : cell coordinate to be initialized
static void initStorage(int x, int y) {
	

	printf("------------------------>extracting the storage(%d, %d)...\n",x,y);
	printStorageInside(x,y);
	deliverySystem[x][y].cnt=0;		//save the number of object is 0 in storage (x,y) 
	storedCnt--;					//the number of object in storage --1

	
}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching, -1 - password is not matching
static int inputPasswd(int x, int y)
{
	
		char getpass[PASSWD_LEN+1];				
		int result;
		int result2; 
	
		printf(" - input password for (%d, %d) storage : ",x,y);
		scanf("%s",getpass);									//get password
		
		result=strcmp(getpass,deliverySystem[x][y].passwd);		//compare getpass with password
		result2=strcmp(getpass,masterPassword);					//compare getpass with master password
		
		if(result==0 || result2==0) 							//getpassword = storage's password or getpass = master password
		{
			return 0;
		}
		else					//getpassword =/= storage's password or master password
		{
			printf("-------------------------> password is wrong !!\n");
			return -1;
		}

}





// ------- API function for main.c file ---------------

//backup the delivery system context to the file system
//char* filepath : filepath and name to write
//return : 0 - backup was successfully done, -1 - failed to backup
int str_backupSystem(char* filepath) {
	
	int i,j;
	
	FILE *fp;
	
	if((fp=fopen(filepath,"w"))==NULL)						//file open writing mode 
		return -1;					
	
	fprintf(fp,"%d %d\n",systemSize[0],systemSize[1]);	//save systemsize and masterpassword
	fprintf(fp,"%s\n",masterPassword);
	
	for(i=0;i<systemSize[0];i++)
	{
		for(j=0;j<systemSize[1];j++)
		{
			//if there is object in storage cell
			if(deliverySystem[i][j].cnt!=0)
			{				
			//save informations
			fprintf(fp,"%d %d %d %d %s %s\n",i,j,deliverySystem[i][j].building,deliverySystem[i][j].room,deliverySystem[i][j].passwd,deliverySystem[i][j].context);
			}
		}
	}
	
	//close file
	if(fclose(fp)==0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}



//create delivery system on the double pointer deliverySystem
//char* filepath : filepath and name to read config parameters (row, column, master password, past contexts of the delivery system
//return : 0 - successfully created, -1 - failed to create the system
int str_createSystem(char* filepath) {
	
	int i,j;
	int x,y;
	char c;
	
	
	FILE *fp;
	
	if((fp=fopen(filepath,"r"))==NULL)					//open file
		return -1;
	

	fscanf(fp,"%d %d",&systemSize[0],&systemSize[1]);	//get systemsize and master password
	fscanf(fp,"%s",masterPassword);
	
	
	deliverySystem=malloc(systemSize[0]*sizeof(struct storage_t*)); //memory allocation
	for(i=0;i<systemSize[0];i++)
		deliverySystem[i]=malloc(systemSize[1]*sizeof(storage_t));
		
	for(i=0;i<systemSize[0];i++)									//memory allocation deliverySystem.context
	{
		for(j=0;j<systemSize[1];j++)
			deliverySystem[i][j].context=(char *)malloc(100*sizeof(char));
	}

		
	if(deliverySystem==NULL)				//if fail to allocate
	{
		return-1;
	}

	

	for(i=0;i<systemSize[0];i++)			//before get deliverySystem information, initialization
	{
		for(j=0;j<systemSize[1];j++)
		{
			deliverySystem[i][j].cnt=0;	
		}
	}


	
	while(c=fgetc(fp)!=EOF)					//to the end of the file
	 {	
	 	
	 	fscanf(fp,"%d %d ",&x,&y);
	 	deliverySystem[x][y].cnt=1;
	 	fscanf(fp," %d",&deliverySystem[x][y].building);
	 	fscanf(fp," %d",&deliverySystem[x][y].room);
	 	fscanf(fp," %s",deliverySystem[x][y].passwd);
	 	fscanf(fp," %s",deliverySystem[x][y].context);
	 	
	 }
	 
	for(i=0;i<systemSize[0];i++)				//count the number of object in storage
	{
		for(j=0;j<systemSize[1];j++)
		{
			if(deliverySystem[i][j].cnt!=0)
			storedCnt++;
		}
	}
	 
	
	
	if(fclose(fp)==0)
	{
		return 0;
	}
	else
	{
		return -1;
	
	}


}

//free the memory of the deliverySystem 
void str_freeSystem(void) {
	
	int i;
	int j;
	
	for(j=0;j<systemSize[1];j++)
	{
		for(i=0;i<systemSize[0];i++)
			free(deliverySystem[i][j].context);			//free context
	}


	
	for(i=0;i<systemSize[0];i++)					//free deliverySystem
		free(deliverySystem[i]);
	free(deliverySystem);
	

}



//print the current state of the whole delivery system (which cells are occupied and the destination of the each occupied cells)
void str_printStorageStatus(void) {
	
	int i, j;
	printf("----------------------------- Delivery Storage System Status (%i occupied out of %i )-----------------------------\n\n", storedCnt, systemSize[0]*systemSize[1]);
	
	printf("\t");
	for (j=0;j<systemSize[1];j++)
	{
		printf(" %i\t\t",j);
	}
	printf("\n-----------------------------------------------------------------------------------------------------------------\n");
	
	for (i=0;i<systemSize[0];i++)
	{
		printf("%i|\t",i);
		for (j=0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].cnt > 0)
			{
				printf("%i,%i\t|\t", deliverySystem[i][j].building, deliverySystem[i][j].room);
			}
			else
			{
				printf(" -  \t|\t");
			}
		}
		printf("\n");
	}
	printf("--------------------------------------- Delivery Storage System Status --------------------------------------------\n\n");
}



//check if the input cell (x,y) is valid and whether it is occupied or not
int str_checkStorage(int x, int y) {
	if (x < 0 || x >= systemSize[0])
	{
		return -1;
	}
	
	if (y < 0 || y >= systemSize[1])
	{
		return -1;
	}
	
	return deliverySystem[x][y].cnt;	
}


//put a package (msg) to the cell
//input parameters
//int x, int y : coordinate of the cell to put the package
//int nBuilding, int nRoom : building and room numbers of the destination
//char msg[] : package context (message string)
//char passwd[] : password string (4 characters)
//return : 0 - successfully put the package, -1 - failed to put
int str_pushToStorage(int x, int y, int nBuilding, int nRoom, char msg[MAX_MSG_SIZE+1], char passwd[PASSWD_LEN+1])
{
	
	deliverySystem[x][y].cnt++;
	deliverySystem[x][y].building=nBuilding;
	strcpy(deliverySystem[x][y].passwd,passwd);
	deliverySystem[x][y].room=nRoom;
	strcpy(deliverySystem[x][y].context,msg);
	storedCnt++;
	
	return 0;
}



//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) {
	
	int result;
	
	result=inputPasswd(x,y);			//password check
	
	if(result!=0)
	{
		return -1;
	}
	else
	{
		initStorage(x,y);
		return 0;
	}
	
}

//find my package from the storage
//print all the cells (x,y) which has my package
//int nBuilding, int nRoom : my building/room numbers
//return : number of packages that the storage system has
int str_findStorage(int nBuilding, int nRoom) {
	
	int i,j;
	int cnt=0;
	
	for(i=0;i<systemSize[0];i++)
	{
		for(j=0;j<systemSize[1];j++)
		{
			if(deliverySystem[i][j].building==nBuilding && deliverySystem[i][j].room==nRoom)	//if find storage cell, print information and finish
			{
			printf("-------------->Found a package in (%d, %d)",i,j);
			return 1;
			}
		
		}
		
	}
	
	return cnt;
}
