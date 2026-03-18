#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define constant for "not exist" status (used for sequence lookup)
#define NOT_EXIST -1

// Define the Dictionary structure to store sequence-code pairs
typedef struct {
    char**  seq;        // 2D char pointer to store string sequences
    int*    code;       // Integer array to store corresponding codes
    int     size;       // Current number of elements stored in the dictionary
    int     max_size;   // Maximum capacity of the dictionary
} Dictionary;

// Insert a string sequence into the dictionary
void insert_seq(Dictionary* dict, char* seq) {
    int i = dict->size;
    // Allocate memory for the sequence (add 1 for null terminator)
    dict->seq[i] = malloc(sizeof(char) * strlen(seq) + 1);
    // Assign code (use current index as the code value)
    dict->code[i] = i;
    // Increment the size of the dictionary
    dict->size++;
    // Copy the input sequence to the allocated memory
    strcpy(dict->seq[i], seq);
}

// Initialize the dictionary with specified maximum capacity
// Pre-populate with '#' and uppercase letters A-Z
void init_dictionary(Dictionary* dict, int max_size) {
    dict->max_size = max_size;
    dict->size     = 0;
    dict->seq      = malloc(sizeof(char*) * max_size); // Allocate memory for sequence array
    dict->code     = malloc(sizeof(int) * max_size);   // Allocate memory for code array

    // Insert the special character '#' as the first element
    insert_seq(dict, "#");
    // Initialize a sequence buffer for uppercase letters (A-Z)
    char seq[2] = "A";
    // Loop to insert A to Z into the dictionary
    for (int i = 0; i < 26; i++) {
        insert_seq(dict, seq);
        // Increment the character (A->B->...->Z)
        seq[0]++;
    }
}

// Print the dictionary in a formatted table (Code | Sequence)
void print_dictionary(Dictionary* dict) {
    printf("=========================\n");
    printf(" Code     Sequence\n");
    printf("=========================\n");
    // Iterate through all elements in the dictionary
    for (int i = 0; i < dict->size; i++) {
        // Print code with 5-width alignment, followed by a space
        printf("%5d%7c", dict->code[i], ' ');
        // Print the corresponding sequence
        printf("%s\n", dict->seq[i]);
    }
    printf("=========================\n");
}

// Look up the code of a given sequence in the dictionary
// Return the code if found, NOT_EXIST (-1) if not found
int get_seq_code(Dictionary* dict, char* seq) {
    // Iterate through all existing sequences in the dictionary
    for (int i = 0; i < dict->size; i++) {
        // Compare target sequence with current sequence (strcmp returns 0 if equal)
        if (strcmp(dict->seq[i], seq) == 0) {
            // Return the corresponding code if match is found
            return dict->code[i];
        }
    }
    // Return NOT_EXIST if sequence is not found in the dictionary
    return NOT_EXIST;
}

// LZW encoding function: compress input text using the dictionary
void lzw_encode(char* text, Dictionary* dict) {
    char current[1000];  // Buffer to store current sequence being processed
    char next;            // Next character to append to current sequence
    int code;             // Code of the found sequence
    int i = 0;            // Index for iterating through input text

    // Iterate through each character in the input text
    while (i < strlen(text)) {
        // Initialize current sequence with the current character
        sprintf(current, "%c", text[i]);
        // Get the next character in the text
        next = text[i+1];

        // Keep appending next character if current sequence exists in dictionary
        while (get_seq_code(dict, current) != NOT_EXIST) {
            // Append next character to current sequence
            sprintf(current, "%s%c", current, next);
            // Move to next character in text
            i++;
            // Update next character to the new next position
            next = text[i+1];
        }

        // Remove the last appended character (since current sequence is not found)
        current[strlen(current) - 1] = '\0';
        // 新增行：重新赋值next为当前文本字符
        next = text[i];
        // Get the code of the valid existing sequence
        code = get_seq_code(dict, current);
        // Reconstruct the non-existing sequence (current + next)
        sprintf(current, "%s%c", current, next);
        // Insert the new sequence into the dictionary
        insert_seq(dict, current);
        
        // Print the code of the valid sequence (newly added line)
        printf("%d\n", code);
    }
}

int main() {
    // Declare a Dictionary instance
    Dictionary dict;
    // Initialize dictionary with max capacity of 1000
    init_dictionary(&dict, 1000);
    // Print the initialized dictionary (shows #, A-Z with codes 0-26)
    print_dictionary(&dict);
    
    // 以下行被删减：printf("%d\n", get_seq_code(&dict, "B"));
    // Call LZW encode function with target text "TOBEORNOTTOBEORTOBEORNOT"
    lzw_encode("TOBEORNOTTOBEORTOBEORNOT", &dict);
    
    return 0;
}
