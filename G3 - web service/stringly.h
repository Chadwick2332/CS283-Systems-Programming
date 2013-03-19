//
//  stringly.h
//  assignment2_xcode
//
//  Created by Christian Benincasa on 2/7/13.
//  Copyright (c) 2013 Christian Benincasa. All rights reserved.
//

#ifndef assignment2_xcode_stringly_h
#define assignment2_xcode_stringly_h

char* str_replace(char *orig, char *rep, char *with);
char* sanitize(char* string);
int string_compare(const void * a, const void * b);
void strextend(char** string, unsigned long* size);

#endif
