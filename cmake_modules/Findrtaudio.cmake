#
#  This file is part of openauto project.
#  Copyright (C) 2018 f1x.studio (Michal Szwaj)
#
#  openauto is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.

#  openauto is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with openauto. If not, see <http://www.gnu.org/licenses/>.
#

if (RTAUDIO_LIBRARIES AND RTAUDIO_INCLUDE_DIRS)
  # in cache already
  set(RTAUDIO_FOUND TRUE)
else (RTAUDIO_LIBRARIES AND RTAUDIO_INCLUDE_DIRS)
  find_path(RTAUDIO_INCLUDE_DIR
    NAMES
        RtAudio.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
	PATH_SUFFIXES
          rtaudio
  )

  find_library(RTAUDIO_LIBRARY
    NAMES
      rtaudio
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  set(RTAUDIO_INCLUDE_DIRS
    ${RTAUDIO_INCLUDE_DIR}
  )
  set(RTAUDIO_LIBRARIES
    ${RTAUDIO_LIBRARY}
)

  if (RTAUDIO_INCLUDE_DIRS AND RTAUDIO_LIBRARIES)
     set(RTAUDIO_FOUND TRUE)
  endif (RTAUDIO_INCLUDE_DIRS AND RTAUDIO_LIBRARIES)

  if (RTAUDIO_FOUND)
    if (NOT rtaudio_FIND_QUIETLY)
      message(STATUS "Found rtaudio:")
          message(STATUS " - Includes: ${RTAUDIO_INCLUDE_DIRS}")
          message(STATUS " - Libraries: ${RTAUDIO_LIBRARIES}")
    endif (NOT rtaudio_FIND_QUIETLY)
  else (RTAUDIO_FOUND)
    if (rtaudio_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find rtaudio")
    endif (rtaudio_FIND_REQUIRED)
  endif (RTAUDIO_FOUND)

    mark_as_advanced(RTAUDIO_INCLUDE_DIRS RTAUDIO_LIBRARIES)

endif (RTAUDIO_LIBRARIES AND RTAUDIO_INCLUDE_DIRS)
