# COMP 530 Assignments

Programming assignments for UNC's COMP 530 course.


## Assignments

### HW1 - Linux Refresher

Reads in characters from standard input and buffers them to 80 character lines. Some character replacements are also performed.

### HW2 - Simple Shell

Reads in a line from standard input and executes it in its own process.

### HW3 - Threaded Producer/Consumer

This is a rewrite of [HW1](#hw1---linux-refresher). It now uses separate threads for each task. Characters are read in from one thread, characters are replaced on separate threads, and a final thread outputs complete lines. Data is passed between threads using a bounded buffer implemented with semaphores.


## License

This code is licensed under the MIT license.


## Author

Chathan Driehuys (chathan@driehuys.com)
