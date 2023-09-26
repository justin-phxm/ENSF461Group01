#include "parser.h"

// Command to trim whitespace and ASCII control characters from buffer
//[Input] char* inputbuffer - input string to trim
//[Input] size_t bufferlen - size of input and output string buffers
//[Output] char* outputbuffer - output string after trimming
//[Return] size_t - size of output string after trimming
size_t trimstring(char *outputbuffer, const char *inputbuffer, size_t bufferlen)
{
    memcpy(outputbuffer, inputbuffer, bufferlen * sizeof(char));

    for (size_t ii = strlen(outputbuffer) - 1; ii >= 0; ii--)
    {
        if (outputbuffer[ii] < '!') // In ASCII '!' is the first printable (non-control) character
        {
            outputbuffer[ii] = 0;
        }
        else
        {
            break;
        }
    }

    return strlen(outputbuffer);
}

// Command to trim the input command to just be the first word
//[Input] char* inputbuffer - input string to trim
//[Input] size_t bufferlen - size of input and output string buffers
//[Output] char* outputbuffer - output string after trimming
//[Return] size_t - size of output string after trimming
size_t firstword(char* outputbuffer, const char* inputbuffer, size_t bufferlen)
{
    //TO DO: Implement this function
    int len = 0;
    while (inputbuffer[len] && inputbuffer[len] != ' '){
        len++;
    }
    outputbuffer = (char*) malloc(len + 1);

    strcpy(outputbuffer, inputbuffer);

    return len;
}

// Command to test that string only contains valid ascii characters (non-control and not extended)
//[Input] char* inputbuffer - input string to test
//[Input] size_t bufferlen - size of input buffer
//[Return] bool - true if no invalid ASCII characters present
bool isvalidascii(const char *inputbuffer, size_t bufferlen)
{
    // TO DO: Correct this function so that the second test string fails
    size_t testlen = bufferlen;
    size_t stringlength = strlen(inputbuffer);
    if (strlen(inputbuffer) < bufferlen)
    {
        testlen = stringlength;
    }

    bool isValid = true;
    for (size_t ii = 0; ii < testlen; ii++)
    {
        isValid &= ((unsigned char)inputbuffer[ii] <= '~'); // In (lower) ASCII '~' is the last printable character
        isValid &= ((unsigned char)inputbuffer[ii] >= ' '); // In (lower) ASCII ' ' is the first printable character
        }

    return isValid;
}

// Command to find location of pipe character in input string
//[Input] char* inputbuffer - input string to test
//[Input] size_t bufferlen - size of input buffer
//[Return] int - location in the string of the pipe character, or -1 pipe character not found
int findpipe(const char* inputbuffer, size_t bufferlen){
    //TO DO: Implement this function
    int ind = -1;
    for (int i = 0; i < bufferlen; i++){
        if (inputbuffer[i] == '|'){
            ind = i;
            break;
        }
    }
    
    return ind;
}

// Command to test whether the input string ends with "&" and
// thus represents a command that should be run in background
//[Input] char* inputbuffer - input string to test
//[Input] size_t bufferlen - size of input buffer
//[Return] bool - true if string ends with "&"
bool runinbackground(const char *inputbuffer, size_t bufferlen)
{
    // TO DO: Implement this function

    return false;
}

char * getcommand(const char* inputCommand, char* outputCommand){
    if (inputCommand && inputCommand[0] == '/'){
        outputCommand = (char*) malloc(strlen(inputCommand) + 1);
        strcpy(outputCommand, inputCommand);
        return outputCommand;
    }
    char * binPath = "/usr/bin/";
    outputCommand = (char*) malloc(strlen(binPath) + strlen(inputCommand) + 1);
    strcpy(outputCommand, binPath);
    strcat(outputCommand, inputCommand);
    return outputCommand;
}
