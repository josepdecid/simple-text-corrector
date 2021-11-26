# Simple Text Corrector

This is a simple context-unaware spell correction program written in C++.

## The idea behind

This tool aims to use a simple dictionary-based correction tool that is context-unaware, which means that it does not use any complex Deep Learning or probabilistic sequence prediction language model to obtain the word that may be the correct one given its higher likelihood. Instead, it uses a dictionary that checks the presence of every word in the text and, if it is not present, it searches and replaces it with the most similar one.

### A naive approach

The simplest and most common approach to tackle this problem is to compute the edit distance from the query term to each dictionary term and replace it with the word that has the minimum edit distance. The edit distance computes the dissimilarity of two strings by counting the minimum number of operations required to transform one word into the other. The branching factor of the possible edits is exponential and, therefore, so expensive to compute, scaling with the length of the text *t = |T|* and the size of the dictionary *d = |D|*.

### Pre-calculation approach

We can also pre-generate all possible terms with an edit distance from the query term and search them in the dictionary, much faster than the naive approach, but still presenting a search cost problem and another critical problem. The consideration of replacements and inserts means that the branching factor is language-dependent (depending on the alphabet size *a = |A|*), which in some languages like Chinese (where *a≃ 70,000* due to the Unicode Han characters) is a big problem.

### Symmetric Delete Spelling Correction approach

Recalling that the edit distance between two terms is symmetrical, we can generate all words with an edit distance *e_d* from the query term by deleting characters and checking them against all dictionary terms (and vice-versa). Therefore, following the same idea as the previous approach, but only applying delete operations, we pre-compute all possible words from a dictionary entry to a fixed maximum depth. By doing it in this way, the heavy computation cost is done in the pre-processing, and it scales really well once the text size increases, being almost linear to it instead of exponential. Given that in real-world scenarios, it is very likely that *d < t*, this approach produces a performance boost compared to the previous ones.

The code uses several data structure tricks to store the pre-computation in a way that it is the most efficient way to access it (nearly *O(1)*), by using priority hash maps (insertion and accession cost of *O(1)*) that map all possible generated words with deletions to the original word and the minimum edit distance to build it. The list of possible generated words is stored in priority queues that allow retrieving the nearest value from the first element (insertion cost *O(log n)* and accession cost of *O(1)*).

The overall cost to iterate over the data and generate these structures is the *O(d + t)* as we may consider the mean length of the words, which is involved in the branching factor and the insertions in the priority queue, as a constant factor.

## Compilation and execution

We can compile the code with the standard `GCC` compiler. However, I have tested enabling ceratin optimization features by adding some flags to achieve more significant performance. After trying some optimization flags, the one that performs the best is `-Ofast`. Similar to the `-O3` optimization, it also enables extra flags such as `-fassociative-math`, `-freciprocal-math`, `-fno-signed-zeros`, and `-fno-trapping-math`.

Other flags such as `-march=native`, which enables the usage of instructions specific for the given CPU architecture, produced slightly worse results. By using `-Ofast` in my machine, I have achieved a speed-up around x10 compared to the non-optimized build.

You can compile the code with the Makefile by running the following command from the root folder:
```shell
$ make compile
```

If you don't have the `make` utility installed, you can compile it manually with the following command:
```shell
$ g++ -std=c++11 -Ofast corrector.cc -o corrector.exe
```

To run the program, simply run the generated executable file and pass the two required parameters, the text file to correct, and the dictionary:

```shell
$ ./corrector.exe <ORIGINAL_TXT_PATH> <DICTIONARY_PATH>
```

Check the execution time appending the `time` utility to the previous command:
```shell
$ time <COMMAND>
```

> ⚠️ The code has been tested only in Windows Sybsystem for Linux (Ubuntu). Therefore, it can have strange or missing characters due to different encoding strategies. 