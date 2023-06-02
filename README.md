I only have 32 bit SFML binaries so compile for 32 bit. 
If you try to build and run the project, you will need to move the stuff from .sfml/bin/ into the directory where the .exe ended up. 
A version of minesweeper very similar to the old school looking version from microsoft.
Made with C++ and SFML. I went back and re wrote almost the entire project as it had lots of very unreadable confusing and non-modern c++ code.
switched out things like manual memory management for std::vectors and other more modern options. I also fixed
potential bugs lurking in the code in the form of race conditions between the timer thread and main thread.




