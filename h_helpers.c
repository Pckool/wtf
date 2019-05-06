// #include "c_client.h"
// #include "s_server.h"
#include "h_both.h"

char *stringAppend(const char *str1, const char *str2, int len){
	printf("string1: %d\n", strlen(str1));
	printf("string2: %d\n", strlen(str2));
	char *new_str;
	// new_str = malloc(strlen(str1) + strlen(str2) + 1);
	if ( (new_str = malloc(strlen(str1) + strlen(str2) + 1)) != NULL){

		memcpy(new_str, str1, strlen(str1));
		memcpy(new_str + strlen(str1), str2, strlen(str2) + 1); // +1 to copy the null-terminator

		printf("string1: %s\n", new_str);
		return new_str;
	}
	else
	{
		// fprintf(STDERR, "malloc failed!\n");
		printf("malloc failed!\n");
		// exit?
	}
}

char *concat(const char *s1, const char *s2){
	unsigned lineLen = (strlen(s1) + strlen(s2) + 1);
	char *result = (char *)malloc(lineLen * sizeof(char)); // +1 for the null-terminator
	memcpy(result, "\0", lineLen);						   // to ensure that result is a string.
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

/**
	* Take in an int and convert it to a string
	* @param num the int to parse
	* @return str the resulting string. (MUST BE FREED)
	*/
char* parseInt(const int num){
	// printf("%d\n", getLen(num));
	char *str = malloc(20);
	sprintf(str, "%d", num);
	return str;

}


//Returns the number of digits in an int. Was needed at one point but not anymore. Havent deleted yet incase it is needed again
int getLen(int x){
	int toReturn = 0;
	while(x > 0){
		toReturn++;
		x /= 10;
	}
	return toReturn;
}
// removes the first occurance of the string
void removeSubstring(char *s, const char *toremove){
  while( s=strstr(s, toremove) )
    memmove(s, s+strlen(toremove), 1+strlen(s+strlen(toremove)));
}

/**
	* Adds char to the string that is being added to the codebook
	* The way I implemented this is actually has the potential to create memory leaks.
	* Thereason is because I can also pass a malloced string, and unless that is handled by the function that calles this, it will just abandon that original malloced string.
	* If this isn't fixed I ran out of time.
	*/
char *charAppend(char *str, char charr){
	char *newStr = malloc(strlen(str)+2);
	strcpy(newStr, str);
	newStr[strlen(str)] = charr;
	newStr[strlen(str)+1] = '\0';
	return newStr;
}

int startsWith(const char *pre, const char *str){
    // size_t lenpre = strlen(pre),
    //        lenstr = strlen(str);
    // return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
	if(strncmp(pre, str, strlen(str)) == 0) return 1;
   	return 0;
}

char *getLine(char *str){
    printf("Extracting the first line from %s\n", str);
    char *temp = malloc(sizeof(char));
    memcpy(temp, "\0", sizeof(char));

    char *final = malloc(sizeof(char));
    memcpy(final, "\0", sizeof(char));
    char lett;
    int i;
    for(i=0; i<strlen(str); ++i){

        lett = str[i];
        temp = charAppend(temp, lett);

        if(lett == '\n'){ // if the letter is a new line terminator then we exit the loop
            i = strlen(str) - 1;
        }
    }
    return temp;
}

void loading(){
	// do something
	printf("\b/");
	fflush(stdout);
	// do some more
	printf("\b-");
	fflush(stdout);

	printf("\b\\");
	fflush(stdout);
}

// returns the byte content of a file as a string to use when writing
char *getByteContent(char *filePath){
	FILE *fileptr;
	char *buffer;
	long filelen;

	fileptr = fopen(filePath, "rb");  // Open the file in binary mode
	fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
	filelen = ftell(fileptr);             // Get the current byte offset in the file
	rewind(fileptr);                      // Jump back to the beginning of the file

	buffer = (char *)malloc((filelen+1)*sizeof(char)); // Enough memory for file + \0
	fread(buffer, filelen, 1, fileptr); // Read in the entire file
	fclose(fileptr); // Close the file
	return buffer;
}


/**
	* This function is essential in understanding and working with the 
	* netowrk protocol. It takes in a string (the buffer) and a pointer to a
	* head (of type ProtocolLink) and returns a pointer (of type ProtocolLink) 
	* that is a linked list of all of the tokens in a protocol message.
	*/
ProtocolLink *tokenizeProtocolMessage(char *inputMsg, ProtocolLink *head){
	printf("1\n");
	char *tempToken = (char *)malloc(sizeof(char));
	memcpy(tempToken, "\0", sizeof(char));
	printf("2\n");
	int i = 0;
	int lenOfMsg = strlen(inputMsg);

	char *copyToManipulate = (char *)malloc(lenOfMsg+1);
	strcpy(copyToManipulate, inputMsg);

	char *copyToManipulate2 = (char *)malloc(lenOfMsg+1);
	strcpy(copyToManipulate2, inputMsg);

	printf("3\n");
	while(i<lenOfMsg){

		char frontLetter[2] = {copyToManipulate[0], '\0'};
		char nextLetter[2] = {copyToManipulate[1], '\0'};
		if( strcmp(frontLetter,":") != 0){ // while we don't have the token separating char
			printf("\tAPPENDING STRING\n");
			// append the char to the temp string
			tempToken = charAppend(tempToken, copyToManipulate[0]);

			printf("\tREMOVING FIRST LETTER IN STRING\n");
			// remove the first char
			
			printf("\tCREATED FIRST LETTER `%c` AS STRING: %s\n", copyToManipulate[0], frontLetter);
			copyToManipulate = strstr(++copyToManipulate, nextLetter);
			
		}
		else{ // we have found the token char
			printf("4\n");
			head->next = (ProtocolLink *)malloc(sizeof(ProtocolLink));
			head->next = newProtocolLink(tempToken);
			free(tempToken);

			// remove the first char
			copyToManipulate = strstr(++copyToManipulate, nextLetter);
			printf("5\n");
			tokenizeProtocolMessage(copyToManipulate, head->next);
			break;

		}
		++i;
	}
	return head;
}

ProtocolLink *newProtocolLink(char *token){
	ProtocolLink *link = (ProtocolLink *)malloc(sizeof(ProtocolLink));
	link->token = (char *)malloc(strlen(token) * sizeof(char) + 1);
	memcpy(link->token, "\0", strlen(token) * sizeof(char) + 1);
	strcpy(link->token, token);

	// link->next = (ProtocolLink *)malloc(sizeof(ProtocolLink));
	// memcpy(link->next, 0x0, sizeof(ProtocolLink));
	link->next = NULL;
	return link;
}