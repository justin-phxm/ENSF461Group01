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
size_t firstword(char *outputbuffer, const char *inputbuffer, size_t bufferlen)
{
    // TO DO: Implement this function
    int len = 0;
    while (inputbuffer[len] && inputbuffer[len] != ' ')
    {
        len++;
    }
    outputbuffer = (char *)malloc(len + 1);

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
int findpipe(const char *inputbuffer, size_t bufferlen)
{
    // TO DO: Implement this function
    int ind = -1;
    for (int i = 0; i < bufferlen; i++)
    {
        if (inputbuffer[i] == '|')
        {
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
    char str[bufferlen + 1];
    strncpy(str, inputbuffer, bufferlen);
    str[bufferlen] = '\0';

    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '&')
    {
        return true;
    }

    return false;
}

char *getcommand(const char *inputCommand)
{
    char *outputCommand;
    if (inputCommand && inputCommand[0] == '/')
    {
        outputCommand = (char *)malloc(strlen(inputCommand) + 1);
        strcpy(outputCommand, inputCommand);
        return outputCommand;
    }
    char *binPath = "/usr/bin/";
    outputCommand = (char *)malloc(strlen(binPath) + strlen(inputCommand) + 1);
    strcpy(outputCommand, binPath);
    strcat(outputCommand, inputCommand);
    return outputCommand;
}

char *firstwordpointer(const char *inputbuffer)
{
    if (!inputbuffer)
        return NULL; // handle NULL input

    int len = 0;
    while (inputbuffer[len] && inputbuffer[len] != ' ')
    {
        len++;
    }

    char *outputbuffer = (char *)malloc(len + 1);
    if (!outputbuffer)
        return NULL; // handle memory allocation failure

    strncpy(outputbuffer, inputbuffer, len); // copy only the first word
    outputbuffer[len] = '\0';

    return outputbuffer;
}

char *argsstringpointer(const char *inputbuffer)
{
    if (!inputbuffer || strlen(inputbuffer) == 0)
        return NULL; // handle NULL input

    int len = 0;
    while (inputbuffer[len] && inputbuffer[len] != ' ')
    {
        len++;
    }

    // Check if the found character is space and move to the next character
    if (inputbuffer[len] == ' ')
    {
        len++;
    }

    const char *temp = &inputbuffer[len];
    char *outputbuffer = (char *)malloc(strlen(temp) + 1);
    if (!outputbuffer)
        return NULL; // handle memory allocation failure

    strcpy(outputbuffer, temp);

    return outputbuffer;
}

char** parseInput(const char* input, int* argCount) {
    *argCount = 0;
    int length = strlen(input);
    bool inQuote = false;
    bool newArg = true;

    // Count the number of arguments
    for (int i = 0; i < length; i++) {
        if (input[i] == ' ' && !inQuote) {
            newArg = true;
        } else if (input[i] == '"') {
            inQuote = !inQuote;
        } else if (newArg) {
            (*argCount)++;
            newArg = false;
        }
    }

    char** args = (char**)malloc((*argCount) * sizeof(char*));
    int start = 0;
    int argIndex = 0;
    newArg = true;
    inQuote = false;

    for (int i = 0; i < length; i++) {
        if (input[i] == ' ' && !inQuote) {
            if (!newArg) {
                int len = i - start;
                args[argIndex] = (char*)malloc((len + 1) * sizeof(char));
                strncpy(args[argIndex], &input[start], len);
                args[argIndex][len] = '\0';  // Null terminate
                argIndex++;
            }
            newArg = true;
        } else if (input[i] == '"') {
            inQuote = !inQuote;
            if (inQuote) {
                start = i + 1;
            } else {
                int len = i - start;
                args[argIndex] = (char*)malloc((len + 1) * sizeof(char));
                strncpy(args[argIndex], &input[start], len);
                args[argIndex][len] = '\0';  // Null terminate
                argIndex++;
                newArg = true;
            }
        } else if (newArg) {
            start = i;
            newArg = false;
        }
    }

    return args;
}