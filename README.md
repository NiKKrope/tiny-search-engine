# CS50 TSE
## Danylo Borodchuk (@NiKKrope)

The assignment and Specs are in a [public repo](https://github.com/CS50Spring2023/labs/tse).
Do not clone that repo; view it on GitHub.
Watch there for any commits that may represent updates to the assignment or specs.

(Fixed) Bug found with indexer:
I accidently assumed that the second line of the crawler output file of a webpage is the docID,
thus making the indexer write the wrong number for the .indexer output file


Add here any assumptions you made while writing the crawler, any ways in which your implementation differs from the three Specs, or any ways in which you know your implementation fails to work.

Commands: make, make clean

In order to test the program, just run "make" and then run the "testing.sh" script to run the crawler test.