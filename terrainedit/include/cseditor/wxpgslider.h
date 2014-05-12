/*
    Copyright (C) 2012 by Andrei Barsan

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
#ifndef WXPGSLIDER_H
#define WXPGSLIDER_H

/**\file 
 * Slider property for wxPropertyGrid.
 */

/**
 * \addtogroup utility
 * @{ */

// common wx stuff
#include <wx/slider.h>
#include <wx/wx.h>

// for wxPGEditor
#include "cseditor/wx/propgrid/propgrid.h"
#include "cseditor/wx/propgrid/propdev.h"
#include "cseditor/wx/propgrid/advprops.h"
#include "cseditor/wx/propgrid/editors.h"

namespace CSE {
namespace Editor {
namespace Utility {

//----------------- wxPGSliderEditor ---------------------

/**
 * wxSlider for wxPropertyGrid. Contains a value slider.
 */
class WXDLLIMPEXP_PG wxPGSliderEditor : public wxPGEditor 
{
#ifndef SWIG
  WX_PG_DECLARE_EDITOR_CLASS(wxPGSliderEditor)
#endif

public:
  wxPGSliderEditor (float min = 0, float max = 100, int precision = 1)
    : precision(precision), min(min), max(max)        
  {
  }

  virtual ~wxPGSliderEditor ()
  {}

  // Macro for the CreateControls method stub
  wxPG_DECLARE_CREATECONTROLS;

  /// Implementation of wxPGEditor::UpdateControl()
  void UpdateControl ( wxPGProperty* property, wxWindow* wnd) const;
  /// Implementation of wxPGEditor::OnEvent()
  bool OnEvent ( wxPropertyGrid* propgrid, wxPGProperty* property, wxWindow* wnd, wxEvent& event) const;
  /// Implementation of wxPGEditor::GetValueFromControl()
  bool GetValueFromControl ( wxVariant& variant, wxPGProperty* property, wxWindow* wnd) const;
  /// Implementation of wxPGEditor::SetValueToUnspecified()
  void SetValueToUnspecified ( wxPGProperty* property, wxWindow* wnd) const;
  //void DrawValue ( wxDC& dc, const wxRect& rect, wxPGProperty* property, const wxString& text) const;

private:
  int precision;
  float min, max;
};

} // namespace Utility
} // namespace Editor
} //namespace CSE

/** @} */

#endif
