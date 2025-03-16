##
# Copyright 2009-2010 Jakob Westhoff. All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#    1. Redistributions of source code must retain the above copyright notice,
#       this list of conditions and the following disclaimer.
# 
#    2. Redistributions in binary form must reproduce the above copyright notice,
#       this list of conditions and the following disclaimer in the documentation
#       and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY JAKOB WESTHOFF ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
# EVENT SHALL JAKOB WESTHOFF OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# The views and conclusions contained in the software and documentation are those
# of the authors and should not be interpreted as representing official policies,
# either expressed or implied, of Jakob Westhoff
##

include(CMakeParseArguments)

# Updated include statement with correct file name
include(${CMAKE_CURRENT_LIST_DIR}/FindVala.cmake)

##
# Compile vala files to their c equivalents for further processing. 
#
# The "vala_precompile" macro takes care of calling the valac executable on the
# given source to produce c files which can then be processed further using
# default cmake functions.
# 
# The first parameter provided is a variable, which will be filled with a list
# of c files outputted by the vala compiler. This list can than be used in
# conjunction with functions like "add_executable" or others to create the
# necessary compile rules with CMake.
# 
# The initial variable is followed by a list of .vala files to be compiled.
# Please take care to add every vala file belonging to the currently compiled
# project or library as Vala will otherwise not be able to resolve all
# dependencies.
# 
# The following sections may be specified afterwards to provide certain options
# to the vala compiler:
# 
# PACKAGES
#   A list of vala packages/libraries to be used during the compile cycle. The
#   package names are exactly the same, as they would be passed to the valac
#   "--pkg=" option.
# 
# OPTIONS
#   A list of optional options to be passed to the valac executable. This can be
#   used to pass "--thread" for example.
#
# CUSTOM_VAPIS
#   A list of custom vapi files to be included for compilation. This can be
#   useful to include freshly created vala libraries without having to install
#   them in the system.
#
# GENERATE_VAPI
#   Pass all the needed flags to the compiler to create an internal vapi for
#   the compiled library. The provided name will be used for this and a
#   <provided_name>.vapi file will be created.
# 
# GENERATE_HEADER
#   Let the compiler generate a header file for the compiled code. There will
#   be a header file as well as an internal header file being generated called
#   <provided_name>.h and <provided_name>_internal.h
#
# GENERATE_GIR
#   Have the compiler generate a GObject-Introspection repository file with
#   name: <provided_name>.gir. This can be later used to create a binary
#   typelib using the GI compiler.
#
# GENERATE_SYMBOLS
#   Output a <provided_name>.symbols file containing all the exported symbols.
# 
# The following call is a simple example to the vala_precompile macro showing
# an example to every of the optional sections:
#
#   vala_precompile(VALA_C mytarget
#       source1.vala
#       source2.vala
#       source3.vala
#   PACKAGES
#       gtk+-2.0
#       gio-1.0
#       posix
#   DIRECTORY
#       gen
#   OPTIONS
#       --thread
#   CUSTOM_VAPIS
#       some_vapi.vapi
#   GENERATE_VAPI
#       myvapi
#   GENERATE_HEADER
#       myheader
#   GENERATE_GIR
#       mygir
#   GENERATE_SYMBOLS
#       mysymbols
#   )
#
# Most important is the variable VALA_C which will contain all the generated c
# file names after the call.
##

macro(vala_precompile output target_name)
    cmake_parse_arguments(ARGS 
        "" 
        "DIRECTORY;GENERATE_HEADER;GENERATE_VAPI;GENERATE_GIR;GENERATE_SYMBOLS" 
        "PACKAGES;OPTIONS;CUSTOM_VAPIS" 
        ${ARGN})

    if(ARGS_DIRECTORY)
        set(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${ARGS_DIRECTORY})
    else(ARGS_DIRECTORY)
        set(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endif(ARGS_DIRECTORY)
    include_directories(${DIRECTORY})
    set(vala_pkg_opts "")
    foreach(pkg ${ARGS_PACKAGES})
        list(APPEND vala_pkg_opts "--pkg=${pkg}")
    endforeach(pkg ${ARGS_PACKAGES})
    set(in_files "")
    set(out_files "")
    set(${output} "")
    set(vapi_files "")
    set(gir_files "")
    set(header_files "")
    set(symbols_files "")
    foreach(file ${ARGS_UNPARSED_ARGUMENTS})
        list(APPEND in_files "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
        string(REPLACE ".vala" ".c" src_file "${file}")
        string(REPLACE ".gs" ".c" src_file "${src_file}")
        set(out_file "${DIRECTORY}/${src_file}")
        list(APPEND out_files "${DIRECTORY}/${src_file}")
        list(APPEND ${output} ${out_file})
    endforeach(file ${ARGS_UNPARSED_ARGUMENTS})

    set(custom_vapi_arguments "")
    if(ARGS_CUSTOM_VAPIS)
        # Copy the vapi files to the build directory, as they can include with
        # relative paths.
        foreach(vapi ${ARGS_CUSTOM_VAPIS})
            file(MAKE_DIRECTORY "${DIRECTORY}")
            file(COPY ${vapi} DESTINATION ${DIRECTORY})
            
            # The vapi file might be generated from METADATA
            string(REGEX REPLACE "^.*/" "" vapi_file "${vapi}")
            list(APPEND custom_vapi_arguments "${DIRECTORY}/${vapi_file}")
        endforeach(vapi ${ARGS_CUSTOM_VAPIS})
    endif(ARGS_CUSTOM_VAPIS)

    set(vapi_arguments "")
    if(ARGS_GENERATE_VAPI)
        list(APPEND out_files "${DIRECTORY}/${ARGS_GENERATE_VAPI}.vapi")
        list(APPEND vapi_arguments "--library=${ARGS_GENERATE_VAPI}")
        list(APPEND vapi_arguments "${DIRECTORY}/${ARGS_GENERATE_VAPI}.vapi")
        list(APPEND vapi_files "${DIRECTORY}/${ARGS_GENERATE_VAPI}.vapi")
    endif(ARGS_GENERATE_VAPI)

    set(gir_arguments "")
    if(ARGS_GENERATE_GIR)
        list(APPEND out_files "${DIRECTORY}/${ARGS_GENERATE_GIR}.gir")
        list(APPEND gir_arguments "--gir=${ARGS_GENERATE_GIR}.gir")
        list(APPEND gir_files "${DIRECTORY}/${ARGS_GENERATE_GIR}.gir")
    endif(ARGS_GENERATE_GIR)

    set(header_arguments "")
    if(ARGS_GENERATE_HEADER)
        list(APPEND out_files "${DIRECTORY}/${ARGS_GENERATE_HEADER}.h")
        list(APPEND header_arguments "--header=${DIRECTORY}/${ARGS_GENERATE_HEADER}.h")
        list(APPEND header_files "${DIRECTORY}/${ARGS_GENERATE_HEADER}.h")
    endif(ARGS_GENERATE_HEADER)

    set(symbols_arguments "")
    if(ARGS_GENERATE_SYMBOLS)
        list(APPEND out_files "${DIRECTORY}/${ARGS_GENERATE_SYMBOLS}.symbols")
        list(APPEND symbols_arguments "--symbols=${DIRECTORY}/${ARGS_GENERATE_SYMBOLS}.symbols")
        list(APPEND symbols_files "${DIRECTORY}/${ARGS_GENERATE_SYMBOLS}.symbols")
    endif(ARGS_GENERATE_SYMBOLS)

    # Create a command that will compile the vala files.
    add_custom_command(OUTPUT ${out_files} 
    COMMAND 
        ${VALA_EXECUTABLE} 
    ARGS 
        "-C" 
        ${header_arguments} 
        ${vapi_arguments} 
        ${gir_arguments} 
        ${symbols_arguments} 
        "-d" ${DIRECTORY} 
        ${vala_pkg_opts} 
        ${ARGS_OPTIONS} 
        ${in_files} 
        ${custom_vapi_arguments}
    DEPENDS 
        ${in_files} 
        ${ARGS_CUSTOM_VAPIS}
    )

    # Use a single target for all Vala files instead of one per file
    add_custom_target(${target_name} DEPENDS ${out_files})

    # Add the output files to the specified variable.
    set(${output} ${out_files})
endmacro(vala_precompile)