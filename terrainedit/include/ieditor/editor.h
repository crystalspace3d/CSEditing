/*
    Copyright (C) 2011-2013 by Jorrit Tyberghein, Jelle Hellemans, Christian Van Brussel
    Copyright (C) 2007 by Seth Yastrov

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
#ifndef __IEDITOR_EDITOR_H__
#define __IEDITOR_EDITOR_H__

/**\file 
 * Main interfaces of the cseditor.
 */

/**
 * \addtogroup core
 * @{ */

#include <csutil/scf_interface.h>

class wxFrame;

struct iDocumentNode;
struct iProgressMeter;

namespace CSE {
namespace Editor {
namespace Core {

struct iActionManager;
struct iComponentManager;
struct iContext;
struct iEditor;
struct iEditorFrame;
struct iMenuManager;
struct iOperatorManager;
struct iPerspective;
struct iPerspectiveManager;

/**
 * Manager of CSE::Editor::Core::iEditor instances, and root access to the
 * cseditor plugin.
 */
struct iEditorManager : public virtual iBase
{
  SCF_INTERFACE (iEditorManager, 1, 0, 0);

  /// \todo
  virtual bool StartEngine () = 0;
  /// \todo
  virtual bool StartApplication () = 0;

  /// \todo
  virtual iEditor* CreateEditor (const char* name, const char* title,
				 iContext* context) = 0;
  /// \todo
  virtual void DeleteEditor (iEditor* editor) = 0;
  /// \todo
  virtual iEditor* FindEditor (const char* name) = 0;
  /// \todo
  virtual iEditor* GetEditor (size_t index) = 0;
  /// \todo
  virtual size_t GetEditorCount () const = 0;
};

/**
 * Instance of an editor, containing at least one CSE::Editor::Core::iEditorFrame
 */
struct iEditor : public virtual iBase
{
  SCF_INTERFACE (iEditor, 1, 0, 0);

  /// \todo
  virtual iContext* GetContext () const = 0;

  /// \todo
  virtual iActionManager* GetActionManager () const = 0;
  /// \todo
  virtual iEditorManager* GetEditorManager () const = 0;
  /// \todo
  virtual iMenuManager* GetMenuManager () const = 0;
  /// \todo
  virtual iOperatorManager* GetOperatorManager () const = 0;
  /// \todo
  virtual iPerspectiveManager* GetPerspectiveManager () const = 0;
  /// \todo
  virtual iComponentManager* GetComponentManager () const = 0;
  // TODO: icon/image manager
  // TODO: translation manager

  /// \todo
  virtual csPtr<iProgressMeter> CreateProgressMeter () const = 0;
  /// \todo
  virtual void ReportStatus (const char* text) = 0;

  //virtual void Save (iDocumentNode* node) const = 0;
  //virtual bool Load (iDocumentNode* node) = 0;

  /// \todo
  virtual iEditorFrame* CreateFrame (const char* title,
				     iPerspective* perspective = nullptr) = 0;
  /// \todo
  virtual void DeleteFrame (iEditorFrame* frame) = 0;
  /// \todo
  virtual size_t GetFrameCount () const = 0;
  /// \todo
  virtual iEditorFrame* GetFrame (size_t index = 0) const = 0;
};

/**
 * A top-level window (that is, a wxFrame) of an instance of
 * CSE::Editor::Core::iEditor.
 */
struct iEditorFrame : public virtual iBase
{
  SCF_INTERFACE (iEditorFrame, 1, 0, 0);

  /// \todo
  virtual wxFrame* GetwxFrame () = 0;

  /// \todo
  virtual bool SetPerspective (iPerspective* perspective) = 0;
  /// \todo
  virtual iPerspective* GetPerspective () const = 0;
};

} // namespace Core
} // namespace Editor
} // namespace CSE

/** @} */

#endif
