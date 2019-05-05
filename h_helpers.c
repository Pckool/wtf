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

void removeSubstring(char *s,const char *toremove){
  while( s=strstr(s,toremove) )
    memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
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