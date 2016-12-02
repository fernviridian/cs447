Setup for OSX:
works on 10.11,10.12; tested on both


# Grade
100/100

# Run
Install fltk through homebrew: 
`brew install fltk`
`./compile.sh`
`./a.out`

## Source changes 
(already done for you to get working on OSX):

replace any instances of 
`#include <GL/glu.h>`
with 
`#include <OpenGL/glu.h>`

in libtarga.c
replace 
`#include <malloc.h>`
with 
`#include <stdlib.h>`

## Controls

press keys:
'r' for ride roller coaster
't' for merry go round view
'y' for tree view
'u' for top down view
'i' for seeing the roller coaster texture

## [Demo](https://gfycat.com/BoldForkedCaimanlizard)
