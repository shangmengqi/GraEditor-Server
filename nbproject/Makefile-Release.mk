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
CC=clang
CCC=clang++
CXX=clang++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=CLang-Linux
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
	${OBJECTDIR}/Control/ControlLayer.o \
	${OBJECTDIR}/Model/ModelLayer.o \
	${OBJECTDIR}/Module/CooperationModule.o \
	${OBJECTDIR}/Module/VersionModule.o \
	${OBJECTDIR}/Net/NetLayer.o \
	${OBJECTDIR}/main.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/graeditor-server

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/graeditor-server: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/graeditor-server ${OBJECTFILES} ${LDLIBSOPTIONS} -levent -I Thirdpart/sdbdriver/include -L Thirdpart/sdbdriver/lib -lsdbcpp

${OBJECTDIR}/Control/ControlLayer.o: Control/ControlLayer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Control
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -IThirdpart/sdbdriver/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Control/ControlLayer.o Control/ControlLayer.cpp

${OBJECTDIR}/Model/ModelLayer.o: Model/ModelLayer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Model
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -IThirdpart/sdbdriver/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Model/ModelLayer.o Model/ModelLayer.cpp

${OBJECTDIR}/Module/CooperationModule.o: Module/CooperationModule.cpp 
	${MKDIR} -p ${OBJECTDIR}/Module
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -IThirdpart/sdbdriver/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Module/CooperationModule.o Module/CooperationModule.cpp

${OBJECTDIR}/Module/VersionModule.o: Module/VersionModule.cpp 
	${MKDIR} -p ${OBJECTDIR}/Module
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -IThirdpart/sdbdriver/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Module/VersionModule.o Module/VersionModule.cpp

${OBJECTDIR}/Net/NetLayer.o: Net/NetLayer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Net
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -IThirdpart/sdbdriver/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Net/NetLayer.o Net/NetLayer.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -IThirdpart/sdbdriver/include -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/graeditor-server

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
