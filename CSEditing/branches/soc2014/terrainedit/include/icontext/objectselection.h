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
#ifndef __ICONTEXT_OBJECTSELECTION_H__
#define __ICONTEXT_OBJECTSELECTION_H__

/**\file 
 * Context for object selection.
 */

/**
 * \addtogroup editorapp
 * @{ */


#include <csutil/scf_interface.h>
#include <csutil/weakrefarr.h>

struct iObject;

namespace CSE {
namespace Editor {
namespace Context {

/**
 * Context for object selection.
 */
struct iContextObjectSelection : public virtual iBase
{
  SCF_INTERFACE (iContextObjectSelection, 1, 0, 0);
  
  virtual void SetActiveObject (iObject* object) = 0;
  virtual iObject* GetActiveObject () const = 0;

  virtual void SetSelectedObjects
    (const csWeakRefArray<iObject>& objects) = 0;
  virtual const csWeakRefArray<iObject>& GetSelectedObjects () const = 0;

  virtual void AddSelectedObject (iObject* object) = 0;
  virtual void DeleteSelectedObject (iObject* object) = 0;
  virtual void ClearSelectedObjects () = 0;

  virtual bool ContainsSelectedObject (iObject* object) const = 0;
};

} // namespace Context
} // namespace Editor
} // namespace CSE

/** @} */

#endif // __ICONTEXT_OBJECTSELECTION_H__
