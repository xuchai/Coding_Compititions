This program is to solve hexagonal honeycomb word search problem. 
To read the detailed description of this problem, please check
“Challenge Description.pdf”

This program takes in two text files as command-line arguments. The first
file is used to create the honeycomb word search. Its first line must
contain the number of layers, followed by strings representing each
layer of the honeycomb, proceeding from the center outward, where the
first letter in each string is the top letter in its layer's hexagon,
and the rest of the string wraps around the center of the honeycomb in
clockwise order. The second file is the word search's lexicon, which
must contain one word per line.

The honeycomb is internally represented as a series of columns, with an
array of characters comrpising the letters in each column. The
implementation relies on recursive backtracking to search for words, a
hashtable for checking each word against the user-specified file of
valid words, and a sortable vector that stores words found.