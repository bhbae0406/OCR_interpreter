# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_SOURCE_DIR = /home/jdominic/ProQuest/OCR_interpreter/classes

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jdominic/ProQuest/OCR_interpreter/classes/build

# Include any dependencies generated for this target.
include CMakeFiles/SegmentPage.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/SegmentPage.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SegmentPage.dir/flags.make

CMakeFiles/SegmentPage.dir/src/main.cpp.o: CMakeFiles/SegmentPage.dir/flags.make
CMakeFiles/SegmentPage.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jdominic/ProQuest/OCR_interpreter/classes/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/SegmentPage.dir/src/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SegmentPage.dir/src/main.cpp.o -c /home/jdominic/ProQuest/OCR_interpreter/classes/src/main.cpp

CMakeFiles/SegmentPage.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SegmentPage.dir/src/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jdominic/ProQuest/OCR_interpreter/classes/src/main.cpp > CMakeFiles/SegmentPage.dir/src/main.cpp.i

CMakeFiles/SegmentPage.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SegmentPage.dir/src/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jdominic/ProQuest/OCR_interpreter/classes/src/main.cpp -o CMakeFiles/SegmentPage.dir/src/main.cpp.s

CMakeFiles/SegmentPage.dir/src/main.cpp.o.requires:

.PHONY : CMakeFiles/SegmentPage.dir/src/main.cpp.o.requires

CMakeFiles/SegmentPage.dir/src/main.cpp.o.provides: CMakeFiles/SegmentPage.dir/src/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/SegmentPage.dir/build.make CMakeFiles/SegmentPage.dir/src/main.cpp.o.provides.build
.PHONY : CMakeFiles/SegmentPage.dir/src/main.cpp.o.provides

CMakeFiles/SegmentPage.dir/src/main.cpp.o.provides.build: CMakeFiles/SegmentPage.dir/src/main.cpp.o


CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.o: CMakeFiles/SegmentPage.dir/flags.make
CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.o: ../src/segment_temp.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jdominic/ProQuest/OCR_interpreter/classes/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.o -c /home/jdominic/ProQuest/OCR_interpreter/classes/src/segment_temp.cpp

CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jdominic/ProQuest/OCR_interpreter/classes/src/segment_temp.cpp > CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.i

CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jdominic/ProQuest/OCR_interpreter/classes/src/segment_temp.cpp -o CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.s

CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.o.requires:

.PHONY : CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.o.requires

CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.o.provides: CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.o.requires
	$(MAKE) -f CMakeFiles/SegmentPage.dir/build.make CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.o.provides.build
.PHONY : CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.o.provides

CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.o.provides.build: CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.o


CMakeFiles/SegmentPage.dir/src/textLine.cpp.o: CMakeFiles/SegmentPage.dir/flags.make
CMakeFiles/SegmentPage.dir/src/textLine.cpp.o: ../src/textLine.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jdominic/ProQuest/OCR_interpreter/classes/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/SegmentPage.dir/src/textLine.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SegmentPage.dir/src/textLine.cpp.o -c /home/jdominic/ProQuest/OCR_interpreter/classes/src/textLine.cpp

CMakeFiles/SegmentPage.dir/src/textLine.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SegmentPage.dir/src/textLine.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jdominic/ProQuest/OCR_interpreter/classes/src/textLine.cpp > CMakeFiles/SegmentPage.dir/src/textLine.cpp.i

CMakeFiles/SegmentPage.dir/src/textLine.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SegmentPage.dir/src/textLine.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jdominic/ProQuest/OCR_interpreter/classes/src/textLine.cpp -o CMakeFiles/SegmentPage.dir/src/textLine.cpp.s

CMakeFiles/SegmentPage.dir/src/textLine.cpp.o.requires:

.PHONY : CMakeFiles/SegmentPage.dir/src/textLine.cpp.o.requires

CMakeFiles/SegmentPage.dir/src/textLine.cpp.o.provides: CMakeFiles/SegmentPage.dir/src/textLine.cpp.o.requires
	$(MAKE) -f CMakeFiles/SegmentPage.dir/build.make CMakeFiles/SegmentPage.dir/src/textLine.cpp.o.provides.build
.PHONY : CMakeFiles/SegmentPage.dir/src/textLine.cpp.o.provides

CMakeFiles/SegmentPage.dir/src/textLine.cpp.o.provides.build: CMakeFiles/SegmentPage.dir/src/textLine.cpp.o


CMakeFiles/SegmentPage.dir/src/block.cpp.o: CMakeFiles/SegmentPage.dir/flags.make
CMakeFiles/SegmentPage.dir/src/block.cpp.o: ../src/block.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jdominic/ProQuest/OCR_interpreter/classes/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/SegmentPage.dir/src/block.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/SegmentPage.dir/src/block.cpp.o -c /home/jdominic/ProQuest/OCR_interpreter/classes/src/block.cpp

CMakeFiles/SegmentPage.dir/src/block.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SegmentPage.dir/src/block.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jdominic/ProQuest/OCR_interpreter/classes/src/block.cpp > CMakeFiles/SegmentPage.dir/src/block.cpp.i

CMakeFiles/SegmentPage.dir/src/block.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SegmentPage.dir/src/block.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jdominic/ProQuest/OCR_interpreter/classes/src/block.cpp -o CMakeFiles/SegmentPage.dir/src/block.cpp.s

CMakeFiles/SegmentPage.dir/src/block.cpp.o.requires:

.PHONY : CMakeFiles/SegmentPage.dir/src/block.cpp.o.requires

CMakeFiles/SegmentPage.dir/src/block.cpp.o.provides: CMakeFiles/SegmentPage.dir/src/block.cpp.o.requires
	$(MAKE) -f CMakeFiles/SegmentPage.dir/build.make CMakeFiles/SegmentPage.dir/src/block.cpp.o.provides.build
.PHONY : CMakeFiles/SegmentPage.dir/src/block.cpp.o.provides

CMakeFiles/SegmentPage.dir/src/block.cpp.o.provides.build: CMakeFiles/SegmentPage.dir/src/block.cpp.o


# Object files for target SegmentPage
SegmentPage_OBJECTS = \
"CMakeFiles/SegmentPage.dir/src/main.cpp.o" \
"CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.o" \
"CMakeFiles/SegmentPage.dir/src/textLine.cpp.o" \
"CMakeFiles/SegmentPage.dir/src/block.cpp.o"

# External object files for target SegmentPage
SegmentPage_EXTERNAL_OBJECTS =

SegmentPage: CMakeFiles/SegmentPage.dir/src/main.cpp.o
SegmentPage: CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.o
SegmentPage: CMakeFiles/SegmentPage.dir/src/textLine.cpp.o
SegmentPage: CMakeFiles/SegmentPage.dir/src/block.cpp.o
SegmentPage: CMakeFiles/SegmentPage.dir/build.make
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_xphoto.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_xobjdetect.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_tracking.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_surface_matching.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_structured_light.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_stereo.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_saliency.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_rgbd.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_reg.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_plot.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_optflow.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_line_descriptor.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_hdf.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_fuzzy.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_dpm.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_dnn.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_datasets.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_ccalib.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_bioinspired.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_bgsegm.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_aruco.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_videostab.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_superres.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_stitching.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_photo.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_text.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_face.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_ximgproc.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_xfeatures2d.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_shape.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_video.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_objdetect.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_calib3d.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_features2d.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_ml.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_highgui.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_videoio.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_imgcodecs.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_imgproc.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_flann.so.3.1.0
SegmentPage: /home/jdominic/anaconda3/lib/libopencv_core.so.3.1.0
SegmentPage: CMakeFiles/SegmentPage.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jdominic/ProQuest/OCR_interpreter/classes/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable SegmentPage"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SegmentPage.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SegmentPage.dir/build: SegmentPage

.PHONY : CMakeFiles/SegmentPage.dir/build

CMakeFiles/SegmentPage.dir/requires: CMakeFiles/SegmentPage.dir/src/main.cpp.o.requires
CMakeFiles/SegmentPage.dir/requires: CMakeFiles/SegmentPage.dir/src/segment_temp.cpp.o.requires
CMakeFiles/SegmentPage.dir/requires: CMakeFiles/SegmentPage.dir/src/textLine.cpp.o.requires
CMakeFiles/SegmentPage.dir/requires: CMakeFiles/SegmentPage.dir/src/block.cpp.o.requires

.PHONY : CMakeFiles/SegmentPage.dir/requires

CMakeFiles/SegmentPage.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/SegmentPage.dir/cmake_clean.cmake
.PHONY : CMakeFiles/SegmentPage.dir/clean

CMakeFiles/SegmentPage.dir/depend:
	cd /home/jdominic/ProQuest/OCR_interpreter/classes/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jdominic/ProQuest/OCR_interpreter/classes /home/jdominic/ProQuest/OCR_interpreter/classes /home/jdominic/ProQuest/OCR_interpreter/classes/build /home/jdominic/ProQuest/OCR_interpreter/classes/build /home/jdominic/ProQuest/OCR_interpreter/classes/build/CMakeFiles/SegmentPage.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SegmentPage.dir/depend

