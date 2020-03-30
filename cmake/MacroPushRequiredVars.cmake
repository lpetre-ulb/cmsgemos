# Copied from Kexi on 2019-03-18.
#
# https://github.com/KDE/kexi/blob/master/cmake/modules/MacroPushRequiredVars.cmake
#
# Original contents of the COPYING-CMAKE-SCRIPTS file:
#
# -----------------------------------------------------------------------------
# Copyright (c) The Regents of the University of California.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the University nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
# -----------------------------------------------------------------------------

# this module defines two macros:
# MACRO_PUSH_REQUIRED_VARS()
# and
# MACRO_POP_REQUIRED_VARS()
# use these if you call cmake macros which use
# any of the CMAKE_REQUIRED_XXX variables
#
# Usage:
# MACRO_PUSH_REQUIRED_VARS()
# SET(CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS} -DSOME_MORE_DEF)
# CHECK_FUNCTION_EXISTS(...)
# MACRO_POP_REQUIRED_VARS()

# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

macro(MACRO_PUSH_REQUIRED_VARS)

   if(NOT DEFINED _PUSH_REQUIRED_VARS_COUNTER)
      set(_PUSH_REQUIRED_VARS_COUNTER 0)
   endif()

   math(EXPR _PUSH_REQUIRED_VARS_COUNTER "${_PUSH_REQUIRED_VARS_COUNTER}+1")

   if(DEFINED CMAKE_REQUIRED_INCLUDES)
      set(_CMAKE_REQUIRED_INCLUDES_SAVE_${_PUSH_REQUIRED_VARS_COUNTER}    ${CMAKE_REQUIRED_INCLUDES})
   endif()
   if(DEFINED CMAKE_REQUIRED_DEFINITIONS)
      set(_CMAKE_REQUIRED_DEFINITIONS_SAVE_${_PUSH_REQUIRED_VARS_COUNTER} ${CMAKE_REQUIRED_DEFINITIONS})
   endif()
   if(DEFINED CMAKE_REQUIRED_LIBRARIES)
      set(_CMAKE_REQUIRED_LIBRARIES_SAVE_${_PUSH_REQUIRED_VARS_COUNTER}   ${CMAKE_REQUIRED_LIBRARIES})
   endif()
   if(DEFINED CMAKE_REQUIRED_FLAGS)
      set(_CMAKE_REQUIRED_FLAGS_SAVE_${_PUSH_REQUIRED_VARS_COUNTER}       ${CMAKE_REQUIRED_FLAGS})
   endif()
endmacro(MACRO_PUSH_REQUIRED_VARS)

macro(MACRO_POP_REQUIRED_VARS)

# don't pop more than we pushed
   if("${_PUSH_REQUIRED_VARS_COUNTER}" GREATER "0")
      if(DEFINED _CMAKE_REQUIRED_INCLUDES_SAVE_${_PUSH_REQUIRED_VARS_COUNTER})
         set(CMAKE_REQUIRED_INCLUDES    ${_CMAKE_REQUIRED_INCLUDES_SAVE_${_PUSH_REQUIRED_VARS_COUNTER}})
      endif()
      if(DEFINED _CMAKE_REQUIRED_DEFINITIONS_SAVE_${_PUSH_REQUIRED_VARS_COUNTER})
         set(CMAKE_REQUIRED_DEFINITIONS ${_CMAKE_REQUIRED_DEFINITIONS_SAVE_${_PUSH_REQUIRED_VARS_COUNTER}})
      endif()
      if(DEFINED _CMAKE_REQUIRED_LIBRARIES_SAVE_${_PUSH_REQUIRED_VARS_COUNTER})
         set(CMAKE_REQUIRED_LIBRARIES   ${_CMAKE_REQUIRED_LIBRARIES_SAVE_${_PUSH_REQUIRED_VARS_COUNTER}})
      endif()
      if(DEFINED _CMAKE_REQUIRED_FLAGS_SAVE_${_PUSH_REQUIRED_VARS_COUNTER})
         set(CMAKE_REQUIRED_FLAGS       ${_CMAKE_REQUIRED_FLAGS_SAVE_${_PUSH_REQUIRED_VARS_COUNTER}})
      endif()

      math(EXPR _PUSH_REQUIRED_VARS_COUNTER "${_PUSH_REQUIRED_VARS_COUNTER}-1")
   endif()

endmacro(MACRO_POP_REQUIRED_VARS)

