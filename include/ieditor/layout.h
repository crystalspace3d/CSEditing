/*
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
#ifndef __IEDITOR_LAYOUT_H__
#define __IEDITOR_LAYOUT_H__

/**\file 
 * Visual layout of CS::Editor::Core::iSpace objects
 */

/**
 * \addtogroup core
 * @{ */

#include <csutil/scf_interface.h>

namespace CSE {
namespace Editor {
namespace Core {
  
struct iOperator;
struct iMenu;
  
/**
 * \todo
 */
struct iLayout : public virtual iBase
{
  SCF_INTERFACE (iLayout, 0, 0, 1);

  virtual wxWindow* GetwxWindow () const = 0;

  /// \todo
  // Take label and icon from the iOperator?
  virtual iOperator* AppendOperator (const char* id, const char* label, const char* icon) = 0;
  /// \todo
  // Take label from the iMenu?
  virtual iMenu* AppendMenu (const char* id, const char* label) = 0;
  /// \todo
  // return the label widget
  virtual void AppendLabel (const char* label) = 0;
  /// \todo
  virtual void AppendWindow (wxWindow* window) = 0;
  /// \todo
  virtual void AppendSeparator () = 0;

  // those methods are disabled until they are implemented
  /// \todo
  //virtual iLayout* Row () = 0;
  /// \todo
  //virtual iLayout* Column () = 0;
};

} // namespace Core
} // namespace Editor
} // namespace CSE

/** @} */

#endif
