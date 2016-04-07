#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/** DEFAULT VALUES **/

#define DEFAULT_CAPACITY 199 // default capacity for Lexicon 
#define MAX_WORD_LENGTH 100 // max length for words to be searched for


/** TYPE DEFINITIONS **/

/* Type: Lexicon
 * -------------
 * Stores words to be searched for in a hashtable for quick lookup.
 */
typedef struct lexicon {
    int nentries;
    int nbuckets;
    void **hashtable;
} Lexicon;

/* Type: Vector
 * ------------
 * Stores words found in honeycomb in sorted order.
 */
typedef struct vector {
    int nelems;
    char **words;
} Vector;

/* Type: Honeycomb
 * ---------------
 * Stores column representation of honeycomb structure, as well as array
 * tracking which letters in the honeycomb have been visited to facilitate
 * recursive backtracking during word search.
 */
typedef struct honeycomb {
    int ncols;
    char **columns;
} Honeycomb;


/** LEXICON MEMBER FUNCTIONS **/

/* Function: hash
 * --------------
 * This hash function I used here is taught by Barb Cutler, a CS professor from RPI. 
 * The choice of the value for the multiplier can have a significant impact on the 
 * performance of the algorithm, but not on its correctness.
 */
int hash(const char *s, int nbuckets) {
   const unsigned long MULTIPLIER = 2630849305L; // magic number
   unsigned long hashcode = 0;
   for (int i = 0; s[i] != '\0'; ++i)
      hashcode = hashcode * MULTIPLIER + s[i];
   return hashcode % nbuckets;                 // code returned is guaranteed to be nbuckets or less
}

/* Function: get_word
 * ------------------
 * Returns a pointer to the word stored inside cell.
 */
char *get_word(void *cell) {
    return (char *) cell + sizeof(void *);
}

/* Function: get_cell
 * ------------------
 * Returns a pointer to the cell corresponding to word, located in the
 * linked list attached to the bucket in the Lexicon's hashtable hashed
 * to by word. Returns NULL if no cell is found.
 */
void *get_cell(const Lexicon *lex, const char *word) {
    for (void *cell = lex->hashtable[hash(word, lex->nbuckets)]; cell != NULL; cell = ((void **) cell)[0])
        // loop through linked list attached to hashed bucket until matching key found
        if (strcmp(get_word(cell), word) == 0) return cell;
    return NULL;
}

/* Function: link_cell
 * -------------------
 * Checks if the bucket hashed to by cell's word already has a linked list
 * of cells attached to it, in which case cell is linked to the end of the
 * list. Otherwise links cell to the bucket.
 */
void link_cell(void **hashtable, int nbuckets, void *cell) {
    int hashcode = hash(get_word(cell), nbuckets);
    if (hashtable[hashcode] == NULL) {             // add to empty bucket
        hashtable[hashcode] = cell;
    } else {                                       // or add to end of existing list
        void *cur;
        for (cur = hashtable[hashcode]; ((void **) cur)[0] != NULL; cur = ((void **) cur)[0]); // FIXME: JUST LINK CELL TO BASE!!!
        ((void **) cur)[0] = cell;                 // set last cell to point to new cell
    }
}

/* Function: check_rehash
 * ----------------------
 * Checks if the Lexicon's hashtable has exceeded a load factor of 1.5,
 * and if so, rehashes. 
 * This idea of this rehash function I used here is taught by Barb Cutler, a CS professor from RPI.
 */
void check_rehash(Lexicon *lex) {
    if ((double) lex->nentries / (double) lex->nbuckets > 1.5) { // load factor exceeded
        int newsize = lex->nbuckets * 3 + 1;
        void **newtable = (void **) calloc(newsize, sizeof(void *));
        assert(newtable != NULL);
        for (int i = 0; i < lex->nbuckets; ++i) {   // loop through all buckets
            void *cur = lex->hashtable[i];
            while (cur != NULL) {                   // loop through every cell in bucket's linked list
                link_cell(newtable, newsize, cur);  // rehash cell
                void *next = ((void **) cur)[0];    // copy next item in linked list
                ((void **) cur)[0] = NULL;          // before setting cell's pointer to next element to NULL
                cur = next;
            }
        }
        free(lex->hashtable);
        lex->hashtable = newtable;
        lex->nbuckets = newsize;
    }
}

/* Function: lex_contains
 * ----------------------
 * Returns true if the Lexicon contains word, or false otherwise. Operates
 * in linear time.
 */
bool lex_contains(const Lexicon *lex, const char *word) {
    return get_cell(lex, word) != NULL;
}

/* Function: lex_store
 * -------------------
 * Stores word in the Lexicon by creating a cell for it, which includes
 * space for a pointer to the next node in a linked list. Returns true if
 * word is not yet a member of the Lexicon and has been stored for the
 * first time, or false otherwise. Runs in linear time.                   // FIXME: MAKE AMORTIZED CONSTANT
 */
bool lex_store(Lexicon *lex, const char *word) {
    if (get_cell(lex, word) == NULL) {               // word not yet a member of lexicon
        check_rehash(lex);
        void *cell = malloc(sizeof(void *) + strlen(word) + 1); // +1 for '\0'
        assert(cell != NULL);                                   // check that allocation is successful
        ((void **) cell)[0] = NULL;                             // set pointer to next element to NULL
        strcpy(get_word(cell), word);                           // copy word to cell
        link_cell(lex->hashtable, lex->nbuckets, cell);         // copy cell to hashtable
        lex->nentries++;
        return true;
    }
    return false;
}

/* Function: read_word
 * -------------------
 * Reads a single word from file and returns true, or false when the end
 * of file is reached.
 */
bool read_word(FILE *file, char buffer[]) {
    return fscanf(file, "%s", buffer) != EOF;
}

/* Function: lex_fill
 * ------------------
 * Reads all words from file into wordlex, and stores all substrings
 * of each word that start at the first character in fraglex to guide
 * recursive search toward valid words.
 */
void lex_fill(Lexicon *wordlex, Lexicon *fraglex, FILE *file) {
    char buffer[MAX_WORD_LENGTH];
    while (read_word(file, buffer)) { // read in all words from file
        if (lex_store(wordlex, buffer)) { // if new word, store all of its fragments in fraglex
            for (int i = strlen(buffer) - 1; i > 0; i--) {
                buffer[i] = '\0';
                lex_store(fraglex, buffer);
            }
        }
    }
}

/* Function: lex_dispose
 * ---------------------
 * Frees all heap memory assiciated with Lexicon structure.
 */
void lex_dispose(Lexicon *lex) {
    for (int i = 0; i < lex->nbuckets; ++i) { // loop through all buckets
        void *cur = lex->hashtable[i];
        while (cur != NULL) {                 // loop through every cell in bucket's linked list
            void *next = ((void **) cur)[0];
            free(cur);
            cur = next;
        }
    }
    free(lex->hashtable);
    free(lex);
}

/* Function: lex_create
 * --------------------
 * Creates a Lexicon structure, and sets all buckets (base pointers to
 * linked lists) in the hashtable to NULL. Asserts that heap allocation of
 * Lexicon and internal hashtable are successful.
 */
Lexicon *lex_create() {
    Lexicon *lex = (Lexicon *) malloc(sizeof(Lexicon));
    assert(lex != NULL);
    lex->nentries = 0;
    lex->nbuckets = DEFAULT_CAPACITY;
    lex->hashtable = (void **) calloc(lex->nbuckets, sizeof(void *));
    assert(lex->hashtable != NULL);
    for (int i = 0; i < lex->nbuckets; ++i) {
        lex->hashtable[i] = NULL;
    }
    return lex;
}


/** VECTOR MEMBER FUNCTIONS **/

/* Function: vec_dispose
 * ---------------------
 * Frees all heap memory assiciated with Vector structure.
 */
void vec_dispose(Vector *vec) {
    for (int i = 0; i < vec->nelems; ++i) { // free all column strings
        free(vec->words[i]);
    }
    free(vec->words);
    free(vec);
}

/* Function: vec_create
 * --------------------
 * Creates a Vector structure. Asserts that heap allocation of Vector and
 * internal array are successful.
 */
Vector *vec_create(int size) {
    Vector *vec = (Vector *) malloc(sizeof(Vector));
    assert(vec != NULL);
    vec->nelems = 0;
    vec->words = (char **) calloc(size, sizeof(char *));
    assert(vec->words != NULL);
    return vec;
}


/** HONEYCOMB MEMBER FUNCTIONS **/


/* Function: hcomb_store
 * ---------------------
 * Converts the set of honeycomb layers to column format, and stores
 * each column in the Honeycomb structure. The bool value right determines
 * which side of the honeycomb the columns are added to.
 */
void hcomb_store(Honeycomb *hc, char **layers, int nlayers, bool right) {
    for (int i = nlayers - 1; i >= 0; i--) {
        int collen = 2 * nlayers - i;
        char *column = (char *) malloc(collen + 1); // + 1 for '\0'
        column[collen] = '\0';
        strncpy(column + nlayers - i - 1, layers[i] + i, i + 2);       // first copy contiguous segment of layer i in ith column from center
        for (int j = nlayers - 1; j > i; --j) {                        // then add characters to column that are part of adjacent layers
            strncpy(column + nlayers - j - 1, layers[j] + i, 1);             // character on lower end of ith column from center
            strncpy(column + nlayers - i + j, layers[j] - i + 3 * j + 1, 1); // character on upper end of ith column from center
        }
        if (right) {
            hc->columns[nlayers + i + 1] = column;
        } else {
            hc->columns[nlayers - i - 1] = column; // 'half' multiplier places column in the correct half of hc's internal columns array
        }
    }
}

/* Function: hcomb_fill
 * --------------------
 * Fills Honeycomb structure with letters from file. Begins by reading
 * letters from each layer into an array representing either the right
 * half of a layer, the left half, or directly into an array representing
 * the center column. The sets of layers representing the right and left
 * sides are then passed to hcomb_store to be converted into a column
 * representation of the honeycomb.
 */
void hcomb_fill(Honeycomb *hc, FILE *file, int nlayers) {
    char *center = (char *) malloc(2 * nlayers); // space for '\0'
    fscanf(file, " %c", center + nlayers - 1);
    center[2 * nlayers - 1] = '\0';
    if (nlayers > 1) {
        char *rightlayers[nlayers - 1];
        char *leftlayers[nlayers - 1];
        for (int i = 1; i < nlayers; ++i) {
            int halflayerlen = 2 + (i - 1) * 3;
            char *right = (char *) malloc(halflayerlen + 1);
            char *left = (char *) malloc(halflayerlen + 1);
            fscanf(file, " %c", center + nlayers - 1 + i);  // read first char in layer string to upper center column
            for (int j = halflayerlen - 1; j >= 0; --j)     // read right side of layer and add to right layer string in REVERSE order
                fscanf(file, " %c", &(right[j]));
            right[halflayerlen] = '\0';
            rightlayers[i - 1] = right;
            fscanf(file, " %c", center + nlayers - 1 - i);
            for (int k = 0; k < halflayerlen; ++k)         // read left side of layer and add to left layer string in SAME order
                fscanf(file, " %c", left + k);
            left[halflayerlen] = '\0';
            leftlayers[i - 1] = left;
        }
        hcomb_store(hc, leftlayers, nlayers - 1, false); // pass nlayers - 1 because center layer is not stored; only layers 1 through nlayers - 1
        hcomb_store(hc, rightlayers, nlayers - 1, true);
        for (int i = 0; i < nlayers - 1; ++i) {
            free(rightlayers[i]);
            free(leftlayers[i]);
        }
    }
    hc->columns[nlayers - 1] = center;
}

/* Function: hcomb_dispose
 * -----------------------
 * Frees all heap memory assiciated with Honeycomb structure.
 */
void hcomb_dispose(Honeycomb *hc) {
    for (int i = 0; i < hc->ncols; ++i) { // free all column strings
        free(hc->columns[i]);
    }
    free(hc->columns);
    free(hc);
}

/* Function: hcomb_create
 * ----------------------
 * Creates a Lexicon structure, and sets all buckets (base pointers to
 * linked lists) in the hashtable to NULL.
 */
Honeycomb *hcomb_create(int nlayers) {
    Honeycomb *hc = (Honeycomb *) malloc(sizeof(Honeycomb));
    hc->ncols = 2 * nlayers - 1;
    hc->columns = (char **) calloc(hc->ncols, sizeof(char *)); // space for arrays representing honeycomb's columns
    assert(hc->columns != NULL);
    return hc;
}


/** SEARCH & SORT FUNCTIONS **/

/* Function: find_words_helper
 * ---------------------------
 * Uses recursive backtracking to search for words starting with the
 * letter in column col of the honeycomb (from left), at index index
 * (from bottom) of the column. 
 */
void find_words_helper(Honeycomb *hc, Lexicon *wordlex, Lexicon *fraglex, Vector *found, char *frag, int col, int index) {
    if ((col < 0 || index < 0 || col >= hc->ncols || index >= strlen(hc->columns[col]) || (hc->columns[col])[index] > 'Z'
            || !(lex_contains(fraglex, frag))) && strlen(frag) != 0) {
        return;
    } else {
        strncat(frag, hc->columns[col] + index, 1); // concatenate current character to end of frag
        if (lex_contains(wordlex, frag)) {
            found->words[found->nelems] = strdup(frag);
            found->nelems++;
        }
        (hc->columns[col])[index] = tolower((hc->columns[col])[index]); // setting to lowercase marks as checked
        for (int i = -1; i <= 1; ++i)
            for (int j = -1; j <= 1; ++j)
                if (!(col + i == col && index + j == index))
                    find_words_helper(hc, wordlex, fraglex, found, frag, col + i, index + j);
        (hc->columns[col])[index] = toupper((hc->columns[col])[index]);
        frag[strlen(frag) - 1] = '\0';
    }
}

Vector *find_words(Honeycomb *hc, Lexicon *wordlex, Lexicon *fraglex)
{
    Vector *found = vec_create(wordlex->nentries);
    char *frag = (char *) malloc(MAX_WORD_LENGTH);
    for (int i = 0; i < hc->ncols; ++i) {
        for (int j = 0; j < strlen(hc->columns[i]); ++j) {
            frag[0] = '\0';
            find_words_helper(hc, wordlex, fraglex, found, frag, i, j);
        }
    }
    free(frag);
    return found;
}

/* Function: cmp_words
 * -------------------
 * Compares two words for sort order when sorting Vector of found words.
 */
int cmp_words(const void *word1, const void *word2) {
    return strcmp(*(char **) word1, *(char **) word2);
}

int main(int argc, char *argv[]) {
    FILE *honeycomb = fopen(argv[1], "r");
    FILE *dictionary = fopen(argv[2], "r");
    assert(honeycomb != NULL && dictionary != NULL);

    int nlayers;
    fscanf(honeycomb, "%d", &nlayers);
    Honeycomb *hc = hcomb_create(nlayers);
    hcomb_fill(hc, honeycomb, nlayers);
    fclose(honeycomb);

    Lexicon *wordlex = lex_create();
    Lexicon *fraglex = lex_create();
    lex_fill(wordlex, fraglex, dictionary);
    fclose(dictionary);
    lex_store(fraglex, "");

    Vector *found = find_words(hc, wordlex, fraglex);

    if (found->nelems == 0) {
        printf("No words found.\n");
    } else {
        qsort(found->words, found->nelems, sizeof(char *), cmp_words);
        printf("Words found: %d\n", found->nelems);
        printf("\n%s\n", found->words[0]);
        for (int i = 1; i < found->nelems; ++i) {
            if (strcmp(found->words[i], found->words[i - 1]) != 0) // prevent printing duplicates
                printf("%s\n", found->words[i]);
        }
    }
    printf("\n");

    hcomb_dispose(hc);
    lex_dispose(wordlex);
    lex_dispose(fraglex);
    vec_dispose(found);
    return 0;
}