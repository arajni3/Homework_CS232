#include <math.h>
#include "crawler.h"
#include "indexPage.h"
/* 
 * main gets three command line arguments:
 * The first one is the name of the file containing the URLs to index.
 * The second one is the maximum number of web pages to index (not the number of start URLs in the file).
 * The third one is a long int that should be passed to srand before beginning the web crawling. Use atol
 * to convert a string to a long.
 */

int main(int argc, char* argv[]) {
    srand(atol(argv[3]));

    FILE* pipe;
    pipe = fopen(argv[1], "r");

    char *url;
    url = malloc(sizeof(char) * 1000);
    int hopsLimit = 0, MAX_N = atoi(argv[2]), n = 0;
    struct listNode *urls = NULL;
    // crawl and index
    while ((fscanf(pipe, "%s %d", url, &hopsLimit) != EOF) && (n < MAX_N)) {
        if ((hopsLimit == 0) || (MAX_N == 0)) continue;
        urls = crawl(url, hopsLimit, &n, MAX_N);

        // get number of webpages indexed by getting length of linked list, and also index while looping
        int num_indexed = 0;
        for (struct listNode *current = urls; current != NULL; current = current->next) {
            ++num_indexed;
        }
        // store the tries of the webpages and index the webpages
        printf("Indexing...\n");
        struct trienode* tries[num_indexed];
        int q = -1;
        for (struct listNode *current = urls; current != NULL; current = current->next) {
            tries[++q] = indexPage(current->addr, &(current->numTerms));
        }

        // answer queries and compute scores
        // we are now reusing url to be a general buffer for the query; url is no longer an actual url string
        free(url);
        url = malloc(sizeof(char) * 1000);
        for (int r = 0; r < 1000; ++r) {
            url[r] = '\0';
        }
        printf("\nEnter a web query: ");
        fgets(url, 1000, stdin);
        //putchar('\n');
        // user query is empty iff. url[0] = '\n' after fgets
        while (url[0] != '\n') {
            // for (int i = 0; i < 1000; ++i) {
            //     putchar(url[i]);
            // }
            // putchar('\n');
            int isgood = 1;
            for (int i = 0; (i < 1000) && isgood; ++i) {
                // print error message if user did not input all lowercase terms separated by whitespace
                if ((url[i] != 32) && (!((url[i] >= 97) && (url[i] < 123)))) {
                    if (url[i] == 0) {
                        break;
                    } else if (url[i] == '\n') {
                        url[i] = '\0';
                        continue;
                    } else {
                        printf("Please enter a query containing only lower-case letters.\n");
                        isgood = 0;
                    }
                }
            }
            if (isgood) {
                putchar('\n');
                printf("Query is %c%s%c.\n", '"', url, '"');
                printf("IDF scores are\n");

                int start = 0, afterend = 0;
                double first_place = -1.0000000, second_place = -1.0000000, third_place = -1.0000000;
                char first_url[1000];
                char second_url[1000];
                char third_url[1000];
                for (int r = 0; r < 1000; ++r) {
                    first_url[r] = '\0';
                    second_url[r] = '\0';
                    third_url[r] = '\0';
                }
                double score_d[num_indexed];

                for (int r = 0; r < num_indexed; ++r) {
                    score_d[r] = 0;
                }
                while (url[start] != 0) {
                    while ((url[afterend] != 32) && (url[afterend] != 0)) {
                        ++afterend;
                    }
                    url[afterend] = '\0';
                    // calculate score of each url for current word by finding tf scores and then idf score
                    int i = -1, num_docs_contains = 0;
                    double tf_d_t[num_indexed];
                    for (struct listNode *current = urls; current != NULL; current = current->next) {
                        tf_d_t[++i] = 0;
                        // find count of current word in current web address
                        struct trienode *root = tries[i];
                        for (int index = start; (index < afterend) && (root != NULL); ++index) {
                            root = root->children[url[index] - 97];
                        }
                        if ((root != NULL) && (root->count > 0)) {
                            // add tf(d, t) to score of d
                            tf_d_t[i] = ((double)(root->count)) / ((double)(current->numTerms));
                            ++num_docs_contains;
                        }
                    }
                    // calculate idf score of current word and then finish computing tf-idf score
                    double idf_t = log((1.0 + (double)num_indexed) / (1.0 + (double)num_docs_contains));
                    // print score of current word with correct formatting
                    printf("IDF(%s): %.7f\n", url + start, idf_t);

                    i = -1;
                    for (struct listNode *current = urls; current != NULL; current = current->next) {
                        // finish computing tf score
                        tf_d_t[++i] *= idf_t;
                        // add score for each webpage for the current word
                        score_d[i] += tf_d_t[i];
                    }
                    // move delimiters to progress
                    start = ++afterend;
                }
                // calculate the top (up to three) highest webpage scores
                int i = -1;
                for (struct listNode *current = urls; current != NULL; current = current->next) {
                    ++i;
                    if (num_indexed >= 3) {
                        if (score_d[i] > first_place) {
                            third_place = second_place;
                            second_place = first_place;
                            first_place = score_d[i];

                            strncpy(third_url, second_url, 1000);
                            strncpy(second_url, first_url, 1000);
                            strncpy(first_url, current->addr, 1000);
                        } else if (score_d[i] > second_place) {
                            third_place = second_place;
                            second_place = score_d[i];

                            strncpy(third_url, second_url, 1000);
                            strncpy(second_url, current->addr, 1000);
                        } else if (score_d[i] > third_place) {
                            third_place = score_d[i];

                            strncpy(third_url, current->addr, 1000);
                        }
                    } else if (num_indexed == 2) {
                        if (score_d[i] > first_place) {
                            second_place = first_place;
                            first_place = score_d[i];
                    
                            strncpy(second_url, first_url, 1000);
                            strncpy(first_url, current->addr, 1000);
                        } else if (score_d[i] > second_place) {
                            second_place = score_d[i];

                            strncpy(second_url, current->addr, 1000);
                        }
                    } else if (num_indexed == 1) {
                        if (score_d[i] > first_place) {
                            first_place = score_d[i];

                            strncpy(first_url, current->addr, 1000);
                        }
                    }
                    score_d[i] = 0;
                }
                // print the top (up to three) webpages with the highest scores
                printf("Web pages: \n");
                if (num_indexed >= 3) {
                    printf("1. %s (score: %.7f)\n", first_url, first_place);
                    printf("2. %s (score: %.7f)\n", second_url, second_place);
                    printf("3. %s (score: %7f)\n", third_url, third_place);
                } else if (num_indexed == 2) {
                    printf("1. %s (score: %.7f)\n", first_url, first_place);
                    printf("2. %s (score: %.7f)\n", second_url, second_place);
                } else if (num_indexed == 1) {
                    printf("1. %s (score: %.7f)\n", first_url, first_place);
                }
                first_place = -1.0000000;
                second_place = -1.0000000;
                third_place = -1.0000000;
            } 
            printf("\nEnter a web query: ");
            fgets(url, 1000, stdin);
            //putchar('\n');
        }
        printf("Exiting the program\n");

        // free all trie heap memory
        for (int i = 0; i < num_indexed; ++i) {
            freeTrieMemory(tries[i]);
        }
        // free all linked list heap memory
        destroyList(urls);
    }
    
    // free url heap memory, close pipe, and we're done
    free(url);
    fclose(pipe);
    return 0;
};