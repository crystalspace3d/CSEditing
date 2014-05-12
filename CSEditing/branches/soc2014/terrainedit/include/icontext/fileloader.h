/*
    Copyright (C) 2013 by Christian Van Brussel
    Copyright (C) 2011 by Jelle Hellemans

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
#ifndef __ICONTEXT_FILELOADER_H__
#define __ICONTEXT_FILELOADER_H__

/**\file 
 * Context for file loading.
 */

/**
 * \addtogroup editorapp
 * @{ */

#include <csutil/scf_interface.h>

struct iCollection;

namespace CSE {
namespace Editor {
namespace Context {

/**
 * Context for file loading.
 */
struct iContextFileLoader : public virtual iBase
{
  SCF_INTERFACE (iContextFileLoader, 1, 0, 0);
  
  virtual void SetPath (const char* path) = 0;
  virtual const char* GetPath () = 0;
  virtual void SetFilename (const char* filename) = 0;
  virtual const char* GetFilename () = 0;
  virtual void SetCollection (iCollection* collection) = 0;
  virtual iCollection* GetCollection () const = 0;
};

} // namespace Context
} // namespace Editor
} // namespace CSE

/** @} */

#endif // __ICONTEXT_FILELOADER_H__
