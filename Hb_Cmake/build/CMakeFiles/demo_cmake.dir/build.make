# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/work/github/HbSrc/Hb_Cmake

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/work/github/HbSrc/Hb_Cmake/build

# Include any dependencies generated for this target.
include CMakeFiles/demo_cmake.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/demo_cmake.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/demo_cmake.dir/flags.make

CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.o: CMakeFiles/demo_cmake.dir/flags.make
CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.o: ../src/LinuxLog.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/work/github/HbSrc/Hb_Cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.o"
	/usr/bin/aarch64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.o -c /home/work/github/HbSrc/Hb_Cmake/src/LinuxLog.cpp

CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.i"
	/usr/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/work/github/HbSrc/Hb_Cmake/src/LinuxLog.cpp > CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.i

CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.s"
	/usr/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/work/github/HbSrc/Hb_Cmake/src/LinuxLog.cpp -o CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.s

CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.o.requires:

.PHONY : CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.o.requires

CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.o.provides: CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.o.requires
	$(MAKE) -f CMakeFiles/demo_cmake.dir/build.make CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.o.provides.build
.PHONY : CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.o.provides

CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.o.provides.build: CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.o


CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.o: CMakeFiles/demo_cmake.dir/flags.make
CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.o: ../src/MyUDP.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/work/github/HbSrc/Hb_Cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.o"
	/usr/bin/aarch64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.o -c /home/work/github/HbSrc/Hb_Cmake/src/MyUDP.cpp

CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.i"
	/usr/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/work/github/HbSrc/Hb_Cmake/src/MyUDP.cpp > CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.i

CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.s"
	/usr/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/work/github/HbSrc/Hb_Cmake/src/MyUDP.cpp -o CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.s

CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.o.requires:

.PHONY : CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.o.requires

CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.o.provides: CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.o.requires
	$(MAKE) -f CMakeFiles/demo_cmake.dir/build.make CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.o.provides.build
.PHONY : CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.o.provides

CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.o.provides.build: CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.o


CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.o: CMakeFiles/demo_cmake.dir/flags.make
CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.o: ../src/UdpFunc.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/work/github/HbSrc/Hb_Cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.o"
	/usr/bin/aarch64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.o -c /home/work/github/HbSrc/Hb_Cmake/src/UdpFunc.cpp

CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.i"
	/usr/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/work/github/HbSrc/Hb_Cmake/src/UdpFunc.cpp > CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.i

CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.s"
	/usr/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/work/github/HbSrc/Hb_Cmake/src/UdpFunc.cpp -o CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.s

CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.o.requires:

.PHONY : CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.o.requires

CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.o.provides: CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.o.requires
	$(MAKE) -f CMakeFiles/demo_cmake.dir/build.make CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.o.provides.build
.PHONY : CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.o.provides

CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.o.provides.build: CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.o


CMakeFiles/demo_cmake.dir/src/main.cpp.o: CMakeFiles/demo_cmake.dir/flags.make
CMakeFiles/demo_cmake.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/work/github/HbSrc/Hb_Cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/demo_cmake.dir/src/main.cpp.o"
	/usr/bin/aarch64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/demo_cmake.dir/src/main.cpp.o -c /home/work/github/HbSrc/Hb_Cmake/src/main.cpp

CMakeFiles/demo_cmake.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/demo_cmake.dir/src/main.cpp.i"
	/usr/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/work/github/HbSrc/Hb_Cmake/src/main.cpp > CMakeFiles/demo_cmake.dir/src/main.cpp.i

CMakeFiles/demo_cmake.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/demo_cmake.dir/src/main.cpp.s"
	/usr/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/work/github/HbSrc/Hb_Cmake/src/main.cpp -o CMakeFiles/demo_cmake.dir/src/main.cpp.s

CMakeFiles/demo_cmake.dir/src/main.cpp.o.requires:

.PHONY : CMakeFiles/demo_cmake.dir/src/main.cpp.o.requires

CMakeFiles/demo_cmake.dir/src/main.cpp.o.provides: CMakeFiles/demo_cmake.dir/src/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/demo_cmake.dir/build.make CMakeFiles/demo_cmake.dir/src/main.cpp.o.provides.build
.PHONY : CMakeFiles/demo_cmake.dir/src/main.cpp.o.provides

CMakeFiles/demo_cmake.dir/src/main.cpp.o.provides.build: CMakeFiles/demo_cmake.dir/src/main.cpp.o


CMakeFiles/demo_cmake.dir/src/thread.cpp.o: CMakeFiles/demo_cmake.dir/flags.make
CMakeFiles/demo_cmake.dir/src/thread.cpp.o: ../src/thread.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/work/github/HbSrc/Hb_Cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/demo_cmake.dir/src/thread.cpp.o"
	/usr/bin/aarch64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/demo_cmake.dir/src/thread.cpp.o -c /home/work/github/HbSrc/Hb_Cmake/src/thread.cpp

CMakeFiles/demo_cmake.dir/src/thread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/demo_cmake.dir/src/thread.cpp.i"
	/usr/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/work/github/HbSrc/Hb_Cmake/src/thread.cpp > CMakeFiles/demo_cmake.dir/src/thread.cpp.i

CMakeFiles/demo_cmake.dir/src/thread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/demo_cmake.dir/src/thread.cpp.s"
	/usr/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/work/github/HbSrc/Hb_Cmake/src/thread.cpp -o CMakeFiles/demo_cmake.dir/src/thread.cpp.s

CMakeFiles/demo_cmake.dir/src/thread.cpp.o.requires:

.PHONY : CMakeFiles/demo_cmake.dir/src/thread.cpp.o.requires

CMakeFiles/demo_cmake.dir/src/thread.cpp.o.provides: CMakeFiles/demo_cmake.dir/src/thread.cpp.o.requires
	$(MAKE) -f CMakeFiles/demo_cmake.dir/build.make CMakeFiles/demo_cmake.dir/src/thread.cpp.o.provides.build
.PHONY : CMakeFiles/demo_cmake.dir/src/thread.cpp.o.provides

CMakeFiles/demo_cmake.dir/src/thread.cpp.o.provides.build: CMakeFiles/demo_cmake.dir/src/thread.cpp.o


# Object files for target demo_cmake
demo_cmake_OBJECTS = \
"CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.o" \
"CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.o" \
"CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.o" \
"CMakeFiles/demo_cmake.dir/src/main.cpp.o" \
"CMakeFiles/demo_cmake.dir/src/thread.cpp.o"

# External object files for target demo_cmake
demo_cmake_EXTERNAL_OBJECTS =

../bin/demo_cmake: CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.o
../bin/demo_cmake: CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.o
../bin/demo_cmake: CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.o
../bin/demo_cmake: CMakeFiles/demo_cmake.dir/src/main.cpp.o
../bin/demo_cmake: CMakeFiles/demo_cmake.dir/src/thread.cpp.o
../bin/demo_cmake: CMakeFiles/demo_cmake.dir/build.make
../bin/demo_cmake: CMakeFiles/demo_cmake.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/work/github/HbSrc/Hb_Cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable ../bin/demo_cmake"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/demo_cmake.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/demo_cmake.dir/build: ../bin/demo_cmake

.PHONY : CMakeFiles/demo_cmake.dir/build

CMakeFiles/demo_cmake.dir/requires: CMakeFiles/demo_cmake.dir/src/LinuxLog.cpp.o.requires
CMakeFiles/demo_cmake.dir/requires: CMakeFiles/demo_cmake.dir/src/MyUDP.cpp.o.requires
CMakeFiles/demo_cmake.dir/requires: CMakeFiles/demo_cmake.dir/src/UdpFunc.cpp.o.requires
CMakeFiles/demo_cmake.dir/requires: CMakeFiles/demo_cmake.dir/src/main.cpp.o.requires
CMakeFiles/demo_cmake.dir/requires: CMakeFiles/demo_cmake.dir/src/thread.cpp.o.requires

.PHONY : CMakeFiles/demo_cmake.dir/requires

CMakeFiles/demo_cmake.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/demo_cmake.dir/cmake_clean.cmake
.PHONY : CMakeFiles/demo_cmake.dir/clean

CMakeFiles/demo_cmake.dir/depend:
	cd /home/work/github/HbSrc/Hb_Cmake/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/work/github/HbSrc/Hb_Cmake /home/work/github/HbSrc/Hb_Cmake /home/work/github/HbSrc/Hb_Cmake/build /home/work/github/HbSrc/Hb_Cmake/build /home/work/github/HbSrc/Hb_Cmake/build/CMakeFiles/demo_cmake.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/demo_cmake.dir/depend

