# shuffler

Randomly shuffles the lines in files. Only use this program for very large files that don't fit into RAM. For smaller files, you're better off with the GNU `shuf` command.

The program uses a "merge shuffle" approach to recursively shuffle files: If the file is small enough, read it into memory and shuffle it. Otherwise, randomly split the file into two files by reading line-by-line and randomly assigning each line to one of the output files. Then recursively shuffle each output file and the concatenate the shuffled outputfiles.

Minimally tested, but I shuffled a 40GB file in 30 minutes on a macbook when `shuf` was unable to shuffle it.

Usage:
```
shuffler fileToShuffle outputFile
```
