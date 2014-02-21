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
#ifndef __ICONTEXT_H__
#define __ICONTEXT_H__

/**\file 
 * Context of the active camera.
 */

/**
 * \addtogroup editorapp
 * @{ */

#include <csutil/scf_interface.h>

struct iCamera;
struct iView;

namespace CSE {
namespace Editor {
namespace Context {

/**
 * Context of the active camera.
 */
struct iContextCamera : public virtual iBase
{
  SCF_INTERFACE (iContextCamera, 1, 0, 0);
  
  /// Set the active camera that is displaying the main CS view
  virtual void SetCamera (iCamera* camera) = 0;

  /// Get the active camera that is displaying the main CS view
  virtual iCamera* GetCamera () = 0; // const?
  
  /// Set the main CS view
  virtual void SetView (iView* view) = 0;

  /// Get the main CS view
  virtual iView* GetView () = 0; // const?
};

} // namespace Context
} // namespace Editor
} // namespace CSE

/** @} */

#endif // __ICONTEXT_H__
