# This is the makefile for the March Madness Engine
LIB_DIR=lib
BIN_DIR=bin
GTEST_DIR=${LIB_DIR}/gtest-1.7.0
CLEAN_FILES=dbctest.o predictor.o dbc.o dbc dbctests mmetests mme
C_FLAGS=-lsqlite3

# Default functionality should be to build the program and tests
all: mmetests mme 

# To build the gtest library run init once
init : gtest

# Command to build gtest library
gtest:
	g++ -isystem ${GTEST_DIR}/include -I${GTEST_DIR} -pthread -c \
	 	${GTEST_DIR}/src/gtest-all.cc 
	ar -rv libgtest.a gtest-all.o

# This March Madness Engine test builder all test should be a dependency of mmetests
mmetests: dbctests

dbctests: dbctest.cpp dbc.o
	g++ -isystem ${PWD}/${GTEST_DIR}/include -c dbctest.cpp 
	g++ -isystem ${PWD}/${GTEST_DIR}/include -pthread dbctest.o dbc.o \
		${PWD}/${GTEST_DIR}/src/gtest_main.cc libgtest.a \
		-lsqlite3 -o mmetests

# This is the March Madness Engine builder
mme: predictor.o dbc.o
	g++ ${C_FLAGS} -o mme predictor.o

predictor.o:
	g++ -o predictor.o -c basicPredictor.cpp

dbc.o: dbc.cpp dbc.h
	g++ ${C_FLAGS} -c dbc.cpp

# Destroy object files
clean:
	rm -f ${CLEAN_FILES}


