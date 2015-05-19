PROJECTDIR=$(PWD)

CROSSCOMPILE=
CC=gcc
STRIP=strip
 
#for include file
INCLUDEDIR=-I ./inc
 
#for Library file
LIBRARYDIR=-L ./

#for math function
LIBRARYS=-lm -lpthread

#define compile parameters
#:= and = is the same
FALGS:=-O2 -Wall

#we can add a new parameter
FALGS+=-g

#Set Install Dir
INSTALLDIR=./bin

#Set Target
EXEBINARY=FIFO.out
STRIPEXEBINARY=FIFO
