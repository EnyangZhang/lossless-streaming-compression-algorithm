#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define constant for "not exist" status (used for sequence/code lookup)
#define NOT_EXIST -1

// Dictionary structure: stores mapping between string sequences and integer codes
typedef struct {
    char**  seq;        // 2D character pointer array to store string sequences
    int*    code;       // Integer array to store corresponding codes for sequences
    int     size;       // Current number of elements stored in the dictionary
    int     max_size;   // Maximum capacity of the dictionary (total elements it can hold)
} Dictionary;

// Insert a new string sequence into the dictionary
// Allocates memory for the sequence and assigns a unique code (based on current size)
void insert_seq(Dictionary* dict, char* seq) {
    int i = dict->size;
    // Allocate memory for the sequence (add 1 byte for null terminator '\0')
    dict->seq[i] = malloc(sizeof(char) * strlen(seq) + 1);
    // Assign code: use current index as the unique code (0-based)
    dict->code[i] = i;
    // Increment the count of elements in the dictionary
    dict->size++;
    // Copy the input sequence to the allocated memory (safe string copy)
    strcpy(dict->seq[i], seq);
}

// Initialize the dictionary with specified maximum capacity
// Pre-populates the dictionary with '#' (special character) and uppercase letters A-Z
void init_dictionary(Dictionary* dict, int max_size) {
    dict->max_size = max_size;
    dict->size     = 0;
    // Allocate memory for the sequence array (array of char pointers)
    dict->seq      = malloc(sizeof(char*) * max_size);
    // Allocate memory for the code array (array of integers)
    dict->code     = malloc(sizeof(int) * max_size);

    // Insert the special character '#' as the first element (code = 0)
    insert_seq(dict, "#");
    // Initialize a buffer for uppercase letters (A-Z)
    char seq[2] = "A";
    // Loop to insert A to Z into the dictionary (codes 1-26)
    for (int i = 0; i < 26; i++) {
        insert_seq(dict, seq);
        // Increment the character (A -> B -> ... -> Z)
        seq[0]++;
    }
}

// Look up the code of a given string sequence in the dictionary
// Returns the code if found, NOT_EXIST (-1) if the sequence is not in the dictionary
int get_seq_code(Dictionary* dict, char* seq) {
    int i; // Loop variable (declared at start for C89 compatibility)
    // Iterate through all elements in the dictionary
    for (i = 0; i < dict->size; i++) {
        // Compare target sequence with current sequence (strcmp returns 0 if equal)
        if (strcmp(dict->seq[i], seq) == 0) {
            // Return the corresponding code if match is found
            return dict->code[i];
        }
    }
    // Return NOT_EXIST if the sequence is not found
    return NOT_EXIST;
}

// Look up the string sequence by a given code in the dictionary
// Returns pointer to the sequence if found, NULL if the code is invalid/out of bounds
char* get_code_seq(Dictionary* dict, int code) {
    // Boundary check: reject negative codes or codes beyond dictionary size (avoid out-of-bounds)
    if (code < 0 || code >= dict->size) {
        return NULL;
    } else {
        int i = code; // Code equals array index (no need to traverse for lookup)
        return dict->seq[i]; // Return the corresponding sequence
    }
}

// Complete LZW decode function: decode full code array & dynamically update dictionary
// Parameters: 
//   codes[] - array of encoded integers (input)
//   n       - length of the codes array
//   dict    - pointer to the initialized dictionary
void lzw_decode(int codes[], int n, Dictionary* dict) {
    int code;          // Variable to store current code being processed
    char prev[1000];   // Buffer to store previous decoded sequence
    char* output;      // Pointer to store current decoded string sequence

    // Process the first code (initialization step of LZW decoding)
    code = codes[0];
    output = get_code_seq(dict, code);
    printf("%s", output); // Print first decoded sequence (no newline for continuous output)

    // Traverse and process remaining codes (from index 1 to end)
    int i;
    for (i = 1; i < n; i++) {
        code = codes[i];                  // Get current code from the array
        strcpy(prev, output);             // Save previous decoded sequence to buffer
        output = get_code_seq(dict, code);// Look up sequence for current code
        
        // Splice new sequence: previous sequence + first char of current sequence
        sprintf(prev, "%s%c", prev, output[0]);
        insert_seq(dict, prev);           // Insert new sequence to update dictionary
        
        printf("%s", output);             // Print current decoded sequence (continuous text)
    }
    printf("\n"); // Add newline after full decoded text (formatting)
}

// Main function: initialize dictionary and test LZW decode with sample codes
int main() {
    Dictionary dict;
    // Initialize dictionary with maximum capacity of 1000 elements
    init_dictionary(&dict, 1000);
    
    // Test code array (matches encoder output for "TOBEORNOTTOBEORTOBEORNOT")
    int arr[16] = {20, 15, 2, 5, 15, 18, 14, 15, 20, 27, 29, 31, 36, 30, 32, 34};
    int code_count = sizeof(arr) / sizeof(arr[0]); // Calculate length of code array
    
    // Run LZW decoding and print full result
    printf("Decoded Text: ");
    lzw_decode(arr, code_count, &dict);

    // Free allocated memory (good practice to avoid memory leaks)
    for (int j = 0; j < dict.size; j++) {
        free(dict.seq[j]);
    }
    free(dict.seq);
    free(dict.code);

    return 0;
}
