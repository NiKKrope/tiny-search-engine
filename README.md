# CS50 TSE
## Danylo Borodchuk (@NiKKrope)

(Fixed) Bug found with the indexer:
I accidentally assumed that the second line of the crawler output file of a webpage is the docID,
thus making the indexer write the wrong number for the .indexer output file

Add here any assumptions you made while writing the crawler, any ways in which your implementation differs from the three Specs, or any ways in which you know your implementation fails to work.

Commands: make, make clean

In order to test the program, just run "make" and then run the "testing.sh" script to run the crawler test.
