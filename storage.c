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
	deliverySystem[x][y].cnt=0;	//save the number of object is 0 in storage (x,y) 

	
}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching, -1 - password is not matching
static int inputPasswd(int x, int y)
{
	
		char getpass[PASSWD_LEN+1];				
		int result;
	
		printf(" - input password for (%d, %d) storage",x,y);
		scanf("%s",getpass);					//get password
		
		result=strcmp(getpass,deliverySystem[x][y].passwd);		//compare password
		
		if(result!=0)					//getpassword =/= storage's password
		{
			printf("-------------------------> password is wrong !!\n");
			return -1;
		}
		else 							//getpassword = storage's password
		{
			initStorage(x,y);			//reset storage cell
			return 0;
		}

}





// ------- API function for main.c file ---------------

//backup the delivery system context to the file system
//char* filepath : filepath and name to write
//return : 0 - backup was successfully done, -1 - failed to backup
int str_backupSystem(char* filepath) {
	
	int i,j;
	
	FILE *fp;
	
	fp=fopen(filepath,"w");								//file open writing mode 
	
	fprintf(fp,"%d %d\n",systemSize[0],systemSize[1]);	//save systemsize and masterpassword
	fprintf(fp,"%s\n",masterPassword);
	
	for(i=0;i<systemSize[0];i++)
	{
		for(j=0;j<systemSize[1];j++)
		{
			//if there is object in storage cell
			if(deliverySystem[i][j].cnt!=0)			
			//save informations
			fprintf(fp,"%d %d %d %d %s %s\n",i,j,deliverySystem[i][j].building,deliverySystem[i][j].room,deliverySystem[i][j].passwd,deliverySystem[i][j].context);
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
	
	int i;
	int x,y;
	int c; 
	int nBuilding;
	
	 
	
	FILE *fp;
	
	fp=fopen(filepath,"r");//open file
	//정보를 가져와서 먼저 저장한 후에 메모리 할당  

	fscanf(fp,"%d %d\n",&systemSize[0],&systemSize[1]);
	fscanf(fp,"%s\n",masterPassword);
	
	/*deliverySystem=(struct storage_t**)malloc(systemSize[0]*sizeof(struct storage_t*));*/
	



	
	while(c=fgetc(fp)!=EOF)//공백이 나올떄까지 
	 {
	 	fscanf(fp,"%d %d ",&x,&y);
	 	fscanf(fp,"%d ",&deliverySystem[x][y].building);
	 	fscanf(fp,"%d ",&deliverySystem[x][y].room);
	 	fscanf(fp,"%s ",deliverySystem[x][y].passwd);
	 	fscanf(fp,"%s ",deliverySystem[x][y].context);
	 	
	 }
	 
	
	
	if(fclose(fp)==0)
	{
		return 0;
	}
	else
	{
		return -1;
	
	}
	/*int i,j;	
	deliverySystem=(struct storage_t**)malloc(sizeof(struct storage_t*));
	for(i=0;i<4;i++)
	{
		for(j=0;j<6;j++)
		{
		deliverySystem[i][j]=(struct storage_t*)malloc(systemSize[0]*systemSize[1]);	
		}
	}*/

	
			


	return 0;

}

//free the memory of the deliverySystem 
void str_freeSystem(void) {
	
	
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
	
	deliverySystem[x][y].building=nBuilding;
	strcpy(deliverySystem[x][y].passwd,passwd);
	deliverySystem[x][y].room=nRoom;
	strcpy(deliverySystem[x][y].context,msg);
}



//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) {
	
	int result;
	
	result=inputPasswd(x,y);
	
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
	
	for(i=0;i<3;i++)
	{
		for(j=0;j<5;i++)
		{
			if(deliverySystem[i][j].building==nBuilding && deliverySystem[i][j].room==nRoom)
			cnt=1;
			break;
		}
		
		if(deliverySystem[i][j].building==nBuilding && deliverySystem[i][j].room==nRoom)
			break;
		
	}


	printf("-------------->Found a package in (%d, %d)",i,j);
	
	return cnt;
}
