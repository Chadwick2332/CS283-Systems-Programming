#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include "stringly.h"

struct params;
const int MAX_DIRNAME_LEN = 1024;
void scan_dir(const char* dirname, const char* context, const char* path, struct params* params);

// Encapsulate
struct params {
  char* pattern;
  char* replace;
  char* fname;
  int mode;
};

int main(int argc, char** argv)
{
  char* dirname = (char*) malloc(sizeof(char) * MAX_DIRNAME_LEN);
  struct params* params = (struct params*) malloc(sizeof(struct params));
  int c;
  
  while((c = getopt(argc, argv, "d:p:r:f:m:")) != -1) {
    switch(c) {
      case 'd':
        strncpy(dirname, optarg, strlen(optarg) + 1);
        break;
      case 'p':
        params->pattern = (char*) malloc(sizeof(char) * 50);
        strncpy(params->pattern, optarg, strlen(optarg) + 1);
        break;
      case 'r':
        params->replace = (char*) malloc(sizeof(char) * 50);
        strncpy(params->replace, optarg, strlen(optarg) + 1);
        break;
      case 'f':
        params->fname = (char*) malloc(sizeof(char) * 50);
        strncpy(params->fname, optarg, strlen(optarg) + 1);
        break;
      case 'm':
        params->mode = atoi(optarg);
        break;
      case '?':
        if(optopt == 'p' || optopt == 'r' || optopt == 'm') {
          fprintf(stderr, "Option -%c requires an argument\n", optopt);
        }
        else {
          fprintf(stderr, "Unknown error! Try again.\n");
        }
        return 1;
    }
  }
  
  if(dirname == NULL) {
    dirname = "./";
  }
  
  scan_dir(dirname, NULL, NULL, params);
  free(dirname);
  free(params->pattern);
  free(params->replace);
  if(params->fname != NULL) {
    free(params->fname);
  }
  free(params);
  return 0;
}

void scan_dir(const char* dirname, const char* context, const char* path, struct params* params) {
  // If dirname is self or parent, stop
  if(path) {
    if(strncmp(path, "..", strlen(path)) == 0 || strncmp(path, ".", strlen(path)) == 0) {
      return;
    }
  }
  DIR* dir;
  struct dirent* dir_meta;
  struct stat* current_file_stat;
  char* search_path = (char*) malloc(sizeof(char) * MAX_DIRNAME_LEN);
  char* relative_path = (char*) malloc(sizeof(char) * MAX_DIRNAME_LEN);
  // Construct paths
  // search_path is the path that the program will attempt to search and open
  // relative_path is a helper for recursive calls and file creation/deletion
  if(path) {
    if(context) {
      snprintf(search_path, MAX_DIRNAME_LEN, "%s/%s/%s", dirname, context, path);
      snprintf(relative_path, MAX_DIRNAME_LEN, "%s/%s", context, path);
    } else {
      snprintf(search_path, MAX_DIRNAME_LEN, "%s/%s", dirname, path);
      strncpy(relative_path, path, MAX_DIRNAME_LEN);
    }
  } else {
    strncpy(search_path, dirname, strlen(dirname) + 1);
    relative_path = NULL;
  }
  if(!(dir = opendir(search_path))) {
    fprintf(stderr, "Couldn't open directory %s.\n", search_path);
    return;
  } else {
    current_file_stat = malloc(sizeof(struct stat));
    while((dir_meta = readdir(dir))) {
      // Found a subdirectory -- recurse
      if(dir_meta->d_type == DT_DIR) {
        scan_dir(dirname, path, dir_meta->d_name, params);
        if((params->fname != NULL && strstr(search_path, params->fname)) ||
           params->fname == NULL)
        {
          char* new_name = (char*) malloc(sizeof(char) * MAX_DIRNAME_LEN);
          strncpy(new_name, search_path, strlen(search_path) + 1);
          new_name = str_replace(new_name, params->pattern, params->replace);
          rename(search_path, new_name);
          strncpy(search_path, new_name, strlen(new_name) + 1);
          free(new_name);
        }
      } else {
        // Nope, we're looking at a file
        int stat_success;
        char *file_path = (char*) malloc(sizeof(char) * MAX_DIRNAME_LEN);
        // If there's a path param, construct correct relative path to file
        // Else, we're in the base of the dir, so append base dir name
        if(path) {
          snprintf(file_path, MAX_DIRNAME_LEN, "%s/%s", search_path, dir_meta->d_name);
        } else {
          snprintf(file_path, MAX_DIRNAME_LEN, "%s/%s", dirname, dir_meta->d_name);
        }
        if((params->fname != NULL && strstr(file_path, params->fname)) ||
            params->fname == NULL)
        {
          if((stat_success = stat(file_path, current_file_stat)) == 0) {
            if(S_ISREG(current_file_stat->st_mode) && (current_file_stat->st_mode & S_IRUSR)) {
              // File renaming mode
              // else, file searching mode
              if(params->mode == 0) {
                char* new_name = (char*) malloc(sizeof(char) * MAX_DIRNAME_LEN);
                strncpy(new_name, file_path, strlen(file_path) + 1);
                new_name = str_replace(new_name, params->pattern, params->replace);
                rename(file_path, new_name);
                free(new_name);
              }
              else if(params->mode == 1) {
                char* buffer = (char*) malloc(sizeof(char) * 1024);
                char* replace_string = (char*) malloc(sizeof(char) * strlen(params->pattern) + 1);
                strncpy(replace_string, params->pattern, strlen(params->pattern) + 1);
                strncat(replace_string, params->replace, strlen(params->replace) + 1);
                FILE* replace_file = fopen(file_path, "r+");
                FILE* pFile = tmpfile();
                if(replace_file != NULL) {
									// Read entire file into string to check for pattern
									fseek(replace_file, 0, SEEK_END);
									long fsize = ftell(replace_file);
									fseek(replace_file, 0, SEEK_SET);
									char* file_check = (char*) malloc(sizeof(char) * fsize + 1);
									fread(file_check, fsize, 1, replace_file);
									
									// If the pattern isn't in the file, start the prepending process
									if(strstr(file_check, params->pattern) == NULL) {
										rewind(replace_file);
	                  // Read from original file to tmpfile
	                  // Make proper appends and replacements in tmpfile
	                  while(fgets(buffer, 1024, replace_file) != NULL) {
											buffer = str_replace(buffer, params->replace, replace_string);
	                    fputs(buffer, pFile);
	                  }
	                  rewind(replace_file);
	                  rewind(pFile);
                  
	                  // Read from tmp file to original file, overwriting data
	                  while(fgets(buffer, 1024, pFile) != NULL) {
	                    fputs(buffer, replace_file);
	                  }
									}
                  fclose(replace_file);
									free(file_check);
                } else {
                  fprintf(stderr, "Error opening file!\n");
                }
                fclose(pFile);
                free(buffer);
                free(replace_string);
              }
            }
          } else {
            fprintf(stderr, "%s\n", strerror(errno));
          }
          free(file_path);
        }
      }
    }
    free(current_file_stat);
    free(dir_meta);
    closedir(dir);
  }
  free(search_path);
  free(relative_path);
}
