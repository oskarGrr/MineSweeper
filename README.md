COMPILE FOR X86 NOT 64 BIT. A version of minesweeper very similar to the old school looking version from microsoft.
Made with C++ and SFML. I went back and re wrote almost the entire project as it had lots of very unreadable confusing and non-modern c++ code.
switched out things like manual memory management for std::vectors and other more modern options. I also fixed
potential bugs lurking in the code in the form of race conditions between the timer thread and main thread.




