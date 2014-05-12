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
#ifndef __IEDITOR_COMPONENT_H__
#define __IEDITOR_COMPONENT_H__

/**\file 
 * Management of cseditor components.
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
struct iEditorComponent;
struct iHeader;
struct iPanelFactory;
struct iSpaceFactory;

/**
 * A component of the cseditor that is active but not visible within
 * the graphical user interface.
 */
struct iEditorComponent : public virtual iBase
{
  SCF_INTERFACE (iEditorComponent, 1, 0, 0);

  /// \todo
  virtual bool Initialize (iEditor* editor) = 0;
  /// \todo
  virtual void Update () = 0;

  /// \todo
  virtual void Save (iDocumentNode* node) const = 0;
  /// \todo
  virtual bool Load (iDocumentNode* node) = 0;
};

/**
 * Manages the set of components of the editor.
 */
struct iComponentManager : public virtual iBase
{
  SCF_INTERFACE (iComponentManager, 1, 0, 0);

  /// \todo
  virtual bool RegisterComponent (const char* pluginName) = 0;

  /// \todo
  virtual bool RegisterSpace (const char* pluginName) = 0;

  /// \todo
  virtual bool RegisterHeader (const char* pluginName) = 0;

  /// \todo
  virtual bool RegisterPanel (const char* pluginName) = 0;

  /// \todo
  virtual bool RegisterPanel (iPanelFactory* panel) = 0;

  /// \todo
  virtual bool UnregisterPanel (iPanelFactory* panel) = 0;

  /// \todo
  virtual iEditorComponent* FindComponent (const char* pluginName) const = 0;
  /// \todo
  virtual iSpaceFactory* FindSpaceFactory (const char* pluginName, size_t& index) const = 0;
};

} // namespace Core
} // namespace Editor
} // namespace CSE

/** @} */

#endif // __IEDITOR_COMPONENT_H__
