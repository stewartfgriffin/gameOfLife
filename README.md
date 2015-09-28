Simple Conway's Game of Life. 
=============================
Written mainly to practise C programming.

*Example usage*: $ ./gameOfLife -t=25 -w=9 -h=9 -p=0,1#1,2#2,0#2,1#2,2

*To compile*: 

1) make sure gcc is installed. 

2) run: $ make

*options*:
---------
*-t*: number of turns to run life for

*-w*: width of grid

*-h*: height of grid

*-p*: points set to alive at simulation start. Points are made up of comma separated natural numbers, falling within the grids dimensions, and separated from one another by the '#' character (see usage above for example).

Compilation via makefile uses gnu11 standard so as to use non-standard C, posix function strtok_r().
