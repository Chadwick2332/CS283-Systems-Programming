#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include "csapp.h"

const int MAX_DIRNAME_LEN = 1024;
int DRY_RUN = 0;
int VERBOSE = 1;
void scan_dir(const char *dirname, const char *path, const char* context, const char* sync_dir, int mode);
void sync_directories(const char *master, const char *slave);
char* find_file_in_dir(const char* dir_to_search, const char* file_path);
int find_dir_in_dir(const char* dir_to_search, const char* dir_path);
int copy_file(FILE* a, FILE* b);

int main(int argc, char* argv[])
{
//  char* master_dir = (char*) malloc(sizeof(char) * MAX_DIRNAME_LEN);
//  char* slave_dir = (char*) malloc(sizeof(char) * MAX_DIRNAME_LEN);
//  int c;
//  
//  while((c = getopt(argc, argv, "m:s:dv")) != -1) {
//    switch(c) {
//      case 'm':
//        master_dir = optarg;
//        break;
//      case 's':
//        slave_dir = optarg;
//        break;
//      case 'd':
//        DRY_RUN = 1;
//        break;
//      case 'v':
//        VERBOSE = 1;
//        break;
//      case '?':
//        if(optopt == 'm'){
//          fprintf(stderr, "You must specify a master directory\n");
//        }
//        else if(optopt == 's') {
//          fprintf(stderr, "You must specify a slave directory\n");
//        }
//        return EXIT_FAILURE;
//    }
//  }
//  if(master_dir != NULL && slave_dir != NULL) {
//    
//    free(master_dir);
//    free(slave_dir);
//  } else {
//    return EXIT_FAILURE;
//  }
  
  if(argc < 3) {
    return EXIT_FAILURE;
  } else {
    sync_directories(argv[1], argv[2]);
  }
  
  return EXIT_SUCCESS;
}

/**
 *
 * int mode
 *    0 : master mode (master searching contents of slave)
 *    1 : slave mode (slave searching contents of master)
 *
 **/

void sync_directories(const char *master, const char *slave) {
  printf("Scanning %s...\n", master);
  scan_dir(master, NULL, NULL, slave, 0);
  printf("Scanning %s...\n", slave);
  scan_dir(slave, NULL, NULL, master, 1);
}

void scan_dir(const char* dirname, const char* context, const char* path, const char* sync_dir, int mode) {
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
        scan_dir(dirname, path, dir_meta->d_name, sync_dir, mode);
        if(path && mode && (find_dir_in_dir(sync_dir, relative_path) == 0)) {
          if(!DRY_RUN) {
            rmdir(search_path);
          }
          if(VERBOSE || DRY_RUN) {
            printf("Successfully removed directory: %s\n", search_path);
          }
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
        if((stat_success = stat(file_path, current_file_stat)) == 0) {
          if(S_ISREG(current_file_stat->st_mode) && (current_file_stat->st_mode & S_IRUSR)) {
            char* found_file;
            // If the file is in a subdirectory, construct the relative path
            // Else, just pass along the filename as it is in the base dir
            if(path) {
              char* search_file_path = (char*) malloc(sizeof(char) * MAX_DIRNAME_LEN);
              if(relative_path) {
                snprintf(search_file_path, MAX_DIRNAME_LEN, "%s/%s", relative_path, dir_meta->d_name);
              } else {
                strncpy(search_file_path, dir_meta->d_name, MAX_DIRNAME_LEN);
              }
              found_file = find_file_in_dir(sync_dir, search_file_path);
              free(search_file_path);
            } else {
              found_file = find_file_in_dir(sync_dir, dir_meta->d_name);
            }
            // If found file is null, the file isn't present in the searched dir -- do appropriate ops
            // Else, the file is there, disregard mode, overwrite older file
            if(found_file == NULL) {
              if(mode && !DRY_RUN) {
                if(remove(file_path) != 0) {
                  printf("Error deleting %s.\n", file_path);
                } else {
                  if(VERBOSE || DRY_RUN) {
                      printf("%s successfully deleted from memory.\n", file_path);
                  }
                }
              } else {
                // Master mode, copy to slave directory
                // found_file should contain the correct path to copy to.
                FILE* src_file, *dest_file;
                char* dest_file_path = (char*) malloc(sizeof(char) * MAX_DIRNAME_LEN);
                char* dest_dir_path = (char*) malloc(sizeof(char) * MAX_DIRNAME_LEN);
                // Construct correct destination path name based on directory level
                if(relative_path) {
                  snprintf(dest_dir_path, MAX_DIRNAME_LEN, "%s/%s", sync_dir, relative_path);
                  snprintf(dest_file_path, MAX_DIRNAME_LEN, "%s/%s", dest_dir_path, dir_meta->d_name);
                } else {
                  strncpy(dest_dir_path, sync_dir, MAX_DIRNAME_LEN);
                  snprintf(dest_file_path, MAX_DIRNAME_LEN, "%s/%s", dest_dir_path, dir_meta->d_name);
                }
                if((src_file = fopen(file_path, "r"))) {
                  // Check if destination dir exists
                  DIR* dest_dir;
                  if(!(dest_dir = opendir(dest_dir_path))) {
                    if(VERBOSE || DRY_RUN) {
                      printf("About to create diretory %s\n", dest_dir_path);
                    }                    
                    if(!DRY_RUN) {
                      mkdir(dest_dir_path, 0777);
                    }
                  } else {
                    closedir(dest_dir);
                  }
                  if(VERBOSE || DRY_RUN) {
                    printf("About to create %s\n", dest_file_path);
                  }
                  if((dest_file = fopen(dest_file_path, "w"))) {
                    if(!DRY_RUN && copy_file(src_file, dest_file) == 0) {
                      if(VERBOSE) {
                        printf("File: %s successfully copied.\n", file_path);
                      }
                    } else {
                      fprintf(stderr, "Error occurred while copying %s.\n", file_path);
                    }
                    if(DRY_RUN) {
                      printf("File: %s successfully copied.\n", file_path);
                    }
                    fclose(dest_file);
                  } else {
                    fprintf(stderr, "Something went wrong when creating file.\n");
                  }
                  fclose(src_file);
                } else {
                  fprintf(stderr, "Something went wrong when attempting to open %s.\n", file_path);
                }
                free(dest_file_path);
                free(dest_dir_path);
              }
            } else {
              // found_file has the path of a file. compare the current file and found_file
              struct stat* found_file_meta = (struct stat*) malloc(sizeof(struct stat));
              long original_file_time, found_file_time;
              original_file_time = current_file_stat->st_mtime;
              FILE* original_file, *dest_file;
              if((stat(found_file, found_file_meta)) == 0) {
                found_file_time = found_file_meta->st_mtime;
                if(original_file_time > found_file_time) {
                  if((original_file = fopen(file_path, "r"))) {
                    if((dest_file = fopen(found_file, "w+"))) {
                      if(!DRY_RUN) {
                        copy_file(original_file, dest_file);
                      }
                      if(VERBOSE || DRY_RUN) {
                        printf("Moved more recent %s to %s.\n", file_path, found_file);
                      }
                      fclose(original_file);
                      fclose(dest_file);
                    } else {
                      fprintf(stderr, "Error opening %s.\n", found_file);
                    }
                  } else {
                    fprintf(stderr, "Error opening %s.\n", file_path);
                  }
                }
                else if(found_file_time < original_file_time) {
                  if((original_file = fopen(found_file, "r"))) {
                    if((dest_file = fopen(file_path, "w+"))) {
                      if(!DRY_RUN) {
                        copy_file(original_file, dest_file);
                      }
                      if(VERBOSE || DRY_RUN) {
                        printf("Copied %s to %s.\n", found_file, file_path);
                      }
                      fclose(original_file);
                      fclose(dest_file);
                    } else {
                      fprintf(stderr, "Error opening %s.\n", found_file);
                    }
                  } else {
                    fprintf(stderr, "Error opening %s.\n", found_file);
                  }
                }
                free(found_file_meta);
              } else {
                fprintf(stderr, "Error reading file info: %s", file_path);
              }
            }
            free(found_file);
          }
        } else {
          fprintf(stderr, "%s\n", strerror(errno));
        }
        free(file_path);
      }
    }
    free(current_file_stat);
    free(dir_meta);
    closedir(dir);
  }
  free(search_path);
  free(relative_path);
}

char* find_file_in_dir(const char* dir_to_search, const char* file_path) {
  struct stat* file_stat = (struct stat *) malloc(sizeof(struct stat));
  int stat_success;
  char* search_rel_path = (char*) Malloc(sizeof(char) * MAX_DIRNAME_LEN);
  if(dir_to_search) {
    snprintf(search_rel_path, MAX_DIRNAME_LEN, "%s/%s", dir_to_search, file_path);
  } else {
    snprintf(search_rel_path, MAX_DIRNAME_LEN, "./%s", file_path);
  }
  stat_success = stat(search_rel_path, file_stat);
  if(stat_success == 0) {
    if(S_ISREG(file_stat->st_mode) && (file_stat->st_mode & S_IRUSR)) {
      //File is readable, return name!
      free(file_stat);
      return search_rel_path;
    }
  }
  free(search_rel_path);
  free(file_stat);
  return NULL;
}

// Find a directory in a directory
int find_dir_in_dir(const char* dir_to_search, const char* dir_path) {
  int ret_value = 0;
  if(dir_path) {
    DIR* dir_to_find;
    char* path = (char*) malloc(sizeof(char) * MAX_DIRNAME_LEN);
    if(dir_to_search) {
      snprintf(path, MAX_DIRNAME_LEN, "%s/%s", dir_to_search, dir_path);
    } else {
      snprintf(path, MAX_DIRNAME_LEN, "./%s", dir_path);
    }
    if((dir_to_find = opendir(path))) {
      ret_value = 1;
      closedir(dir_to_find);
    } else {
      ret_value = 0;
    }
    free(path);
  } else {
    ret_value = 0;
  }
  return ret_value;
}

int copy_file(FILE* a, FILE* b) {
  int ret_value;
  char* buf = (char*) malloc(sizeof(char) * 1024);
  if(a != NULL && b != NULL) {
    long n;
    while(feof(a) == 0) {
      n = fread(buf, 1024, 1, a);
      if(n < 1 && ferror(a)) {
        puts("read error");
        ret_value = 1;
        break;
      } else {
        n = fwrite(buf, 1024, 1, b);
        if(n < 1 && ferror(b)) {
          puts("write error");
          ret_value = 1;
          break;
        }
      }
    }
    ret_value = 0;
  } else {
    ret_value = 1;
  }
  free(buf);
  return ret_value;
}