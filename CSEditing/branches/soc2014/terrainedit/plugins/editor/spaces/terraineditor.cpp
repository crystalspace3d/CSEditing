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
#include <cssysdef.h>
#include <csgeom/math3d.h>
#include <csgeom/plane3.h>
#include <cstool/csview.h>
#include <cstool/enginetools.h>
#include <cstool/initapp.h>
#include <csutil/event.h>
#include <csutil/ref.h>
#include <csutil/scf.h>
#include <csutil/sysfunc.h>
#include <imap/reader.h>
#include <imesh/object.h>
#include <imesh/objmodel.h>
#include <imesh/terrain2.h>
#include <iutil/csinput.h>
#include <iutil/eventq.h>
#include <iutil/object.h>
#include <iutil/objreg.h>
#include <iutil/plugin.h>
#include <iutil/virtclk.h>
#include <ivaria/translator.h>
#include <ivideo/graph2d.h>
#include <ivideo/graph3d.h>
#include <ivideo/natwin.h>
#include <ivideo/wxwin.h>

#include "icontext/fileloader.h"
#include "icontext/objectselection.h"
#include "ieditor/context.h"
#include "ieditor/operator.h"

#include <wx/wx.h>
#include <wx/artprov.h>

#include "terraineditor.h"

using namespace CS::Utility;
using namespace CSE::Editor::Context;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{
  CSTerrainEditSpace* terrainEditorSpace;

  /// Helper function; apparently this functionality isn't built into wx


  // This table triggers a bunch of MFP conversion warnings, but due to
  // the fact that the wxPanel is the first class being inherited, there
  // shouldn't be any problems.
  BEGIN_EVENT_TABLE (CSTerrainEditSpace, wxPanel)
    EVT_SIZE  (CSTerrainEditSpace::OnSize)
  END_EVENT_TABLE ()

  SCF_IMPLEMENT_FACTORY (CSTerrainEditSpace)
  
  CSTerrainEditSpace::CSTerrainEditSpace (iBase* parent) 
  : scfImplementationType (this, parent),
    object_reg (0) 
  {
    // Setup namespace-scoped pointer to editor, to be used by the static
    // event handler to reach the space
    terrainEditorSpace = this;
  }

  CSTerrainEditSpace::~CSTerrainEditSpace () 
  {
    delete mainEditor;
    delete secondaryEditor;
  }

  bool CSTerrainEditSpace::Initialize (iObjectRegistry* obj_reg, iEditor* editor,
				    iSpaceFactory* fact, wxWindow* parent)
  {
    object_reg = obj_reg;
    this->editor = editor;
    spaceFactory = fact;
    mainSizer = new wxBoxSizer (wxVERTICAL);

    // Initializes the wxPanel part of the space
    Create (parent);

    // Load the particle plugins
    csRef<iPluginManager> pluginManager = csQueryRegistry<iPluginManager> (object_reg);
    
    // Setup the event names
    nameRegistry = csEventNameRegistry::GetRegistry (object_reg);
    //addObject = nameRegistry->GetID ("crystalspace.editor.context.selection.addselectedobject");
    //clearObjects = nameRegistry->GetID ("crystalspace.editor.context.selection.clearselectedobjects");
    activateObject = nameRegistry->GetID ("crystalspace.editor.context.selection.setactiveobject");

    // Respond to context events
    //csEventID contextSelect = nameRegistry->GetID ("crystalspace.editor.context");
    RegisterQueue (editor->GetContext ()->GetEventQueue (), activateObject);
    
    // Prepare modifiable editors
    mainEditor = new ModifiableEditor (object_reg, this, idMainEditor, wxDefaultPosition,
				       parent->GetSize (), 0L, wxT ("Modifiable editor"));
    mainSizer->Add (mainEditor, 1, wxEXPAND | wxALL, borderWidth);
    SetSizer (mainSizer);
    mainSizer->SetSizeHints (this);

    middleSizer = new wxBoxSizer (wxHORIZONTAL);
    middleLSizer = new wxBoxSizer (wxVERTICAL);
    middleRSizer = new wxBoxSizer (wxVERTICAL);

    mainSizer->Add (middleSizer, 1, wxEXPAND, borderWidth );
    middleSizer->Add (middleLSizer, 1, wxEXPAND | wxALL, borderWidth);
    middleSizer->Add (middleRSizer, 1, wxEXPAND | wxALL, borderWidth);
    
    
    secondaryEditor = new ModifiableEditor (object_reg, this, idSecondaryEditor,
      wxDefaultPosition, wxDefaultSize, 0L, wxT ("Secondary editor"));

    mainSizer->Add (secondaryEditor, 1, wxALL | wxEXPAND, borderWidth);
    enabled = true;
    printf ("\nTerrain Editor working\n");
    printf("Test");
    // Populate with the current active object 
    Populate (); 

    return true;
  }

  void CSTerrainEditSpace::Update () {
  }

  void CSTerrainEditSpace::OnSize (wxSizeEvent& event)
  {
    mainSizer->SetDimension (0, 0, event.GetSize ().GetWidth (), event.GetSize ().GetHeight ());
    event.Skip ();
  }

  void CSTerrainEditSpace::Populate ()
  {
    
    printf("Test");

    // Get the object from the context
    csRef<iContextObjectSelection> objectSelectionContext =
      scfQueryInterface<iContextObjectSelection> (editor->GetContext ());
    
    // Search for the iModifiable interface of the particle factory
    iObject* result = objectSelectionContext->GetActiveObject ();
    if (!result)
    {
      printf("No object selected.");
      return;
    }
    
    //csString entityName (result->GetName ());

    csRef<iMeshWrapper> mesh = scfQueryInterface<iMeshWrapper> (result);
    if (!mesh)
    {
      printf("No mesh selected.");
      return;
    }

    csRef<iTerrainSystem> terrainSys =
      scfQueryInterface<iTerrainSystem> (mesh->GetMeshObject()); 
    if (!terrainSys)
    {
      printf("No terrain system selected.");
      return;
    }
   
    csRef<iTerrainFactory> terrainFactory = scfQueryInterface<iTerrainFactory> (mesh->GetMeshObject()->GetFactory());
    
    if (!terrainFactory)
    {
      printf("No terrain factory selected.");
      return;
    }
 
    /*
    csRef<iModifiable> modifiable = scfQueryInterface<iModifiable> (fac->GetMeshObjectFactory ());  
    if (!modifiable)
    {
      Empty (wxT ("Selected particle system is non-standard and doesn't implement iModifiable. It cannot be edited."));
      return;
    }
    */
    // Caches a casted pointer to the factory
    factory = terrainFactory;
   
    // Updates the GUI
    //mainEditor->SetModifiable (modifiable);
    
    }

  void CSTerrainEditSpace::Empty (const wxString& message)
  {
    mainEditor->Clear ();
    secondaryEditor->Clear ();
    
    //mainEditor->SetMessage (wxString (wxT ("Notice:")), message);
  }

  wxWindow* CSTerrainEditSpace::GetwxWindow ()
  {
    return this;
  }

  bool CSTerrainEditSpace::GetEnabled () const {
    return enabled;
  }

  void CSTerrainEditSpace::SetEnabled (bool enabled) {
    this->enabled = enabled;
  }

  bool CSTerrainEditSpace::HandleEvent (iEvent& event)
  {
    csRef<iEventNameRegistry> strings = csQueryRegistry<iEventNameRegistry> ( object_reg );

#ifdef CS_DEBUG
    printf ("\tCaught event: %s (ID #%u)\n",
      strings->GetString ( event.GetName () ),
      (unsigned int) event.GetName () );
#endif

    if (event.Name == activateObject) {
      // The user activated (double-clicked) something!
      Populate ();
    }

    return false;
  }


}
CS_PLUGIN_NAMESPACE_END (CSEditor)
