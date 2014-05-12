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
#include "cssysdef.h"
#include "cseditor/wxpgslider.h"

using namespace CSE::Editor::Utility;

//----------------- wxPGSliderEditor ---------------------

WX_PG_IMPLEMENT_EDITOR_CLASS(SliderEditor, wxPGSliderEditor, wxPGEditor)

wxPGWindowList wxPGSliderEditor::CreateControls( wxPropertyGrid*  propgrid,
                                                 wxPGProperty*    property,
                                                 const wxPoint&   pos,
                                                 const wxSize&    size ) const
{
  double v_d = property->GetValue().GetDouble();
  if ( v_d < min )
    v_d = 0;
  else if ( v_d > max )
    v_d = max;

  wxSlider *ctrl = new wxSlider;
  /*
#ifdef __WXMSW__
  ctrl->Hide();
#endif
  //*/

  ctrl->Create ( propgrid->GetPanel(),
                 wxPG_SUBID2,
                 (int)(v_d * precision),
                 (int)(min * precision),
                 (int)(max * precision),
                 pos,
                 size,
                 wxSL_HORIZONTAL );

  return wxPGWindowList(ctrl);
}

void wxPGSliderEditor::UpdateControl ( wxPGProperty* property, wxWindow* wnd ) const
{
  wxSlider* ctrl = wxDynamicCast ( wnd, wxSlider );
  if ( ctrl )
  {
    double val;
    if (wxPGVariantToDouble (property->DoGetValue(), &val))
    {
      if ( val < min )
        val = min;
      else if ( val > max )
        val = max;
      ctrl->SetValue ( (int)(val * precision) );

      //static_cast<wxPGSliderProperty*>(property)->GetLabel()
      //  ->SetValue( wxString::Format(wxT("%ld"), val * precision) );
    }
  }
}

bool wxPGSliderEditor::OnEvent ( wxPropertyGrid*  propgrid, 
                                 wxPGProperty*    property,
                                 wxWindow*        wnd,
                                 wxEvent&         event ) const
{
  if(event.GetEventType() == wxEVT_SCROLL_CHANGED)
  {
    // Update the value    
    event.Skip();
    propgrid->EditorsValueWasModified();

    return true;
  }
  
  return false;
}

bool wxPGSliderEditor::GetValueFromControl ( wxVariant&     variant,
                                             wxPGProperty*  property,
                                             wxWindow*      wnd ) const
{
  wxSlider* ctrl = wxDynamicCast ( wnd, wxSlider );
  if ( ctrl )
  {
    variant = wxVariant ( (double)(ctrl->GetValue())/(double)(precision) );
    property->SetValue ( variant );
  }

  return true;
}

void wxPGSliderEditor::SetValueToUnspecified ( wxPGProperty* property, wxWindow* wnd) const
{
  wxSlider* ctrl = wxDynamicCast ( wnd, wxSlider );
  if ( ctrl )
  {
    ctrl->SetValue (0);
  }
}

/**
void wxPGSliderEditor::DrawValue ( wxDC&           dc,
                                   const wxRect&   rect,
                                   wxPGProperty*   property,
                                   const wxString& text ) const
{

}
//*/
