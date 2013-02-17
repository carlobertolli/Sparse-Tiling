
#
# The following environment variables are required:
#
ST_COMPILER     = gnu
ST_INSTALL_PATH = ..
#

#
# compiler settings
#

ST_OP2 = $(ST_INSTALL_PATH)/support/
ST_OP2_INC = $(ST_OP2)/include
ST_OP2_SRC = $(ST_OP2)/src
ST_OP2_OBJ = $(ST_OP2)/obj
ST_OP2_LIB = $(ST_OP2)/lib

# Default to PGI compiler
ST_COMPILER ?= gcc

# Common definitions
# Uncomment -g to get debugging info on board
DEBUG_FLAGS = -g #-O2 

#  -Werror

# All compilers define CC and FC plus compiler-specific macros
ifeq ($(ST_COMPILER),gnu)
	F_INC_MOD       = $(F_OP2_MOD)/gnu
	CC              = gcc
	CFLAGS          = -std=c99 -Wall -fPIC -pedantic -pipe $(DEBUG_FLAGS)
	FC              = gfortran
	FFLAGS          = -J$(F_INC_MOD) -Wall -pedantic -pipe $(DEBUG_FLAGS)
	MPICC			= $(CC) -I$(MPI_INC) -L$(MPI_LIB)
	MPIF90			= $(FC) -I$(MPI_INC) -L$(MPI_LIB)
	NO_MPI_MACRO    = -DNO_MPI
else
ifeq ($(ST_COMPILER),intel)
	F_INC_MOD       = $(F_OP2_MOD)/intel
	CC              = icc
	CFLAGS          = -std=c99 -Wall -Werror $(DEBUG_FLAGS)
	FC              = ifort
	FFLAGS          = -module $(F_INC_MOD) $(DEBUG_FLAGS)
	MPICC			= mpicc
	MPIF90			= mpif90
	NO_MPI_MACRO    = -D NO_MPI
else
print:
	@echo "unrecognised value for ST_COMPILER"
endif
endif

ifeq ($(VTK), ON)
  VTKCOMP = -DVTK_ON
endif

.PHONY: clean directories

ALLTARGETS      = st-op2

all: $(ALLTARGETS)

directories:
	mkdir -p $(ST_OP2_LIB)
	mkdir -p $(ST_OP2_OBJ)

# does not depend on reference.h because we don't need the op_par_loop templated
# implementation, but our own implementation
st-op2: $(ST_OP2_SRC)/invert.c $(ST_OP2_SRC)/inspector.c $(ST_OP2_SRC)/tile.c $(ST_OP2_SRC)/executor.c\
	directories
	$(CC) $(CFLAGS) $(VTKCOMP) -I$(ST_OP2_INC) -c $(ST_OP2_SRC)/invert.c $(ST_OP2_SRC)/tile.c $(ST_OP2_SRC)/inspector.c $(ST_OP2_SRC)/executor.c $(ST_OP2_SRC)/plotmesh.c
	mv *.o $(ST_OP2_OBJ)/
	ar -r $(ST_OP2_LIB)/libst_op2.a $(ST_OP2_OBJ)/invert.o $(ST_OP2_OBJ)/inspector.o $(ST_OP2_OBJ)/executor.o $(ST_OP2_OBJ)/tile.o $(ST_OP2_OBJ)/plotmesh.o

clean:
	-rm -if $(ST_OP2_OBJ)/*.o
	-rm -if $(ST_OP2_LIB)/*.a
