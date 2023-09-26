#ifndef __PARSER_H
#define __PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

//ToDo: Here are some suggested parsing commands, you may want to expand this list, 
//Please feel free to change modify the commands below to suit your needs

size_t trimstring(char* outputbuffer, const char* inputbuffer, size_t bufferlen);
size_t firstword(char* outputbuffer, const char* inputbuffer, size_t bufferlen);
bool isvalidascii(const char* inputbuffer, size_t bufferlen);
int findpipe(const char* inputbuffer, size_t bufferlen);
bool runinbackground(const char* inputbuffer, size_t bufferlen);

#endif