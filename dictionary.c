// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"

typedef struct trie
{
    char letter;
    bool validword;
    struct trie *next[27]; //a - z plus apostrophe at 27th position
}
trie;

trie *dictrie;
int sizecount;
bool freealloc(trie *n);


// Returns true if word is in dictionary else false
bool check(const char *word)
{
    //printf("%s\n",word);

    // take word[0], go to dictrie (does dictrie need to be declared outside of the load function?)
    int arrayposition = 0;
    trie *ptr = dictrie;

    for (int i = 0; i < strlen(word); i++)
    {
        // set array position as 0-25 for a-z regardless of case, and set to 26 for apostrophe
        if (isupper(word[i]))
        {
            arrayposition = word[i] - 'A';
        }
        else if (islower(word[i]))
        {
            arrayposition = word[i] - 'a';
        }
        else
        {
            arrayposition = 26;
        }

        // if trie contains a pointer that points to another node, go there. if not, this word is not in the dictionary, return false
        if (ptr -> next[arrayposition] == NULL)
        {
            return false;
        }
        else  // there is a node, go there.
        {
            ptr = ptr -> next[arrayposition];
            if (i == strlen(word) - 1 && ptr -> validword == true)
            {
                return true;
            }
        }
    }

    return false;
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    FILE *file = fopen(dictionary, "r");
    if (!file)
    {
        return false;
    }

    sizecount = 0; //variable to be used for easily get int for size()
    dictrie = malloc(sizeof(trie));
    dictrie -> validword = false;

    trie *ptr = dictrie;

    for (int i = 0; i < 27; i++)
    {
        ptr -> next[i] = NULL;
    }

    int c = fgetc(file); // load first char

    // place each letter from dictionary into letter by creating new branches each time a new letter is found
    while (c != EOF) // chose a while loop so I can take the int c and check for EOF and return true
    {
        //printf("%c\n", c);
        int arrayposition = 0;
        bool endofword = false;

        // if at the end of word, reset ptr to point at the top of the trie
        if (c == '\n')
        {
            ptr -> validword = true; //word formed from letters of trie branch up to this node is a valid word
            ptr = dictrie; //point to top of trie
            endofword = true; // skip rest of for loop, add 1 to position to go to the first letter of new word
            sizecount++;
        }
        else if (isalpha(c))
        {
            arrayposition = c - 'a'; // therefore if letter 'a', position 0, and so forth
        }
        else if (c == '\'') // if char is apostrophe, last pointer in array
        {
            arrayposition = 26; // apostrophe goes in the last position of array of pointers
        }
        else // we must be starting or in the middle of a dictionary word
        {
            endofword = false;
        }

        // if next array points to null, new branch of trie forms. if not, follow that next
        if (ptr -> next[arrayposition] == NULL && endofword == false)
        {
            trie *n = malloc(sizeof(trie));
            if (!n) // check for enough space to create new branch
            {
                return 1;
            }

            // initialize new node to have letter = current char, validword to be false, and each pointer to NULL
            ptr -> next[arrayposition] = n;
            n -> letter = c;
            n -> validword = false; // Will switch to true if end of word is found at this node
                                    // (if at the end now, next time around the for loop carrying '\n', sets to true)
            ptr = n;

            for (int i = 0; i < 27; i++)
            {
                ptr -> next[i] = NULL;
            }

        }
        // if a node already exists on the next tier of our trie, go to that lower level
        else if (ptr -> next[arrayposition] != NULL && endofword == false)
        {
            ptr = ptr -> next[arrayposition];
        }
    c = fgetc(file);
    }

    if (c == EOF)
    {
        fclose(file);
        return true;
    }
    else
    {
        return false;
    }
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    return sizecount;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    if (freealloc(dictrie))
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool freealloc(trie *n)
{
    trie *ptr = n;

    for (int i = 0; i < 27; i++)
    {
        if (ptr -> next[i] != NULL)
        {
            freealloc(ptr -> next[i]);
            ptr -> next[i] = NULL;
        }
    }

    free(ptr);

    if (ptr)
    {
        return true;
    }
    else
    {
        return false;
    }
}