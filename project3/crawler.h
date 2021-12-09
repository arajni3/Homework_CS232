#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include "indexPage.h"

/*
 * a node in our linked-list of web addresses
 */
struct listNode{
  // MAX_ADDR_LENGTH == 1000
  char addr[1000];
  int numTerms;
  struct listNode *next;
  //struct trienode *index_trie;
};

/*
 * returns 1 if the list starting at pNode contains the address addr,
 *    and returns 0 otherwise
 */
int contains(const struct listNode *pNode, const char *addr);

/*
 * inserts the address addr as a new listNode at the end of
 *    the list
 */
void insertBack(struct listNode *pNode, const char *addr);

/*
 * prints the addresses from pNode to the end of the list,
 *   one on each line
 */
void printAddresses(const struct listNode *pNode);

/*
 * frees the memory associated with this node and all subsequent nodes
 */
void destroyList(struct listNode *pNode);
  
/*
 * srcAddr should be a web address (e.g., http://www.yahoo.com).
 * link should point to an array of maxLinkLength characters.
 * getLink returns 1 if a link was found and 0 otherwise.
 * If a link was found, "link" will be filled in with the web address.
 */
int getLink(const char* srcAddr, char* link, const int maxLinkLength);

struct listNode* crawl(char* start_url, int numHops, int *n, int MAX_N);