#!/bin/bash
# This script is used to test the code

# Compile the code
make clean
cd ..
make clean

# Make everything
make
cd indexer
make indexer
cd ../querier
make

# Run everything
~/cs50-dev/tse-NiKKrope/crawler/crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ~/cs50-dev/tse-NiKKrope/common 10
~/cs50-dev/tse-NiKKrope/indexer/indexer ~/cs50-dev/tse-NiKKrope/common ~/cs50-dev/tse-NiKKrope/common/.indexer
~/cs50-dev/tse-NiKKrope/querier/querier ~/cs50-dev/tse-NiKKrope/common ~/cs50-dev/tse-NiKKrope/common/.indexer < <(echo "")

# Test case 1: Invalid query
echo ""
echo "Test case 1: Invalid query"
echo "and and" | ./querier ../common123 ../common/.i123ndexer

# Test case 2: Missing index file
echo ""
echo "Test case 2: Missing index file"
echo "word" | ./querier ../common ../common/nonexistentfile

# Test case 3: Valid query
echo ""
echo "Test case 3: Valid query"
echo "for and for or for" | ./querier ../common ../common/.indexer < <(echo "for and for or for")
