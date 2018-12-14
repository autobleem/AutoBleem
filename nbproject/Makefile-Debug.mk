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
CC=gcc-8
CCC=g++-8
CXX=g++-8
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/code/database.o \
	${OBJECTDIR}/src/code/filesystem.o \
	${OBJECTDIR}/src/code/main.o \
	${OBJECTDIR}/src/code/sqlite3.o


# C Compiler Flags
CFLAGS=-DSQLITE_OMIT_LOAD_EXTENSION

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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cbleemsync

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cbleemsync: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cbleemsync ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/code/database.o: src/code/database.c
	${MKDIR} -p ${OBJECTDIR}/src/code
	${RM} "$@.d"
	$(COMPILE.c) -g -Isrc/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/code/database.o src/code/database.c

${OBJECTDIR}/src/code/filesystem.o: src/code/filesystem.c
	${MKDIR} -p ${OBJECTDIR}/src/code
	${RM} "$@.d"
	$(COMPILE.c) -g -Isrc/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/code/filesystem.o src/code/filesystem.c

${OBJECTDIR}/src/code/main.o: src/code/main.c
	${MKDIR} -p ${OBJECTDIR}/src/code
	${RM} "$@.d"
	$(COMPILE.c) -g -Isrc/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/code/main.o src/code/main.c

${OBJECTDIR}/src/code/sqlite3.o: src/code/sqlite3.c
	${MKDIR} -p ${OBJECTDIR}/src/code
	${RM} "$@.d"
	$(COMPILE.c) -g -Isrc/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/code/sqlite3.o src/code/sqlite3.c

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
