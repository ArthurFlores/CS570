/*
Arthur Flores
Program 2
Professor Carroll
CS570
Februrary 29 2016
p2.c file

This is the file that will issue a prompt for the user and read
the input and do some stuff with that input.
*/
#include "p2.h"

extern int DSFLAG;

char argv[MAXSTORAGE];
char *newargv[MAXITEM];
char *IN , *OUT;		//points to the in/out file in argv
int num_chars;			//will detect if there is a EOF character
int BGFLAG = 0,INFLAG = 0,OUTFLAG = 0 , PIPEFLAG = 0; //FLAGS for the <, >, &


int main(){
	signal(SIGTERM,myhandler);
	pid_t pid, child_pid;
	DIR *dirp;
	struct stat sb;
	struct dirent *dp;
	int argc;
	int o_file, i_file;	//this is used for redirection
	for(;;){
		printf("p2: ");
		argc = parse();
		if(num_chars == -2){
			fprintf(stderr,"Unmatched '.\n");
			continue;
		}
		if(num_chars == -1)  //This will terminate the program when getword sees EOF
			break;
		if(argc == 0)		//Will re-issue prompt if no words entered
			continue;
		if(newargv[0] == NULL){		//This will be used if the <,> are flagged and no arg
			fprintf(stderr,"Invalid null command.\n");
			continue;
		}
/////////////////"|" code//////////////////////////////		
		if(PIPEFLAG != 0){
			if(PIPEFLAG > 1){
				fprintf(stderr,"Error '|': Must contain only ONE pipe.\n");
				continue;
			}
			else{
				printf("pipe seen\n");
			}
		}
/////////////////printenv code//////////////////////////////
		if(strcmp(newargv[0],"printenv") == 0){
			if(argc <= 1 || argc > 2)	//Too many arguments to cd
				fprintf(stderr,"printenv: Must contain only ONE argument.\n");
			else{
				printf("printenv seen\n");
			}
			continue;
		}		
/////////////////setenv code//////////////////////////////
		if(strcmp(newargv[0],"setenv") == 0){
			if(argc <= 2 || argc > 3)  //Only one argument allowed
				fprintf(stderr,"setenv: Must contain only TWO argument.\n");
			else{
				printf("setenv seen\n");
			}
			continue;
		}				
/////////////////cd code//////////////////////////////
		if(strcmp(newargv[0],"cd") == 0){
			if(argc > 2)	//Too many arguments to cd
				fprintf(stderr,"cd: Too many arguments.\n");
			else if(argc == 1){	//cd was entered with no arguments 
				if(chdir(getenv("HOME")) != 0)
					fprintf(stderr,"cd: could not find home directory.\n");
			}
			else{	//argc is two so you want to cd to the argument
				if(chdir(newargv[1]) != 0)
					fprintf(stderr,"%s: No such file or directory.\n",newargv[1]);
			}
			continue;
		}
/////////////////ls-F code//////////////////////////////
		if(strcmp(newargv[0],"ls-F") == 0){
			if(argc > 2){	//Too many arguments to ls-F
				fprintf(stderr,"ls-F: Too many arguments.\n");
			}
			if(argc == 1){	//ls-F was entered with no arguments
				dirp = opendir(".");
				if(stat(".",&sb) == -1){
					fprintf(stderr,"stat ls-F failed.\n");
					exit(EXIT_FAILURE);
				}
				while(dirp){ //This will read through the file
					if((dp = readdir(dirp)) != NULL){
						if(stat(dp->d_name, &sb) == -1)
							fprintf(stderr,"stat ls-F failed.\n");
						if((sb.st_mode & S_IFMT) == S_IFDIR) //This states its a directory
							printf("%s\n",dp->d_name);
						else
							printf("%s\n",dp->d_name);
					}
					else{
						closedir(dirp);
						break;
					}
				}
			}	
			else{	//This executes when ls-F has an argument with it
				if(stat(newargv[1], &sb) == -1){
					fprintf(stderr,"%s: No such file or directory.\n",newargv[1]);
					continue;
				}
				if((sb.st_mode & S_IFMT) != S_IFDIR){ //IF NOT DIRECTORY
					printf("%s\n",newargv[1]);
					continue;
				}
				if(chdir(newargv[1]) != 0){ //If a directory but unreadable
					fprintf(stderr,"%s is unreadable\n",newargv[1]);
					continue;
				}
				if((dirp = opendir(newargv[1])) == NULL){
					printf("%s: failed to open directory\n",newargv[1]);
					continue;
				}	
				while(dirp){
					if((dp = readdir(dirp)) != NULL){
						if(stat(dp->d_name, &sb) == -1)
							fprintf(stderr,"stat ls-F failed.\n");
						if((sb.st_mode & S_IFMT) == S_IFDIR)
							printf("%s\n",dp->d_name);
						else
							printf("%s\n",dp->d_name);
					}
					else{
						closedir(dirp);
						break;
					}
				}
			}
			continue;
		}
///////////////// OUTFLAG ">" code//////////////////////////////
		if(OUTFLAG != 0){
			int flags = O_CREAT | O_EXCL | O_RDWR ;
			if(OUTFLAG > 2){
				fprintf(stderr,"Ambiguous input redirect.\n");
				continue;
			}
			if(OUT == NULL){
				fprintf(stderr,"Missing name for redirect.\n");
				continue;
			}
			if((o_file = open(OUT,flags,S_IRWXU)) < 0){
				fprintf(stderr,"%s: Already Exists.\n",OUT);
				OUTFLAG = 0;
				continue;
			}
		}
///////////////// INFLAG "<" code//////////////////////////////
		if(INFLAG != 0){
			int flags = O_RDONLY;
			if(INFLAG > 2){
				fprintf(stderr,"Ambiguous input redirect.\n");
				continue;
			}
			if(IN == NULL){
				fprintf(stderr,"Missing name for redirect.\n");
				continue;
			}
			if((i_file = open(IN,flags)) < 0){
				fprintf(stderr,"Failed to open: %s\n",IN);
				continue;
			}
		}
///////////////// fork code//////////////////////////////
		fflush(stdout);
		fflush(stderr);
		child_pid = fork();
		if(child_pid < 0){
			printf("Terminating! Cant Fork!");
			exit(1);
		}
		else if(child_pid == 0){
//////////////////dev/null code////////////////////////////////
			if(BGFLAG != 0 && INFLAG == 0){		//This will send STDIN to /dev/null/
				int devnull;
				int flags = O_RDONLY;
				if((devnull = open("/dev/null",flags)) < 0){
					fprintf(stderr,"Failed to open /dev/null.\n");
					exit(9);
				}
				dup2(devnull,STDIN_FILENO);
				close(devnull);
			}
///////////////// OUTFLAG ">" code//////////////////////////////
			if(OUTFLAG != 0){
				dup2(o_file,STDOUT_FILENO);
				close(o_file);
			}
///////////////// INFLAG "<" code//////////////////////////////
			if(INFLAG != 0){
				dup2(i_file,STDIN_FILENO);
				close(i_file);
			}
			
/////////////////////now excecvp//////////////////////////////
			if((execvp(*newargv, newargv)) < 0){	//execute the command
				fprintf(stderr,"%s: Command not found.\n",newargv[0]);
				exit(9);
			}
				
		}
//////////////////// test whether to wait or not ////////////////////
		if(BGFLAG !=0){	//when & you will place in background and set STDIN to /dev/null
			printf("%s [%d]\n", *newargv , child_pid);
			BGFLAG = 0;
			continue;
		}
		else{	//if not background then wait till its done
			for(;;){
				pid = wait(NULL);
				if(pid == child_pid)
					break;
			}
		}
	}
	killpg(getpid(),SIGTERM);
	printf("p2 terminated.\n");
	exit(0);
}

///////////////// parse method //////////////////////////////
int parse(){
	int p = 0;
	int ptr = 0;
	int word_count=0;
	IN = '\0';
	OUT = '\0';
	INFLAG = 0;
	OUTFLAG = 0;
	BGFLAG = 0;
	PIPEFLAG = 0;
	
	while((num_chars = getword(argv + ptr)) > 0 ){
		if(num_chars == -2){		//If there is no closing quote
			break;
		}
		if(*(argv+ptr) == '$' && DSFLAG == 0){
			printf("$ seen\n");
		}
		if(*(argv+ptr) == '|'){
			PIPEFLAG++;
			printf("| seen\n");
		}		
		if(*(argv+ptr) == '&'){	//background check
			 if((IN == NULL && OUT == NULL) && (INFLAG == 1 || OUTFLAG == 1)){ //looks after >
				break;
			}
			BGFLAG++;
			break;
		}
		else if((*(argv+ptr) == '<' && num_chars == 1) || INFLAG == 1){	//sets the inflag <
			INFLAG++;
			if(INFLAG == 2)
				IN = argv +ptr;
		}
		else if((*(argv+ptr) == '>' && num_chars == 1) || OUTFLAG == 1){ //sets the outflag >
			OUTFLAG++;
			if(OUTFLAG == 2)
				OUT = argv+ptr;
		}
		else{
			newargv[p++] =  argv + ptr;
		}
////////////////////////////////////////////////////////////////////////////////////	
		argv[ptr+ num_chars] = '\0';
		ptr += num_chars+1;
		word_count++;

	}
	newargv[p] = NULL;
	return word_count;
	
}
///////////////// method to handle SIGTERM //////////////////////////////
void myhandler(){};
