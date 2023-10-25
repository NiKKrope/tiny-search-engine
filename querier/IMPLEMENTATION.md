# CS50 TSE Querier
## Implementation Spec

Here we focus on the core subset:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 

We mainly use the index data structure and also a short "result" data structure, so that we can pass in multiple varuables while iterating through the counters of the index (which is a hashtable in itself)

## Control flow

The Querier is implemented in one file `querier.c`, with many functions.

### main

The `main` function handles command-line argument parsing, validates the existence of required files, loads the index file, processes user queries, and performs the corresponding search operations.

## Functional Decomposition:

The Querier program consists of the following modules or functions:

`clean_query`: This function cleans up the user query by converting it to lowercase, removing leading/trailing whitespace, and handling syntax errors related to the presence of consecutive "AND" or "OR" operators.

`read_query`: This function reads the user query from standard input.

`compare_min`: This function is used during query processing to compare the minimum score of an "AND" sequence with the previous word's score.

`reset_counter`: This function resets the value of each counter in a given counterset to zero.

`max_counter`: This function sets each counter's value in a counterset to the maximum possible integer value.

`add_ands`: This function adds the minimum value of an "AND" sequence to the total score of the corresponding document ID.

`check_if_contains`: This function checks if a given document ID is missing from a word's entry in the index and sets the counter for that document ID to zero if it is missing.

`search_result`: This struct represents a search result containing the document ID, score, and a counterset for tracking already printed results.

`result_set_score`: This function sets the score of a search result.

`get_result_score`: This function retrieves the score of a search result.

`result_get_printed`: This function retrieves the counterset of already printed results from a search result.

`find_max_result`: This function finds the maximum score among search results.

## Pseudo code for Logic/Algorithmic Flow:
The Querier program follows the following logic/algorithmic flow:

1. Read the user query using the read_query function.
2. Clean up the query using the clean_query function.
3. Initialize a counterset to store the ranking of documents based on the query.
4. Tokenize the cleaned query.
5. For each token in the query:
- Check if the token is an "AND" or "OR" operator.
- If it is an "AND" operator, compare the minimum score of the "AND" sequence with the previous word's score using the compare_min function.
- If it is an "OR" operator, reset the counterset using the reset_counter function.
- If it is a word:
    - Look up the word in the index and retrieve its counterset.
    - If the counterset is not NULL:
        - For each document ID and its score in the counterset:
            - If the document ID is missing from the word's entry in the index, set the counter for that document ID to zero using the check_if_contains function.
            - If the token is the first word in the query, set the counter value in the ranking counterset to the score from the index.
            - If the token is an "AND" operator, add the minimum value of the "AND" sequence to the total score of the corresponding document ID using the add_ands function.
            - If the token is an "OR" operator, set the counter value

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.
