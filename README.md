# COMP 530 Assignments

Programming assignments for UNC's COMP 530 course.


## Assignments

### HW1 - Linux Refresher

Reads in characters from standard input and buffers them to 80 character lines. Some character replacements are also performed.

### HW2 - Simple Shell

Reads in a line from standard input and executes it in its own process.

### HW3 - Threaded Producer/Consumer

This is a rewrite of [HW1](#hw1---linux-refresher). It now uses separate threads for each task. Characters are read in from one thread, characters are replaced on separate threads, and a final thread outputs complete lines. Data is passed between threads using a bounded buffer implemented with semaphores.

### HW4 - Producer/Consumer with Message Passing

This is another rewrite of [HW1](#hw1---linux-refresher). It now uses separate process for each task. Characters are read in from one process, replacement is performed in separate processes, and a final process is responsible for outputting complete lines. Data is passed between processes using pipes.

This is also the most extendable version of the text-modification system. Additional 'replacers' can be added by creating a function that reads from one pipe and writes to another and then adding that function to the array of replacers.


## License

This code is licensed under the MIT license.


## Author

Chathan Driehuys (chathan@driehuys.com)
