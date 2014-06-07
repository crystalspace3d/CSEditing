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
#ifndef __SPACE_TERRAINEDITOR_H__
#define __SPACE_TERRAINEDITOR_H__

#include <csutil/csbaseeventh.h>
#include <csutil/eventnames.h>
#include <csutil/ref.h>
#include <csutil/scf_implementation.h>
#include <iutil/event.h>
#include <iutil/comp.h>
#include <iutil/modifiable.h>

#include "cseditor/modifiableeditor.h"
#include "ieditor/editor.h"
#include "ieditor/space.h"

using namespace CSE::Editor::Core;
using namespace CSE::Editor::Utility;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

  class CSTerrainEditSpace : public wxPanel, public csBaseEventHandler,
    public scfImplementation1<CSTerrainEditSpace, iSpace>
  {
  public:
    CSTerrainEditSpace (iBase* parent);
    virtual ~CSTerrainEditSpace ();

    // iSpace
    virtual bool Initialize (iObjectRegistry* obj_reg, iEditor* editor,
			     iSpaceFactory* fact, wxWindow* parent);
    virtual iSpaceFactory* GetFactory () const { return spaceFactory; }
    virtual wxWindow* GetwxWindow ();
    virtual void SetEnabled (bool enabled);
    virtual bool GetEnabled () const;
    virtual void Update ();
    
  private:
    // iEventHandler 
    bool HandleEvent (iEvent &event);

    // Various events
    void OnSize           (wxSizeEvent& event);

    /**
     * Checks the current editor context and, if available, gets the selected
     * particle system, passing it to the modifiable editor and updating the rest of
     * the GUI.
     */
    void Populate ();
    

    /**
     * Called when the editor should be cleared, e.g. when the selected data isn't
     * a particle system, with the message containing the reason.
     */
    void Empty (const wxString& message);

    void OnButtonAddCell (wxCommandEvent &event);
    //void OnButtonRemoveCell (wxCommandEvent &event);
    //void OnButtonAddEffector (wxCommandEvent &event);
    //void OnButtonRemoveEffector (wxCommandEvent &event);
    void OnCellSelect (wxCommandEvent& event);
    //void OnEffectorSelect (wxCommandEvent& event);

    void UpdateCellList  ();
    //void UpdateEffectorList ();

  private:
    static const int borderWidth = 4; 
    bool enabled;
    wxBoxSizer* mainSizer, *middleSizer, *middleLSizer, *middleRSizer;
    wxListBox *cellList;
    csRef<iEditor> editor;
    csRef<iEventQueue> queue;

    iObjectRegistry* object_reg;
    csRef<iSpaceFactory> spaceFactory;
    /// The terrain factory currently being edited
    csRef<iTerrainFactory> factory;

    csRef<iTerrainSystem> terrain;
    
    /// Used to edit the general TS propertiees
    ModifiableEditor* mainEditor;
  
    ModifiableEditor* secondaryEditor;

    // Various event ids
    iEventNameRegistry* nameRegistry;
    csStringID addObject;
    csStringID clearObjects;
    csStringID activateObject;

    enum {
      idMainEditor = 42,
      idSecondaryEditor,
      idButtonAddCell,
      idCellList
      };

    DECLARE_EVENT_TABLE ();
  };
}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif
