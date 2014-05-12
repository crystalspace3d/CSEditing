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
#ifndef __IEDITOR_HEADER_H__
#define __IEDITOR_HEADER_H__

/**\file 
 * Header menus for CS::Editor::Core::iSpace objects.
 */

/**
 * \addtogroup core
 * @{ */

#include <csutil/scf_interface.h>
#include <csutil/refarr.h>

class wxWindow;

namespace CSE {
namespace Editor {
namespace Core {
  
struct iContext;
struct iEditor;
struct iLayout;
  
/**
 * \todo
 */
struct iHeader : public virtual iBase
{
  SCF_INTERFACE (iHeader, 0, 0, 1);

  //iLayout wxwindow is persistent, but content is cleared.
  //iSpace listens to context changes and redraws.
  /// \todo
  virtual void Draw (iContext* context, iLayout* layout) = 0;
};

} // namespace Core
} // namespace Editor
} // namespace CSE

/** @} */

#endif
