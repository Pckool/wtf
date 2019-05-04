#include "c_client.h"
#include "s_server.h"

/**
	* Adds char to the string that is being added to the codebook
	* The way I implemented this is actually has the potential to create memory leaks.
	* Thereason is because I can also pass a malloced string, and unless that is handled by the function that calles this, it will just abandon that original malloced string.
	* If this isn't fixed I ran out of time.
	*/
char *charAppend(char str[], char charr){
	char *newStr = malloc(strlen(str)+2);
	strcpy(newStr, str);
	newStr[strlen(str)] = charr;
	newStr[strlen(str)+1] = '\0';
	return newStr;
}

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
