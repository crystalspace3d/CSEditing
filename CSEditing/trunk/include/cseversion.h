/*
    Copyright (C) 2013 by Jorrit Tyberghein
  
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
  
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __CSE_CSEVERSION_H__
#define __CSE_CSEVERSION_H__

#include <csver.h>

/**\file
 * Crystal Editing Version Information
 */

// *** NOTE ***
// Also update configure.ac, docs/texinfo/version.txi, and
// mk/autoconf/cel.m4 when changing the version number.

/// Major version
#define CSE_VERSION_MAJOR CS_VER_QUOTE(2)
/// Minor version (even are stable versions, odd are development versions)
#define CSE_VERSION_MINOR CS_VER_QUOTE(1)
/// Maintenance release / RC
#define CSE_VERSION_BUILD CS_VER_QUOTE(0)
/// Date of release
#define CSE_RELEASE_DATE  CS_VER_QUOTE(Tue 10-Apr-2011)

/// A complete version number
#define CSE_VERSION_NUMBER CS_VERSION_MAJOR "." CS_VERSION_MINOR "." \
  CS_VERSION_BUILD


#endif // __CSE_CSEVERSION_H__
