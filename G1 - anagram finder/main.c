#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const unsigned int WORD_SIZE = 128;
const unsigned int BUCKET_COUNT = 255;
typedef struct Bucket Bucket;
typedef struct HashTable HashTable;

struct Bucket {
  char* data;
  Bucket *next;
};

struct HashTable {
  int size;
  Bucket **buckets;
};

/** Hash functions **/
int hash(char* data);
void hash_alloc(HashTable **a, unsigned int size);
void hash_dealloc(HashTable *a);
void add_to_hash_array(HashTable** a, char* data);
Bucket* hash_lookup(HashTable *a, char* val);

/** String utility functions **/
void print_list(Bucket *first);
char *str_replace(char *orig, char *rep, char *with);
void string_sort(char* a);

/** Anagram functions **/
void generate_anagrams(HashTable* a, char* word, char letter, int position);
void find_all_words(HashTable* a, char* word, char letter, int position);


int main(int argc, char const *argv[])
{
  int i, place;
  char* arg_dup, letter;
  /* Allocate memory for array of hash tables */
  HashTable** hash_array = (HashTable**) malloc(sizeof(HashTable*) * WORD_SIZE);
  if(hash_array == NULL) {
    printf("Error allocating memory");
    exit(1);
  }
  
  for(i = 0; i < WORD_SIZE; i++) {
    hash_alloc(&hash_array[i], BUCKET_COUNT);
  }
  
  /* Open dictionary file. Abort if something wrong. */
  FILE* dict = fopen("/usr/share/dict/words", "r");
  if(dict == NULL) {
    printf("Something went wrong when trying to open the dictionary file!\n");
    exit(1);
  }
  
  /* Read all dicitionary words into hash table */
  char* dict_word = (char*) malloc(WORD_SIZE * sizeof(char));
  if(dict_word == NULL) {
    printf("Error allocating memory\n");
    exit(1);
  }
  
  while(fgets(dict_word, WORD_SIZE, dict) != NULL) {
    add_to_hash_array(hash_array, dict_word);
  }
  
  free(dict_word);
  fclose(dict);
  
  const char* mode = argv[1];
  /* If an argument is provided, find anagrams for it. */
  switch (argc) {
    case 3:
      /* Anagram mode */
      arg_dup = strdup(argv[2]);
      letter = '\0';
      place = -1;

      // strlen() + 1 to account for NUL character
      if(strncmp(mode, "anagram", strlen(mode) + 1) == 0) {
        printf("Finding anagrams of '%s'...\n\n", arg_dup);
        generate_anagrams(hash_array[strlen(arg_dup) + 1], arg_dup, letter, place);
      } else {
        printf("All words contained in '%s'...\n", argv[2]);
        for(i = 3; i < strlen(arg_dup) + 2; i++) {
          printf("%i letter words\n", i - 1);
          find_all_words(hash_array[i], arg_dup, letter, place);
          printf("\n\n");
        }
      }
      
      free(arg_dup);
      break;
    case 5:
      /* Crossword mode */
      arg_dup = strdup(argv[2]);
      letter = argv[3][0];
      if(letter >= 65 && letter <= 90) {
        letter += 32;
      }
      else if(letter < 65 || (letter > 90 && letter <= 96) || letter > 122) {
        printf("Invalid character\n");
        break;
      }
      place = atoi(argv[4]);
      if(place < 0 || place > strlen(arg_dup)) {
        printf("Position out of range.\n");
        place = 0;
      }
      if(strncmp(mode, "anagram", strlen(mode) + 1) == 0) {
        printf("Finding anagrams of '%s' with '%c' at position %i...\n\n", arg_dup, letter, place);
        generate_anagrams(hash_array[strlen(arg_dup) + 1], arg_dup, letter, place);
      } else {
        printf("All words contained in '%s'...\n\n", arg_dup);
        for(i = 3; i < strlen(arg_dup) + 2; i++) {
          printf("%i letter words\n", i - 1);
          find_all_words(hash_array[i], arg_dup, letter, place);
          printf("\n\n");
        }
      }
      free(arg_dup);
      break;
    case 1:
    case 2:
    case 4:
    default:
      printf("Invalid number of arguments\n");
      break;
  }
  printf("\n");
  /* Free memory */
  for(i = 0; i < WORD_SIZE; i++) {
    hash_dealloc(hash_array[i]);
  }
  free(hash_array);
  return 0;
}

/** Allocate memory for a single hash table **/
void hash_alloc(HashTable** a, unsigned int size) {
  *a = (HashTable*) malloc(sizeof(HashTable));
  (*a)->buckets = (Bucket**) malloc(sizeof(Bucket*) * size);
  if(*a == NULL || (*a)->buckets == NULL) {
    printf("Error allocating memory for hash table");
    return;
  }
  (*a)->size = size;
}

/** Deallocate memory allocated for a single hash table **/
void hash_dealloc(HashTable* a) {
  int i;
  Bucket* current, *temp;
  for(i = 0; i < a->size; i++) {
    current = a->buckets[i];
    if(current) {
      while(current != NULL) {
        temp = current;
        free(temp->data);
        current = current->next;
        free(temp);
      }
      free(current);
    }
  }
  free(a->buckets);
  free(a);
}

/**
    Compute hash value for a given word
    Hash values are computing by summing the lowercase ASCII values
      of each character in a word
    If the computed value is higher than the number of allotted buckets
      store in highest bucket
 **/
int hash(char* word) {
  int i, currentLetter, sum;
  sum = 0;
  for (i = 0; i < strlen(word); i++) {
    currentLetter = word[i];
    if(currentLetter < 65 || (currentLetter >= 91 && currentLetter <= 96) || currentLetter > 122) {
      continue;
    }
    else if(currentLetter >= 65 && currentLetter <= 90) {
      word[i] += 32;
      currentLetter += 32;
    }
    sum += (currentLetter - 96);
  }
  if(sum > BUCKET_COUNT - 1) {
    sum = BUCKET_COUNT - 1;
  }
  return sum;
}

/** Insert a word into its respective hash table **/
void add_to_hash_array(HashTable** a, char* data) {
  int index = hash(data);
  HashTable* currentTable = a[strlen(data)];
  Bucket* existingNode = hash_lookup(currentTable, data);
  if(existingNode == NULL) {
    Bucket *newNode = (Bucket*) malloc(sizeof(Bucket));
    if(newNode == NULL) {
      printf("Error allocating memory for new bucket\n");
      exit(1);
    }
    data = str_replace(data, "\n", "\0");           /* Replace any newlines with null terminators */
    if(data != NULL) {
      newNode->data = strdup(data);
      newNode->next = currentTable->buckets[index];
      currentTable->buckets[index] = newNode;
      free(data);                                   /* Free string returned from str_replace */
    }
  } else {
    return;
  }
}

/* Return reference to the holding bucket */
Bucket* hash_lookup(HashTable* a, char* val) {
  int index = hash(val);
  Bucket* list = NULL;
  list = a->buckets[index];
  if(list) {
    for(; list != NULL; list = list->next) {
      if(strcmp(val, list->data) == 0) {
        return list;
      }
    }
  }
  return NULL;
}

/* Print contents of a linked-list */
void print_list(Bucket* first) {
  Bucket* current = first;
  while(current != NULL) {
    if(current->data != NULL) {
      printf("%s\n", current->data);
    }
    current = current->next;
  }
  printf("\n");
}

/* Casts to (int) to surpress warnings about precision loss */
/* Implementation http://stackoverflow.com/questions/779875/what-is-the-function-to-replace-string-in-c */
char *str_replace(char *orig, char *rep, char *with) {
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
    return NULL;
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

/** 
   Prints out anagrams based on command-line argument
   Anagrams should have same length as provided word
   Each letter in provided word should also be present in anagram
**/
void generate_anagrams(HashTable* a, char* word, char letter, int position) {
  int index = hash(word);
  char* word_dup = strdup(word);
  char* current_word = NULL;
  string_sort(word_dup);
  Bucket* current = a->buckets[index];
  while(current != NULL) {
    if(current->data) {
      current_word = strdup(current->data);
      string_sort(current_word);
      if(strcmp(word_dup, current_word) == 0 && strcmp(word, current->data) != 0) {
        if(letter != '\0' && position >= 0) {
          if((current->data)[position] == letter) {
            printf("%s ", current->data);
          }
        } else {
          printf("%s ", current->data);
        }
      }
      free(current_word);
    }
    current = current->next;
  }
  printf("\n");
  free(word_dup);
}

/**
   Prints out all words that can be formed from a certain word
   Length of words range from [2, length(word)]
   Repeats may be present due to how the hashing treats capital letters
   TO-DO: Add support for extra string to be prepended or appended to words (scrabble solver)
**/
void find_all_words(HashTable* a, char* word, char letter, int position) {
  int i, j, matches, pos;
  char* word_dup, *current_word, *char_match;
  Bucket* current;
  for (j = 0; j < WORD_SIZE; j++) {
    current = a->buckets[j];
    if(current) {
      while(current != NULL) {
        if(current->data) {
          word_dup = strdup(word);
          current_word = strdup(current->data);
          for(i = 0, matches = 0; i < strlen(current_word); i++) {
            char_match = strchr(word_dup, current_word[i]);
            pos = (int)(char_match - word_dup);
            if(current_word[i] != '\0' && char_match != NULL) {
              memmove(&word_dup[pos], &word_dup[pos + 1], strlen(word_dup) - pos);
              matches++;
            }
          }
          if(matches == strlen(current_word)) {
            if(letter != '\0' && position >= 0) {
              if(current->data[position] == letter) {
                printf("%s ", current->data);
              }
            } else {
              printf("%s ", current->data);
            }
          }
          free(word_dup);
          free(current_word);
        }
        current = current->next;
      }
    }
  }
}

/* Sort a string by characters */
void string_sort(char* a)
{
  char tmp;
  int i, j;
  size_t length = strlen(a);
  for(i = 0; i < length - 1; i++)
  {
    for (j = i + 1; j < length; j++)
    {
      if (a[i] > a[j])
      {
        tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
      }
    }
  }
}