/*
    Copyright (C) 2012 by Andrei Bârsan

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
#ifndef __CSEDITOR_MODIFIABLE_EDITOR_H__
#define __CSEDITOR_MODIFIABLE_EDITOR_H__

/**\file 
 * Graphical User Interface generation for CS::Utility::iModifiable objects.
 */

/**
 * \addtogroup utility
 * @{ */

#include <csutil/csstring.h>
#include <csutil/hash.h>
#include <csutil/ref.h>

// Property grid manager
#include "cseditor/wx/propgrid/manager.h"
#include "cseditor/wx/propgrid/advprops.h"
#include "cseditor/wx/propgrid/propdev.h"
#include "cseditor/wx/propgrid/editors.h"

class csVariant;
struct iLoaderPlugin;
struct iPluginManager;
struct iTranslator;

namespace CS {
namespace Utility {

struct iModifiable;
struct iModifiableConstraint;
struct iModifiableDescription;

} // namespace Utility
} // namespace CSE

namespace CSE {
namespace Editor {
namespace Utility {

/**
 * Graphical User Interface generation for CS::Utility::iModifiable objects.
 */
class /*CS_CRYSTALSPACE_EXPORT*/ ModifiableEditor : public wxPanel 
{
public:
  // TODO: use CS types instead of wx ones?
  /// Constructor
  ModifiableEditor (iObjectRegistry* objectRegistry, 
		    wxWindow* parent,
		    wxWindowID id,
		    const wxPoint& position,
		    const wxSize& size,
		    long style,
		    const wxString& name);

  /**
   * Set the resource VFS root path that has to be used to find the translation
   * data.
   */
  virtual void SetResourcePath (const char* path);

  /**
   * Set whether or not an additional description area will be added in order to
   * display additional textual description on the selected elements.
   *
   * \note This method must be called before SetModifiable() or after
   * Clear().
   */
  virtual void SetDescriptionEnabled (bool enabled);

  /**
   * Get whether or not an additional description area will be added in order to
   * display additional textual description on the selected elements.
   */
  virtual bool GetDescriptionEnabled () const;

  virtual void SetRecursive (bool recursive);
  virtual bool GetRecursive () const;

  /// Set the current active modifiable entity to be edited
  virtual void SetModifiable (CS::Utility::iModifiable* modifiable, size_t offset = 0,
			      CS::Utility::iModifiableDescription* description = nullptr);

  /// Get the currently active modifiable entity being edited
  virtual CS::Utility::iModifiable* GetModifiable () const;

  /// Get the description of the modifiable entity that is currently active
  virtual CS::Utility::iModifiableDescription* GetModifiableDescription () const;

  /// Set the property grid's message
  //void SetMessage (const wxString& title, const wxString& message);

  // Clear all data from the editor
  void Clear ();

private:
  void UpdateSize ();

  /// Gets called by the space owning this component
  virtual void OnSize (wxSizeEvent& event);

  /// Appends the element to the property grid, in its own category
  void Append (wxPropertyCategory* category,
	       CS::Utility::iModifiableDescription* description, size_t& offset);

  /**
   * Actually does the work of taking the variant, its name, translation and translated
   * description and generating the corresponding GUI element for each of them.
   */
  void AppendVariant (wxPGPropArg categoryID, csVariant* variant, size_t index,
		      CS::Utility::iModifiableConstraint* param,
		      const wxString& label, const wxString& name,
		      const wxString& description);

  void OnPropertyGridChanging (wxPropertyGridEvent& event);
  void OnPropertyGridChanged (wxPropertyGridEvent& event);
  void OnPropertyGridItemChanged (wxPropertyGridEvent& event);

  bool ReportError (const char* description, ...);

private:
  iObjectRegistry* objectRegistry;
  csRef<CS::Utility::iModifiable> modifiable;
  csRef<CS::Utility::iModifiableDescription> description;
  size_t globalOffset;

  csRef<iPluginManager> pluginManager;
  csRef<iTranslator> translator;
  csRef<iLoaderPlugin> translatorLoader;
  csString resourcePath;
  bool descriptionEnabled;
  bool recursive;

  enum
  {
    pageId = 1
  };

  wxPropertyGridPage* page;
  wxPropertyGridManager* pgMananager;
  csHash<size_t, wxPGProperty*> indexes;

  DECLARE_EVENT_TABLE ();
};

} // namespace Utility
} // namespace Editor
} // namespace CSE

/** @} */

#endif
