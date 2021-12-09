/* File: indexPage.c */
/* Author: Britton Wolfe */
/* Date: September 3rd, 2010 */

/* This program indexes a web page, printing out the counts of words on that page */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* TODO: structure definitions */
struct trienode {
    char ch;    //Store specific char
    int count;      //Store times word appears
    struct trienode* children[26];    //26 children for each letter
};

/* NOTE: int return values can be used to indicate errors (typically non-zero)
   or success (typically zero return value) */

struct trienode* initializeTrieNode();

/* TODO: change this return type */
struct trienode* indexPage(const char* url);

int addWordOccurrence(const char* word, const int wordLength, struct trienode* root);

void printTrieContents(struct trienode* root, char* buffer_beginning, char* buffer_end);

int freeTrieMemory(struct trienode* root);

int getText(const char* srcAddr, char* buffer, const int bufSize);

void printTrieContentsHelper(struct trienode* current, char* buffer_beginning, char* buffer_end);

int main(int argc, char** argv){
  /* TODO: write the (simple) main function

  /* argv[1] will be the URL to index, if argc > 1 */
  if (argc > 1) {
    struct trienode* root = indexPage(argv[1]);
    char buffer[300001];
    printTrieContents(root, buffer, buffer);    //Print each word and its count
    int x = freeTrieMemory(root);   //Free all heap memory used
  }
  return 0;
}

/* TODO: define the functions corresponding to the above prototypes */

// Helper function to malloc for each trie node
struct trienode* initializeTrieNode() {
    struct trienode* root = malloc(sizeof(struct trienode));
    for (int i = 0; i < 26; ++i) {
        root->children[i] = NULL;   //Avoid random value given
    }
    root->ch = ' ';   //Set as whitespace to avoid error
    root->count = 0;  //Starts at 0, not some random variable

    return root;    //Return root to access later
}

/* TODO: change this return type */
struct trienode* indexPage(const char* url) {
  char buffer[300001];
  int bufferlen = getText(url, buffer, 300001);   //Get first 300,000 elements plus 1 for null terminating character
  printf("%s", url);    //Print out url

  struct trienode* root = initializeTrieNode();   //Create/initialize trienode

  int i = 0;    //Loop through buffer array to add words
  while (i < bufferlen) {
    int j = i;
    for (; j < bufferlen; ++j) {
      //If char is not a letter, break
      if (!(((buffer[j] >= 97) && (buffer[j] < 123)) || ((buffer[j] >= 65) && (buffer[j] < 91)))) {
        break;
      }
    }
    //If j != i, we found a word
    if (j != i) {
      i += addWordOccurrence(buffer + i, j - i, root);
    } else {
      ++i;
    }
  }
  putchar('\n');    //Improve formatting

  return root;
}

//Added constraint to accept trienode*
int addWordOccurrence(const char* word, const int wordLength, struct trienode* root) {
  printf("\n\t");   //Formatting

  struct trienode* current = root;  //Temp trienode*
  int i = 0;
  //Loop through word given by indexPage
  for (; i < wordLength; ++i) {
    char toLower = (word[i] >= 97)? word[i] : word[i] + 32;

    //If node doesn't exist, make it
    if (current->children[toLower - 97] == NULL) {
      current->children[toLower - 97] = initializeTrieNode();
    }

    current = current->children[toLower - 97];
    current->ch = toLower;    //Convert character to lowercase
    putchar(current->ch);   //Print current char
  }
  current->count++;   //Increment count of current word
  return i;   //Return word length
}

void printTrieContents(struct trienode* root, char* buffer_beginning, char* buffer_end) {
  if (root != NULL) {
    for (int i = 0; i < 26; ++i) {    //Print for each starting letter
      printTrieContentsHelper(root->children[i], buffer_beginning, buffer_end);
    }
  }
}

//Helper function
void printTrieContentsHelper(struct trienode* current, char* buffer_beginning, char* buffer_end) {
  if (current != NULL) {
    *buffer_end = current->ch;
    if (current->count != 0) {    //At a word, so print
      *(buffer_end + 1) = '\0';   //Add null terminating string at end of word
      printf("%s: %d\n", buffer_beginning, current->count);   //Print word and its count
    }
    for (int i = 0; i < 26; ++i) {    //Recursively print a char and each char following
      printTrieContentsHelper(current->children[i], buffer_beginning, buffer_end + 1);
    }
  }
}

int freeTrieMemory(struct trienode* root) {
  if (root != NULL) {
    for (int i = 0; i < 26; ++i) {
      int x = freeTrieMemory(root->children[i]);
    }
    free(root);
  }
  return 0;
}

/* You should not need to modify this function */
int getText(const char* srcAddr, char* buffer, const int bufSize){
  FILE *pipe;
  int bytesRead;

  snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getText.py", srcAddr);

  pipe = popen(buffer, "r");
  if(pipe == NULL){
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n",
	    buffer);
    return 0;
  }

  bytesRead = fread(buffer, sizeof(char), bufSize-1, pipe);
  buffer[bytesRead] = '\0';

  pclose(pipe);

  return bytesRead;
}
