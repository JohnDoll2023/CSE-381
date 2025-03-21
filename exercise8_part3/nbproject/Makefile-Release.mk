#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/exercise8_starter_code/ChildProcess.o \
	${OBJECTDIR}/exercise8_starter_code/HTTPFile.o \
	${OBJECTDIR}/exercise8_starter_code/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk exercise8_part3_opt

exercise8_part3_opt: ${OBJECTFILES}
	${LINK.cc} -o exercise8_part3_opt ${OBJECTFILES} ${LDLIBSOPTIONS} -lboost_system -lpthread -lmysqlpp

${OBJECTDIR}/exercise8_starter_code/ChildProcess.o: exercise8_starter_code/ChildProcess.cpp
	${MKDIR} -p ${OBJECTDIR}/exercise8_starter_code
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/exercise8_starter_code/ChildProcess.o exercise8_starter_code/ChildProcess.cpp

${OBJECTDIR}/exercise8_starter_code/HTTPFile.o: exercise8_starter_code/HTTPFile.cpp
	${MKDIR} -p ${OBJECTDIR}/exercise8_starter_code
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/exercise8_starter_code/HTTPFile.o exercise8_starter_code/HTTPFile.cpp

${OBJECTDIR}/exercise8_starter_code/main.o: exercise8_starter_code/main.cpp
	${MKDIR} -p ${OBJECTDIR}/exercise8_starter_code
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/exercise8_starter_code/main.o exercise8_starter_code/main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
