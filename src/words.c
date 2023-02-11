#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "extratypes.h"
#include "extrafuns.h"

extern int errno;

void write_word(char** crossword, Word* node, char* word) {
    if (node->orientation) { /* Vertical */
        for (int i = node->begin, j = 0 ; i <= node->end ; i++, j++) {
            crossword[i][node->constant] = word[j];
        }
    }
    else { /* Horizontal */
        for (int i = node->begin, j = 0 ; i <= node->end ; i++, j++) {
            crossword[node->constant][i] = word[j];
        }
    }
}

//TODO add a second words array thats in the correct order for print and -check
void print_solution(char** crossword, int crossword_size) {
    int flag = 0;
    for (int i = 0 ; i < crossword_size ; i++) {
        for (int j = 0 ; j < crossword_size ; j++) {
            if (crossword[i][j] != '#') {
                if (!flag) {
                    if (j == crossword_size - 1) continue;
                    if (crossword[i][j + 1] != '#') flag = 1;
                    else continue;
                }
                putchar(crossword[i][j]);
            }
            if (crossword[i][j] == '#') {
                if (flag) putchar('\n');
                flag = 0;
            }
        }
        if (flag) putchar('\n');
        flag = 0;
    }
    for (int i = 0 ; i < crossword_size ; i++) {
        for (int j = 0 ; j < crossword_size ; j++) {
            if (crossword[j][i] != '#') {
                if (!flag) {
                    if (j == crossword_size - 1) continue;
                    if (crossword[j + 1][i] != '#') flag = 1;
                    else continue;
                }
                putchar(crossword[j][i]);
            }
            if (crossword[j][i] == '#') {
                if (flag) putchar('\n');
                flag = 0;
            }
        }
        if (flag) putchar('\n');
        flag = 0;
    }
}

/* Count words on the crossword */
int count_words_on_grid(char** crossword, int crossword_size) {
    int hor_size = 0, ver_size = 0, count = 0;
    for (int i = 0 ; i < crossword_size ; i++) {
        for (int j = 0 ; j < crossword_size ; j++) {
            /* Horizontal */
            if (crossword[i][j] != '#') hor_size++;
            if (crossword[i][j] == '#') {
                if (hor_size > 1) count++; /* Words with size 1 don't count */
                hor_size = 0;
            }
            /* Vertical */
            if (crossword[j][i] != '#') ver_size++;
            if (crossword[j][i] == '#') {
                if (ver_size > 1) count++;
                ver_size = 0;
            }
        }
        /* We need to check if some words reach the border without finding # */
        if (hor_size > 1) count++;
        if (ver_size > 1) count++;
        hor_size = 0;
        ver_size = 0;
    }
    return count;
}

Word** map_words_on_grid(char** crossword, int crossword_size, int count, int** multi) {
    /* Initializing grid_words */
    Word** grid_words = malloc(count * sizeof(Word*));
    mallerr(grid_words, errno);
    for (int i = 0 ; i < count ; ++i) {
        grid_words[i] = calloc(1, sizeof(Word)); /* Setting most default values to 0 */
        mallerr(grid_words[i], errno);
    }

    /* Mapping the grid */
    int hor_size = 0, ver_size = 0;
    int begin_h = 0, begin_v = 0, index = 0;
    for (int i = 0 ; i < crossword_size ; i++) {
        for (int j = 0 ; j < crossword_size ; j++) {
            if (crossword[i][j] != '#') {
                if (hor_size == 0) begin_h = j;
                hor_size++;
            }
            if (crossword[i][j] == '#') {
                if (hor_size > 1) {
                    *grid_words[index++] = (Word) {
                        .orientation = 0,
                        .constant = i,
                        .begin = begin_h,
                        .end = j - 1,
                        .size = j - begin_h
                    };
                }
                hor_size = 0;
            }
            if (crossword[j][i] != '#') {
                if (ver_size == 0) begin_v = j;
                ver_size++;
            }
            if (crossword[j][i] == '#') {
                if (ver_size > 1) {
                    *grid_words[index++] = (Word) {
                        .orientation = 1,
                        .constant = i,
                        .begin = begin_v,
                        .end = j - 1,
                        .size = j - begin_v
                    };
                }
                ver_size = 0;
            }
        }
        if (hor_size > 1) {
            *grid_words[index++] = (Word) {
                .orientation = 0,
                .constant = i,
                .begin = begin_h,
                .end = crossword_size - 1,
                .size = crossword_size - begin_h
            };
        }
        if (ver_size > 1) {
            *grid_words[index++] = (Word) {
                .orientation = 1,
                .constant = i,
                .begin = begin_v,
                .end = crossword_size - 1,
                .size = crossword_size - begin_v
            };
        }
        hor_size = 0;
        ver_size = 0;
    }

    /* Finding the intersections */
    Intersection* buf_insecs = malloc(count * sizeof(Intersection));
    mallerr(buf_insecs, errno);
    int buf_insecc = 0;
    for (int i = 0 ; i < count ; ++i) { //TODO optimize
        /* Reseting buf_insecs */
        memset(buf_insecs, 0, count * sizeof(Intersection));
        buf_insecc = 0;
        if (grid_words[i]->orientation) {
            for (int j = grid_words[i]->begin ; j <= grid_words[i]->end ; ++j) {
                int found = 0;
                if (grid_words[i]->constant != 0) {
                    char ch = crossword[j][grid_words[i]->constant - 1];
                    if (ch != '#') {
                        for (int k = 0 ; k < count ; ++k) {
                            if (grid_words[k]->orientation) continue;
                            int cord = grid_words[i]->constant - 1;
                            if (grid_words[k]->constant == j && grid_words[k]->begin <= cord && cord <= grid_words[k]->end) {
                                buf_insecs[buf_insecc++] = (Intersection) {
                                    .word = &(*grid_words[k]),
                                    .x = j,
                                    .y = grid_words[i]->constant,
                                    .pos = grid_words[i]->constant - grid_words[k]->begin
                                };
                                ++multi[grid_words[i]->size - 1][j - grid_words[i]->begin];
                                found = 1;
                                break;
                            }
                        }
                    }
                }
                if (!found && grid_words[i]->constant != crossword_size - 1) {
                    char ch = crossword[j][grid_words[i]->constant + 1];
                    if (ch != '#') {
                        for (int k = 0 ; k < count ; ++k) {
                            if (grid_words[k]->orientation) continue;
                            int cord = grid_words[i]->constant + 1;
                            if (grid_words[k]->constant == j && grid_words[k]->begin <= cord && cord <= grid_words[k]->end) {
                                buf_insecs[buf_insecc++] = (Intersection) {
                                    .word = &(*grid_words[k]),
                                    .x = j,
                                    .y = grid_words[i]->constant,
                                    .pos = grid_words[i]->constant - grid_words[k]->begin
                                };
                                ++multi[grid_words[i]->size - 1][j - grid_words[i]->begin];
                                break;
                            }
                        }
                    }
                }
            }
        }
        else {
            for (int j = grid_words[i]->begin ; j <= grid_words[i]->end ; ++j) {
                int found = 0;
                if (grid_words[i]->constant != 0) {
                    char ch = crossword[grid_words[i]->constant - 1][j];
                    if (ch != '#') {
                        for (int k = 0 ; k < count ; ++k) {
                            if (!grid_words[k]->orientation) continue;
                            int cord = grid_words[i]->constant - 1;
                            if (grid_words[k]->constant == j && grid_words[k]->begin <= cord && cord <= grid_words[k]->end) {
                                buf_insecs[buf_insecc++] = (Intersection) {
                                    .word = &(*grid_words[k]),
                                    .x = grid_words[i]->constant,
                                    .y = j,
                                    .pos = grid_words[i]->constant - grid_words[k]->begin
                                };
                                ++multi[grid_words[i]->size - 1][j - grid_words[i]->begin];
                                found = 1;
                                break;
                            }
                        }
                    }
                }
                if (!found && grid_words[i]->constant != crossword_size - 1) {
                    char ch = crossword[grid_words[i]->constant + 1][j];
                    if (ch != '#') {
                        // Check which word has letter
                        for (int k = 0 ; k < count ; ++k) {
                            if (!grid_words[k]->orientation) continue;
                            int cord = grid_words[i]->constant + 1;
                            if (grid_words[k]->constant == j && grid_words[k]->begin <= cord && cord <= grid_words[k]->end) {
                               buf_insecs[buf_insecc++] = (Intersection) {
                                    .word = &(*grid_words[k]),
                                    .x = grid_words[i]->constant,
                                    .y = j,
                                    .pos = grid_words[i]->constant - grid_words[k]->begin
                                };
                                ++multi[grid_words[i]->size - 1][j - grid_words[i]->begin];
                                break;
                            }
                        }
                    }
                }
            }
        }
        grid_words[i]->insecs = malloc((buf_insecc + 1) * sizeof(Intersection));
        memcpy(grid_words[i]->insecs, buf_insecs, (buf_insecc + 1) * sizeof(Intersection));
    }
    free(buf_insecs);
    return grid_words;
}

void prop_word(Word** words, int wordnode_count, int last) { //2nd criteria insecs
    int index = last;
    int min = words[index]->map->sum;
    for (int i = index + 1 ; i < wordnode_count ; ++i) {
        int temp = words[i]->map->sum;
        if (temp < min) {
            min = temp;
            index = i;
        }
    }
    Word* temp = words[last];
    words[last] = words[index];
    words[index] = temp;
}