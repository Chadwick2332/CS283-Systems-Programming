//
//  stringly.c
//  assignment2_xcode
//
//  Created by Christian Benincasa on 2/7/13.
//  Copyright (c) 2013 Christian Benincasa. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include "stringly.h"

// trims newlines, whitespace
// result must be free'd later
char* sanitize(char* string) {
  char* ret;
  ret = str_replace(string, "\n", "");
  ret = str_replace(ret, " ", "");
  return ret;
}

// Replace all occurrences of 'rep' with 'with' in 'orig'
char* str_replace(char *orig, char *rep, char *with) {
  char *result, *ins, *tmp;
  int len_rep, len_with, len_front, replacements_count;
  // if original string isn't defined, abort routine
  if (!orig) {
    return NULL;
  }
  // length of string to be replaced
  len_rep = (int) strlen(rep);
  if (!rep || !len_rep) {
    return NULL;
  }
  // is string to be replaced even in original string?
  ins = strstr(orig, rep);
  if (ins == NULL) {
    return orig;
  }
  // if no replacement string exists, replace with empty string
  if (!with) {
    with = "";
  }
  // length of replacement string
  len_with = (int) strlen(with);
  for (replacements_count = 0; (tmp = strstr(ins, rep)); ++replacements_count) {
    ins = tmp + len_rep;
  }
  tmp = result = (char*) malloc(strlen(orig) + (len_with - len_rep) * replacements_count + 1);
  if (!result) {
    return NULL;
  }
  while (replacements_count--) {
    ins = strstr(orig, rep);
    len_front = (int)(ins - orig);
    tmp = strncpy(tmp, orig, len_front) + len_front;
    tmp = strcpy(tmp, with) + len_with;
    orig += len_front + len_rep;
  }
  strcpy(tmp, orig);
  return result;
}

int string_compare(const void * a, const void * b) {
  return (*(int*) a < *(int*) b);
}

void strextend(char** string, unsigned long* size) {
  char* new = realloc(*string, *size * 2);
  if(new) {
    *string = new;
    *size *= 2;
  }
}