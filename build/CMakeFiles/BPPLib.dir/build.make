# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/koala/Documents/brandons-pi-project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/koala/Documents/brandons-pi-project/build

# Include any dependencies generated for this target.
include CMakeFiles/BPPLib.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/BPPLib.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/BPPLib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/BPPLib.dir/flags.make

CMakeFiles/BPPLib.dir/src/moduli-set.cpp.o: CMakeFiles/BPPLib.dir/flags.make
CMakeFiles/BPPLib.dir/src/moduli-set.cpp.o: /home/koala/Documents/brandons-pi-project/src/moduli-set.cpp
CMakeFiles/BPPLib.dir/src/moduli-set.cpp.o: CMakeFiles/BPPLib.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/koala/Documents/brandons-pi-project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/BPPLib.dir/src/moduli-set.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/BPPLib.dir/src/moduli-set.cpp.o -MF CMakeFiles/BPPLib.dir/src/moduli-set.cpp.o.d -o CMakeFiles/BPPLib.dir/src/moduli-set.cpp.o -c /home/koala/Documents/brandons-pi-project/src/moduli-set.cpp

CMakeFiles/BPPLib.dir/src/moduli-set.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BPPLib.dir/src/moduli-set.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/koala/Documents/brandons-pi-project/src/moduli-set.cpp > CMakeFiles/BPPLib.dir/src/moduli-set.cpp.i

CMakeFiles/BPPLib.dir/src/moduli-set.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BPPLib.dir/src/moduli-set.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/koala/Documents/brandons-pi-project/src/moduli-set.cpp -o CMakeFiles/BPPLib.dir/src/moduli-set.cpp.s

CMakeFiles/BPPLib.dir/src/residue-int.cpp.o: CMakeFiles/BPPLib.dir/flags.make
CMakeFiles/BPPLib.dir/src/residue-int.cpp.o: /home/koala/Documents/brandons-pi-project/src/residue-int.cpp
CMakeFiles/BPPLib.dir/src/residue-int.cpp.o: CMakeFiles/BPPLib.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/koala/Documents/brandons-pi-project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/BPPLib.dir/src/residue-int.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/BPPLib.dir/src/residue-int.cpp.o -MF CMakeFiles/BPPLib.dir/src/residue-int.cpp.o.d -o CMakeFiles/BPPLib.dir/src/residue-int.cpp.o -c /home/koala/Documents/brandons-pi-project/src/residue-int.cpp

CMakeFiles/BPPLib.dir/src/residue-int.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BPPLib.dir/src/residue-int.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/koala/Documents/brandons-pi-project/src/residue-int.cpp > CMakeFiles/BPPLib.dir/src/residue-int.cpp.i

CMakeFiles/BPPLib.dir/src/residue-int.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BPPLib.dir/src/residue-int.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/koala/Documents/brandons-pi-project/src/residue-int.cpp -o CMakeFiles/BPPLib.dir/src/residue-int.cpp.s

CMakeFiles/BPPLib.dir/src/chunky-fio.cpp.o: CMakeFiles/BPPLib.dir/flags.make
CMakeFiles/BPPLib.dir/src/chunky-fio.cpp.o: /home/koala/Documents/brandons-pi-project/src/chunky-fio.cpp
CMakeFiles/BPPLib.dir/src/chunky-fio.cpp.o: CMakeFiles/BPPLib.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/koala/Documents/brandons-pi-project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/BPPLib.dir/src/chunky-fio.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/BPPLib.dir/src/chunky-fio.cpp.o -MF CMakeFiles/BPPLib.dir/src/chunky-fio.cpp.o.d -o CMakeFiles/BPPLib.dir/src/chunky-fio.cpp.o -c /home/koala/Documents/brandons-pi-project/src/chunky-fio.cpp

CMakeFiles/BPPLib.dir/src/chunky-fio.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BPPLib.dir/src/chunky-fio.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/koala/Documents/brandons-pi-project/src/chunky-fio.cpp > CMakeFiles/BPPLib.dir/src/chunky-fio.cpp.i

CMakeFiles/BPPLib.dir/src/chunky-fio.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BPPLib.dir/src/chunky-fio.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/koala/Documents/brandons-pi-project/src/chunky-fio.cpp -o CMakeFiles/BPPLib.dir/src/chunky-fio.cpp.s

CMakeFiles/BPPLib.dir/src/debug.cpp.o: CMakeFiles/BPPLib.dir/flags.make
CMakeFiles/BPPLib.dir/src/debug.cpp.o: /home/koala/Documents/brandons-pi-project/src/debug.cpp
CMakeFiles/BPPLib.dir/src/debug.cpp.o: CMakeFiles/BPPLib.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/koala/Documents/brandons-pi-project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/BPPLib.dir/src/debug.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/BPPLib.dir/src/debug.cpp.o -MF CMakeFiles/BPPLib.dir/src/debug.cpp.o.d -o CMakeFiles/BPPLib.dir/src/debug.cpp.o -c /home/koala/Documents/brandons-pi-project/src/debug.cpp

CMakeFiles/BPPLib.dir/src/debug.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BPPLib.dir/src/debug.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/koala/Documents/brandons-pi-project/src/debug.cpp > CMakeFiles/BPPLib.dir/src/debug.cpp.i

CMakeFiles/BPPLib.dir/src/debug.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BPPLib.dir/src/debug.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/koala/Documents/brandons-pi-project/src/debug.cpp -o CMakeFiles/BPPLib.dir/src/debug.cpp.s

# Object files for target BPPLib
BPPLib_OBJECTS = \
"CMakeFiles/BPPLib.dir/src/moduli-set.cpp.o" \
"CMakeFiles/BPPLib.dir/src/residue-int.cpp.o" \
"CMakeFiles/BPPLib.dir/src/chunky-fio.cpp.o" \
"CMakeFiles/BPPLib.dir/src/debug.cpp.o"

# External object files for target BPPLib
BPPLib_EXTERNAL_OBJECTS =

libBPPLib.a: CMakeFiles/BPPLib.dir/src/moduli-set.cpp.o
libBPPLib.a: CMakeFiles/BPPLib.dir/src/residue-int.cpp.o
libBPPLib.a: CMakeFiles/BPPLib.dir/src/chunky-fio.cpp.o
libBPPLib.a: CMakeFiles/BPPLib.dir/src/debug.cpp.o
libBPPLib.a: CMakeFiles/BPPLib.dir/build.make
libBPPLib.a: CMakeFiles/BPPLib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/koala/Documents/brandons-pi-project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX static library libBPPLib.a"
	$(CMAKE_COMMAND) -P CMakeFiles/BPPLib.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/BPPLib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/BPPLib.dir/build: libBPPLib.a
.PHONY : CMakeFiles/BPPLib.dir/build

CMakeFiles/BPPLib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/BPPLib.dir/cmake_clean.cmake
.PHONY : CMakeFiles/BPPLib.dir/clean

CMakeFiles/BPPLib.dir/depend:
	cd /home/koala/Documents/brandons-pi-project/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/koala/Documents/brandons-pi-project /home/koala/Documents/brandons-pi-project /home/koala/Documents/brandons-pi-project/build /home/koala/Documents/brandons-pi-project/build /home/koala/Documents/brandons-pi-project/build/CMakeFiles/BPPLib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/BPPLib.dir/depend
