Going to use all 3 grace days and a couple late. Sorry

Removed the last line ("write" line) in the process file, just for simplicity, but there must still be a new line after (not sure why, but wouldn't work right without it)

also made sure all lines ended with semi colon for consistency (used line.length() - 2 to chop off the semi colon)


I tried to put descriptive comments, but basically I created pipes ahead of time because I was having issues with the child processes duplicating or hanging. The lines of "code" given were piped using the right hand side as "write" end and the left hand side as the "read". When the line started with a "p" or internalVar the process would wait for the pipe: p0 -> p2; for instance p2 would have to wait for p0. The rest was just checking for operation characters and having those performed.


Sites I've referenced (not included the things from class). nothing was directly copied, but documenting just in case:

https://www.tutorialspoint.com/What-does-int-argc-char-argv-mean-in-C-Cplusplus

https://www.geeksforgeeks.org/create-processes-with-fork-in-cpp/

https://www.geeksforgeeks.org/pipe-system-call/

https://stackoverflow.com/questions/4812891/what-is-fork-and-pipe-in-c-and-can-they-be-used-in-c

https://www.geeksforgeeks.org/how-to-convert-a-single-character-to-string-in-cpp/

https://www.reddit.com/r/learnprogramming/comments/4bdwhb/reading_and_writing_to_a_pipe_c/

https://www.reddit.com/r/learnprogramming/comments/lfl0i/passing_int_between_processes_via_pipe_c/




