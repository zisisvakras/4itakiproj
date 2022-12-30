#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "dict.h"

extern int errno;

Dictnode* make_dict(char* dictionary_path, int max_word_size) {   

    FILE* dictionary_file = fopen(dictionary_path, "r");

    if (dictionary_file == NULL) { /* File error handling */
        fprintf(stderr, "Error while handling dictionary: %s", strerror(errno));
        return NULL;
    }

    /* Allocate enough linked lists for all needed word sizes */
    Dictnode* dictionary = malloc(max_word_size * sizeof(Dictnode));
    if (dictionary == NULL) { /* Malloc error handling */
        fprintf(stderr, "Error while allocating memory: %s", strerror(errno));
        return NULL;
    }
    for (int i = 0 ; i < max_word_size ; i++) {
        dictionary[i] = malloc(sizeof(struct Dictionary));
        if (dictionary[i] == NULL) { /* Malloc error handling */
            fprintf(stderr, "Error while allocating memory: %s", strerror(errno));
            return NULL;
        }
    }

    /* Array to hold pointers at the end of the list */
    Dictnode* dictionary_end = malloc(max_word_size * sizeof(Dictnode));
    if (dictionary_end == NULL) { /* Malloc error handling */
        fprintf(stderr, "Error while allocating memory: %s", strerror(errno));
        return NULL;
    }
    for (int i = 0 ; i < max_word_size ; i++) {
        dictionary_end[i] = dictionary[i];
    }

    char* buffer = malloc(81 * sizeof(char)); /* Hopefully no word will be larger than 80 chars long */
    if (buffer == NULL) { /* Malloc error handling */
        fprintf(stderr, "Error while allocating memory: %s", strerror(errno));
        return NULL;
    }
    while (fscanf(dictionary_file, "%80s", buffer) == 1) { /* Scan 1 word at a time */
        int word_size = strlen(buffer);
        if (word_size > max_word_size) continue; /* No need to allocate larger words than needed */
        Dictnode node = dictionary_end[word_size - 1]; /* Copy current node */
        node->word = malloc((word_size + 1) * sizeof(char)); /* Allocate memory for word */
        if (node->word == NULL) { /* Malloc error handling */
            fprintf(stderr, "Error while allocating memory: %s", strerror(errno));
            return NULL;
        }
        strcpy(node->word, buffer); /* Copy word in buffer to node */
        node->next = malloc(sizeof(struct Dictionary)); /* Allocate next node */
        if (node->next == NULL) { /* Malloc error handling */
            fprintf(stderr, "Error while allocating memory: %s", strerror(errno));
            return NULL;
        }
        dictionary_end[word_size - 1] = node->next; /* Change end node to the next one */
    }
    
    fclose(dictionary_file);
    free(dictionary_end); /* Free excess memory */
    free(buffer);
    return dictionary;
}

void print_dict(Dictnode* dictionary, int max_word_size) {
    for (int i = 0 ; i < max_word_size ; i++) {
        printf("words with size: %d\n\n", i + 1);
        Dictnode temp = dictionary[i];
        do {
            if (temp->word != NULL)
                printf("%s\n", temp->word);
        } while ((temp = temp->next) != NULL);
    }
}

void free_dict(Dictnode* dictionary, int max_word_size) {
    for (int i = 0 ; i < max_word_size ; i++) {
        Dictnode prev = dictionary[i];
        Dictnode new;
        while ((new = prev->next) != NULL) {
            free(prev);
            prev = new;
        }
        free(prev);
        free(new);
    }
}
