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
#ifndef __IEDITOR_CONTEXT_H__
#define __IEDITOR_CONTEXT_H__

/**\file 
 * Management of state and state changes of the cseditor.
 */

/**
 * \addtogroup core
 * @{ */

#include <csutil/scf_interface.h>
#include <iutil/strset.h>

struct iEventQueue;
struct iObjectRegistry;

namespace CSE {
namespace Editor {
namespace Core {

/**
 * A context defines the state and state changes of a CSE::Editor::Core::iEditor.
 */
struct iContext : public virtual iBase
{
  SCF_INTERFACE (iContext, 1, 0, 0);
  
  /// \todo
  virtual iObjectRegistry* GetObjectRegistry () = 0;

  /// \todo
  virtual iEventQueue* GetEventQueue () = 0;

  /// To be called by the plugins at registration time. data is the default value.
  /// It has no effect if the ID is already registered
  virtual void RegisterData (csStringID id, iBase* data) = 0;

  /// Will fire an event for context change
  virtual void SetData (csStringID id, iBase* data) = 0;

  /// Get a specific context data
  virtual iBase* GetData (csStringID id) = 0;

  /// \todo
  virtual void UpdatePerspective (size_t index) = 0;
  /// \todo
  virtual void DeletePerspective (size_t index) = 0;
};

} // namespace Core
} // namespace Editor
} // namespace CSE

/** @} */

#endif
