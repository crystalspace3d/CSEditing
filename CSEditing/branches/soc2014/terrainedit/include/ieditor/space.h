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
#ifndef __IEDITOR_SPACE_H__
#define __IEDITOR_SPACE_H__

/**\file 
 * Base visual components of the cseditor.
 */

/**
 * \addtogroup core
 * @{ */

#include <csutil/scf_interface.h>

class wxWindow;
class wxBitmap;

struct iObjectRegistry;

namespace CSE {
namespace Editor {
namespace Core {

struct iEditor;
struct iSpaceFactory;

/**
 * A space is a window in the editor.
 * 
 * This space interface allows plugins to implement custom spaces.
 */
struct iSpace : public virtual iBase
{
  SCF_INTERFACE (iSpace, 1, 0, 0);

  /// \todo
  // add iSpacefactory::GetEditor instead?
  virtual bool Initialize (iObjectRegistry* obj_reg, iEditor* editor,
			   iSpaceFactory* factory, wxWindow* parent) = 0;

  /// \todo
  virtual iSpaceFactory* GetFactory () const = 0;

  /// Get the underlying wxWindow content area of this space.
  virtual wxWindow* GetwxWindow () = 0;

  /// \todo
  virtual void SetEnabled (bool enabled) = 0;
  /// \todo
  virtual bool GetEnabled () const = 0;

  /// \todo
  virtual void Update () = 0;
};

/**
 * \todo
 */
struct iSpaceFactory : public virtual iBase
{
  SCF_INTERFACE (iSpaceFactory, 1, 0, 0);

  /// Create a new space
  virtual csPtr<iSpace> CreateInstance (wxWindow* parent) = 0;

  /// Get the plugin identifier of this factory
  virtual const char* GetIdentifier () const = 0;

  /**
   * Get the label of this factory, that is, the short, human-readable
   * description of this space factory.
   */
  virtual const char* GetLabel () const = 0;

  /// \todo
  virtual const wxBitmap& GetIcon () const = 0;

  /// \todo
  virtual bool GetMultipleAllowed () const = 0;

  /// \todo
  virtual size_t GetCount () = 0;
  /// \todo
  virtual size_t GetEnabledCount () = 0;
};

} // namespace Core
} // namespace Editor
} // namespace CSE

/** @} */

#endif // __IEDITOR_SPACE_H__
