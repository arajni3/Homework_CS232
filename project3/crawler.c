#include "crawler.h"

#define MAX_ADDR_LENGTH 1000

struct listNode* crawl(char* start_url, int numHops, int *n, int MAX_N){
  char startAddr[MAX_ADDR_LENGTH];
  char destAddr[MAX_ADDR_LENGTH];
  
  int hopNum;
  
  struct listNode *pListStart;

  strncpy(startAddr, start_url, MAX_ADDR_LENGTH);
  startAddr[MAX_ADDR_LENGTH - 1] = '\0';

  pListStart = malloc(sizeof(struct listNode));
  if(pListStart == NULL){
    fprintf(stderr, "ERROR: could not allocate memory\n");
    return NULL;
  }
  strncpy(pListStart->addr, startAddr, MAX_ADDR_LENGTH);
  pListStart->next = NULL;
  pListStart->numTerms = 0;
  
  ++(*n);

  /* start the crawling */
  for(hopNum=1; hopNum <= numHops; hopNum++){
    int res = getLink(startAddr, destAddr, MAX_ADDR_LENGTH);

    if(!res){
      //printf("Dead end on hop %d: no outgoing links\n", hopNum);
      break;
    }

    if(contains(pListStart, destAddr)){
      //printf("Cycle detected on hop %d: address %s\n", hopNum, destAddr);
      hopNum--; // try again for this hop in the next iteration
    } else if (*n < MAX_N) {
      insertBack(pListStart, destAddr);
      //indexPage(destAddr);
      strncpy(startAddr, destAddr, MAX_ADDR_LENGTH);
      ++(*n);
    } else {
      break;
    }
  }
  return pListStart;
}


/*
 * returns 1 if the list starting at pNode contains the address addr,
 *    and returns 0 otherwise
 */
int contains(const struct listNode *pNode, const char *addr){
  // TODO: complete this
  for (const struct listNode *current = pNode; current != NULL; current = current->next) {
    // check if the current address current->addr is equal to the address addr; initialize equal variable to 1
    int equal = 1;

    for (int i = 0; (i < MAX_ADDR_LENGTH) && equal; ++i) {
      // check if equality is violated
      if (current->addr[i] != addr[i]) equal = 0;
    }

    // if equality is not violated, then return true
    if (equal) return 1;
  }

  // if reached here, then equality is violated, so return false
  return 0;
}
    

/*
 * inserts the address addr as a new listNode at the end of
 *    the list
 */
void insertBack(struct listNode *pNode, const char *addr){
  // TODO: complete this

  // reach second-to-last element of the list
  struct listNode *endNode = pNode;
  while (endNode->next != NULL) endNode = endNode->next;

  // allocate heap memory for a new node and initialize its next node to null and its numTerms to 0
  endNode->next = malloc(sizeof(struct listNode));
  endNode = endNode->next;
  endNode->next = NULL;
  endNode->numTerms = 0;

  // give this new node the address addr
  strncpy(endNode->addr, addr, MAX_ADDR_LENGTH);

  // index this address
  //endNode->index_trie = indexPage(addr, &endNode->numTerms);
}


/*
 * prints the addresses from pNode to the end of the list,
 *   one on each line
 */
void printAddresses(const struct listNode *pNode){
  // TODO: complete this
  for (const struct listNode *current = pNode; current != NULL; current = current->next) {
    // print current address current->addr and then print a newline
    printf(current->addr);
    putchar('\n');
  }
}

/*
 * frees the memory associated with this node and all subsequent nodes
 */
void destroyList(struct listNode *pNode){
  // TODO: complete this
  while (pNode != NULL) {
    
    // save the next node in temporary variable nextNode
    struct listNode* nextNode = pNode->next;

    // free up the portion of heap memory to which pNode is pointing
    //freeTrieMemory(pNode->index_trie);
    free(pNode);

    // now continue looping by setting pNode equal to the next node
    pNode = nextNode;
  }
}
  
/*
 * srcAddr should be a web address (e.g., http://www.yahoo.com).
 * link should point to an array of maxLinkLength characters.
 * getLink returns 1 if a link was found and 0 otherwise.
 * If a link was found, "link" will be filled in with the web address.
 */

int getLink(const char* srcAddr, char* link, const int maxLinkLength){
  const int bufSize = 1000;
  char buffer[bufSize];

  int numLinks = 0;

  FILE *pipe;

  snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getLinks.py", srcAddr);

  pipe = popen(buffer, "r");
  if(pipe == NULL){
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n", buffer);
    return 0;
  }

  fscanf(pipe, "%d\n", &numLinks);

  if(numLinks > 0){
    int linkNum;
    double r = (double)rand() / ((double)RAND_MAX + 1.0);

    for(linkNum=0; linkNum<numLinks; linkNum++){
      fgets(buffer, bufSize, pipe);
      
      if(r < (linkNum + 1.0) / numLinks){
		    break;
      }
    }

    /* copy the address from buffer to link */
    strncpy(link, buffer, maxLinkLength);
    link[maxLinkLength-1] = '\0';
    
    /* get rid of the newline */
    {
      char* pNewline = strchr(link, '\n');
      if(pNewline != NULL){
		    *pNewline = '\0';
      }
    }
  }

  pclose(pipe);

  if(numLinks > 0){
    return 1;
  }
  else{
    return 0;
  }
}
