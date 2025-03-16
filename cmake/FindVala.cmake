##
# Copyright 2009-2010 Jakob Westhoff. All rights reserved.
# Copyright 2012 elementary.
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

# Find Vala
##
#
# This module identifies where vala is installed and defines the following
# variables:
#
#   VALA_FOUND       - True if the vala compiler was found
#   VALA_EXECUTABLE  - Path to the vala compiler
#   VALA_VERSION     - The version number of the available vala compiler
#
# If you don't want to install vala in standard directories, or you're not root,
# define VALA_PREFIX, and give that as a hint to find_program().
#

# Search for the vala executable in the usual system paths.
find_program(VALA_EXECUTABLE
  NAMES valac valac-0.56 valac-0.48 valac-0.44 valac-0.42 valac-0.40 valac-0.38 valac-0.36 valac-0.34 valac-0.32 valac-0.30 valac-0.28 valac-0.26 valac-0.24 valac-0.22 valac-0.20 valac-0.18 valac-0.16 valac-0.14 valac-0.12 valac-0.10
  PATHS
    ${VALA_PREFIX}
    /usr/bin
    /usr/local/bin
    /opt/local/bin
    /usr/bin
)

# Handle the QUIETLY and REQUIRED arguments, which may be given to the find call.
# If REQUIRED is specified, make sure that Vala is found or throw an error.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Vala DEFAULT_MSG VALA_EXECUTABLE)

# If Vala is found, determine its version. If we don't do this here, we do this
# every time in vala_precompile.
if(VALA_FOUND)
  execute_process(COMMAND ${VALA_EXECUTABLE} "--version" 
    OUTPUT_VARIABLE VALA_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  # Make the version only the number
  string(SUBSTRING ${VALA_VERSION} 6 -1 VALA_VERSION)
  # Make sure we're using a compatible version
  if(${VALA_VERSION} VERSION_LESS "0.16")
    message(FATAL_ERROR "Vala version not compatible. Found ${VALA_VERSION}, but needed at least 0.16")
  endif()
endif()

# Inform CMake that we've found vala, including where it is.
mark_as_advanced(VALA_EXECUTABLE)
