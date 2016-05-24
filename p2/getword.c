/* 
Arthur Flores
Program 1
Professor Carroll
CS570
Februrary 2016
getword.c file

This file is used to count the number of charcters in a word and 
stores each character in an array so the program p1 can print it out. This
program is similar to program 0 but has many more specifications. 

*/

/* Include Files */
#include "getword.h"

int DSFLAG = 0;

int getword( char *w )
{
    int iochar; // This variable is used to store getchar() character
    int count = 0; // This increments everytime a charcater is read by getchar
    char *word = w; // This references the pointer for the address in the array 
    int backslash = 0; // This variable indicates whether a backslash has been seen or not
    int apostraphe = 0; // This variable indicates whether or not a apostraphe has been seen or not                                     
    DSFLAG = 0;

    while ( ( iochar = getchar() ) != EOF ){
 	if(count == 254){ //Makes sure that the arrays storage doesnt get full
		ungetc(iochar,stdin);
		/**word = '\0';*/
		return count;
	}
	
    	/* This statement below initiates that a backslash has been seen and triggers special functions*/
    	if( iochar == '\\' ){ 
		iochar = getchar();
		if(iochar == '$' && count == 0){
			DSFLAG = 1;
		}
		if( apostraphe == 0)  
			backslash = 1;
		else{
			if( iochar == '\'' ){   //Checks to see if apostraphe equals 1 and there is a backlsash you can insert an '
				*word++ = iochar;
				count++;
				iochar = getchar();
			}
			else{			//If the next character after the backslash when apostraphe equals 1 then we just add backslash to word
				*word++ = '\\';
				count++;
			}	
		}
	}

	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* This statement below deals with a space with and without a backslash*/		
    	if( iochar == ' '){
		
		if(backslash == 1 || apostraphe == 1){ // This lets a space be involved in the word within quotes or after a backslash
			*word++ = iochar;
			count++;
		}
		else if(count == 0){}
		else{
			*word = '\0'; // Places a null character in array to stop reading word from array 
    			return count; // Returns to p0 program
		}	
	} 
	/* This statement below handles all metacharacters with and without a backslash*/
	else if( iochar == '\n' || iochar == '<'|| iochar == '>'|| iochar == '\'' || iochar == '|'|| iochar == ';'|| iochar == '&' ){

		if ( iochar == '\n' || iochar == ';'){		//This will handle any newline character that is entered
			if(count == 0){
				*word = '\0';
				return count;
			}	
			*word = '\0'; // Places a null character in array to stop reading word from array
			ungetc(iochar,stdin);
			if(apostraphe == 1){ 		//catches for closing quote
   				return -2;
   			}
    			return count; // Returns to p0 program
		}
		else if (backslash == 1){ // This make sure that the metacharacter after the backslash gets added to the word
			*word++ = iochar;
			count++;
		}
		else if( iochar == '\'' && apostraphe == 0){ //Shows the open quote and shows to move on to see end quote, will detect in middle of word
			apostraphe++;	
		}
		else if( iochar == '\'' && apostraphe == 1){ //Shows the closed quote has been found and passes word through p1
			apostraphe = 0;
			continue;
		}	
		else if( count != 0 && apostraphe == 0){ // This point shows that the metacharcter was first seen and no backslash
			*word = '\0'; // Places a null character in array to stop reading word from array
			ungetc(iochar,stdin);
    			return count; // Returns to p0 program
			}	
		else if( apostraphe == 1){ // This makes sure that metacharacters are involved within quote marks.
			*word++ = iochar;
			count++;
		}	
		else{ // This shows the metacharacter has already been seen so we need to add this information to p1 by itself where n=1
			*word++ = iochar;
			count++;
			*word = '\0';
			return count;
		}
	}
	/* This statement below adds the  'normal' input character to the array*/
	else{
		*word++ = iochar; // This places the iochar character into the array to be read
		count++;	  // This gets incremented since a character has been successfully passed thru
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	backslash = 0; //This resets the backslash so eventually it allows word seperation
		
    }
   if(apostraphe == 1){
   	return -2;
   }
   *word = '\0'; // This gets excuted when there is an EOF symbol read so 2nd part of printf in p0 gets a blank character
   if( count != 0){	//This lets the word before end of file was reached to be read in p1
   	return count;
   }
   return -1;  //This is the value that is supposed to be printed when EOF is reached

    	             
}
