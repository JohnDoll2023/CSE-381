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
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/homework10_starter_code/ChildProcess.o \
	${OBJECTDIR}/homework10_starter_code/HTTPFile.o \
	${OBJECTDIR}/homework10_starter_code/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-fsanitize=address -DGNUCXX_DEBUG
CXXFLAGS=-fsanitize=address -DGNUCXX_DEBUG

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk Homework10

Homework10: ${OBJECTFILES}
	${LINK.cc} -o Homework10 ${OBJECTFILES} ${LDLIBSOPTIONS} -lboost_system -lpthread -lmysqlpp

${OBJECTDIR}/homework10_starter_code/ChildProcess.o: homework10_starter_code/ChildProcess.cpp
	${MKDIR} -p ${OBJECTDIR}/homework10_starter_code
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/homework10_starter_code/ChildProcess.o homework10_starter_code/ChildProcess.cpp

${OBJECTDIR}/homework10_starter_code/HTTPFile.o: homework10_starter_code/HTTPFile.cpp
	${MKDIR} -p ${OBJECTDIR}/homework10_starter_code
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/homework10_starter_code/HTTPFile.o homework10_starter_code/HTTPFile.cpp

${OBJECTDIR}/homework10_starter_code/main.o: homework10_starter_code/main.cpp
	${MKDIR} -p ${OBJECTDIR}/homework10_starter_code
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/homework10_starter_code/main.o homework10_starter_code/main.cpp

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