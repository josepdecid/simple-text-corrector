# Simple Text Corrector

This is a simple non-contextual spell correction program written in C++.

## Compilation and execution

We can compile the code with the standard `GCC` compiler. However, I have tested enabling ceratin optimization features by adding some flags to achieve more significant performance. After trying some optimization flags, the one that performs the best is `-Ofast`. Similar to the `-O3` optimization, it also enables extra flags such as `-fassociative-math`, `-freciprocal-math`, `-fno-signed-zeros`, and `-fno-trapping-math`.

Other flags such as `-march=native`, which enables the usage of instructions specific for the given CPU architecture, produced slightly worse results. By using `-Ofast` in my machine, I have achieved a speed-up around x10 compared to the non-optimized build.

You can compile the code with the Makefile by running the following command from the root folder:
```shell
make compile
```

If you don't have the `make` utility installed, you can compile it manually with the following command:
```shell
g++ -std=c++11 -Ofast corrector.cc -o corrector.exe
```

To run the program, simply run the generated executable file and pass the two required parameters, the text file to correct, and the dictionary:

```shell
./corrector.exe <ORIGINAL_TXT_PATH> <DICTIONARY_PATH>
```

Check the execution time appending the `time` utility to the previous command:
```shell
time <COMMAND>
```

> ⚠️ The code has not been tested in Windows! Therefore, it can have strange characters due to different encoding strategies. Use a Unix system to compile it and run or use the Windows Subsystem for Linux.