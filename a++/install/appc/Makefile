####################################################################### 
#      Makefile for use with ARS interpreter
#######################################################################
CC = gcc
#CXX = g++
CXX = gcc
CFLAGS=-g 
#     
.SUFFIXES: .c
.c.o:
	$(CXX) -c  $<
#######################################################################
ifiles = arsc.o arsparser.o 

arscint: $(ifiles) mainint.o 
	gcc $(CFLAGS) -rdynamic -o arscint $(ifiles) mainint.o -ldl -lgc -lc 

