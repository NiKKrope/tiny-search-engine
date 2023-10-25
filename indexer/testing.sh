#!/bin/bash

# Clean Everything
make clean
cd ..
make clean

# Make everything
make
cd indexer
make indexer

# Run everything
~/cs50-dev/tse-NiKKrope/crawler/crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ~/cs50-dev/tse-NiKKrope/common 10
~/cs50-dev/tse-NiKKrope/indexer/indexer ~/cs50-dev/tse-NiKKrope/common ~/cs50-dev/tse-NiKKrope/common/.indexer

# Make and run he tester
make test
~/cs50-dev/tse-NiKKrope/indexer/test ~/cs50-dev/tse-NiKKrope/common/.indexer ~/cs50-dev/tse-NiKKrope/common/testing.out
