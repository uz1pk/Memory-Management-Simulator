# Usman Zaheer CIS 3110 (Mmeory Management)
Program Description: 
- Descrete Event Simulation of 4 Memory Management Algorithms that implements:
    - Best-fit
    - Worst-fit
    - Next-fit
    - First-fit

## Compilation:

1. make sure the folder "Memory" is the current working directory
2. type: "make"

## Execution:

- ./holes [fileName] [<first | best | next | worse/worst>]

## Assumptions:

- This program assumes that the input file is in correctly ordered and sepereated by a single space.
    - Both values must be integers (both PID and memory needed by the process)
    - Program assumes that all files aren't seperated by any new lines. rather all data/processes are seprate by 1 space line by line

## Future Improvements:

After writing the code I noticed a few repeated lines of code, I feel like I could've imporved on this by breaking it up
into more sepereated functions. This would've also made my code 10x easier to read and debug.
