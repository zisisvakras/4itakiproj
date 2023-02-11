#ifndef EXTRAFUNS_H_
#define EXTRAFUNS_H_
#include "extratypes.h"

/* Generic error thrower with errno */
#define error(x, e) ({ \
    perror((x));       \
    exit((e));         \
})

/* Malloc error handling */
#define mallerr(x, e) ({                                \
    if ((x) == NULL)                                    \
        error("Error while allocating memory", (e));    \
})

/* Debugging tools */
#ifdef DEBUG
#include <assert.h>
#define DBGCHECK(X) assert(X)
#else
#define DBGCHECK(X) 
#endif

/* dict.c functions */
Dictionary* init_dictionary(char* dictionary_path, int max_word_size, int** words_count_ret, int** multi);
void free_dictionary(Dictionary* bigdict, int max_word_size, int* words_count);
char* find_word(Dictionary dictionary, Word* word);
int word_val(char* word, int** multi);
void sort_dictionary(Dictionary dictionary, int* dictnode_values, int first, int last);

/* words.c functions */
void write_word(char** crossword, Word* node, char* word);
int count_words_on_grid(char** crossword, int crossword_size);
Word** map_words_on_grid(char** crossword, int crossword_size, int count, int** multi);
void prop_word(Word** words, int wordnode_count, int last);
void print_solution(char** crossword, int crossword_size);

/* maps.c functions */
Map*** init_dict_maps(Dictionary* bigdict, int max_word_size, int* words_count);
void update_map(char** crossword, Word* word, Map*** maps);
void join_map(Map* map1, Map* map2);
int sum_bit(Map* map);

/* debug.c funcs */
void print_words(Word** words, int wordnode_count, char** crossword);
void print_dictionary(Dictionary* bigdict, int max_word_size, int* words_count);
void print_map(Map* map);

#endif