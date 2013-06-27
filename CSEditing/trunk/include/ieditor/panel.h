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
#ifndef __IEDITOR_PANEL_H__
#define __IEDITOR_PANEL_H__

/**\file 
 * Panels holding properties and other GUI elements.
 */

/**
 * \addtogroup core
 * @{ */

#include <csutil/scf_interface.h>

namespace CSE {
namespace Editor {
namespace Core {

struct iContext;
struct iLayout;
struct iPanelFactory;
  
/**
 * A panel is a window in the editor.
 * 
 * This panel interface allows plugins to implement custom panels.
 */
struct iPanel : public virtual iBase
{
  SCF_INTERFACE (iPanel, 0, 0, 1);
  
  /// \todo
  // add iPanelfactory::GetEditor instead?
  virtual bool Initialize (iObjectRegistry* obj_reg, iEditor* editor,
			   iPanelFactory* factory) = 0;

  /// Get the factory of this panel
  virtual iPanelFactory* GetFactory () const = 0;

  /**
   * Check whether this panel is visible.
   */
  virtual bool PollVisible (iContext* context) const = 0;

  /**
   * Check whether this panel needs to be redrawn.
   */
  virtual bool PollRedraw (iContext* context) const = 0;

  /**
   * This method will be called when this panel needs to be redrawn.
   */
  virtual void Draw (iContext* context, iLayout* layout) = 0;

/*
  virtual void Save (iDocumentNode* node) const = 0;
  virtual bool Load (iDocumentNode* node) = 0;
*/
};

struct iPanelFactory : public virtual iBase
{
  SCF_INTERFACE (iPanelFactory, 0, 0, 1);

  /// Create a new panel
  virtual csPtr<iPanel> CreateInstance () = 0;

  /// Get the plugin identifier of this factory
  virtual const char* GetIdentifier () const = 0;

  /**
   * Get the label of this factory, that is, the short, human-readable
   * description of this panel factory.
   */
  virtual const char* GetLabel () const = 0;

  /// Get the plugin identifier of the space associated with this factory
  virtual const char* GetSpace () const = 0;
};

} // namespace Core
} // namespace Editor
} // namespace CSE

/** @} */

#endif
