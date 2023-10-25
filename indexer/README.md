Returns 4 if too many arguments were given
Returns 11 if ".crawler" file not found
Returns 12 if indexFilename failed to read
Returns 13 if pageDirectory/1 failed to read

How to run: (make sure you are in "~/cs50-dev/tse-NiKKrope")
make                - > (will make library)
cd indexer          - > (will go to the indexer folder, because Makefile cannot for some reason)
make indexer        - > (will compile the indexer code)
make test           - > (will compile the indexertest code)


./testing.sh        - > (does everything and provides a testing.out file in the common folder)
