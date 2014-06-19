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
#include "icontext/camera.h"    

#include <wx/wx.h>
#include <wx/artprov.h>

#include "terraineditor.h"

#include <iengine/camera.h>

#include "cstool/materialbuilder.h"
#include "imesh/modifiableterrain.h"
#include "ivideo/material.h"


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
    EVT_LISTBOX (idCellList, CSTerrainEditSpace::OnCellSelect)
    EVT_BUTTON (idButtonAddCell, CSTerrainEditSpace::OnButtonAddCell)
    EVT_BUTTON (idButtonDeleteCell , CSTerrainEditSpace::OnButtonDeleteCell)   
  END_EVENT_TABLE ()

  SCF_IMPLEMENT_FACTORY (CSTerrainEditSpace)
  
  CSTerrainEditSpace::CSTerrainEditSpace (iBase* parent) 
  : scfImplementationType (this, parent),
    object_reg (0) 
  {
    // Setup namespace-scoped pointer to editor, to be used by the static
    // event handler to reach the space
    terrainEditorSpace = this;
    decal = nullptr;
  }

  CSTerrainEditSpace::~CSTerrainEditSpace () 
  {
    delete mainEditor;
    delete secondaryEditor;
    if (decal) decalManager->DeleteDecal (decal);
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

    csInitializer::RequestPlugins (object_reg, CS_REQUEST_PLUGIN ("crystalspace.decal.manager",iDecalManager),CS_REQUEST_END);
        
    // Setup the event names
    nameRegistry = csEventNameRegistry::GetRegistry (object_reg);
    //addObject = nameRegistry->GetID ("crystalspace.editor.context.selection.addselectedobject");
    //clearObjects = nameRegistry->GetID ("crystalspace.editor.context.selection.clearselectedobjects");
    activateObject = nameRegistry->GetID ("crystalspace.editor.context.selection.setactiveobject");
       
    // Respond to context events
    //csEventID contextSelect = nameRegistry->GetID ("crystalspace.editor.context");
    RegisterQueue (editor->GetContext ()->GetEventQueue (), activateObject);

    // Register a mouse listener to the global event queue
    eventListener.AttachNew (new CSTerrainEditSpace::EventListener (this));

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
    
    middleLSizer->Add (new wxStaticText (this, wxID_ANY, wxT ("Cells")));
    cellList = new wxListBox (this,idCellList);
    middleLSizer->Add ( cellList,
                        1,
                        wxALL | wxEXPAND,
                        borderWidth );

    wxButton* but = new wxButton (this, idButtonAddCell, wxT ("Add Cell"));
    but->SetSize (-1, 32);
    middleRSizer->Add ( but,
                        0,
                        wxALL | wxEXPAND,
                        borderWidth );

    wxButton* but2 = new wxButton (this, idButtonDeleteCell, wxT ("DeleteCell"));
    but->SetSize (-1, 32);
    middleRSizer->Add ( but2,
                        0,
                        wxALL | wxEXPAND,
                        borderWidth );

       
    secondaryEditor = new ModifiableEditor (object_reg, this, idSecondaryEditor,
      wxDefaultPosition, wxDefaultSize, 0L, wxT ("Secondary editor"));

    mainSizer->Add (secondaryEditor, 1, wxALL | wxEXPAND, borderWidth);
    enabled = true;
    printf ("\nTerrain Editor working and all\n");
       
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
      	
   	// Get the object from the context
    csRef<iContextObjectSelection> objectSelectionContext =
      scfQueryInterface<iContextObjectSelection> (editor->GetContext ());
       

    // Search for the iModifiable interface of the particle factory
    result = objectSelectionContext->GetActiveObject ();
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
 	
 	  factory = terrainFactory;
 	  terrain = terrainSys;
    UpdateCellList ();

    
    csRef<iModifiable> modifiable = scfQueryInterface<iModifiable> (mesh->GetMeshObject()->GetFactory());  
    if (!modifiable)
    {
      printf("Selected particle system is non-standard and doesn't implement iModifiable. It cannot be edited.");
      return;
    }

    // Caches a casted pointer to the factory
    
   
    // Updates the GUI
    mainEditor->SetModifiable (modifiable);

    // Setup the decal
    decalManager = csQueryRegistry<iDecalManager> (object_reg);
    
    if (decalManager)
    {
    // Create the decal material
    iMaterialWrapper* material = CS::Material::MaterialBuilder::CreateColorMaterial(object_reg, "decal", csColor (1.0f, 0.0f, 0.0f));
    
    // Setup the decal template
    decalTemplate = decalManager->CreateDecalTemplate (material);
    decalTemplate->SetDecalOffset (0.1f);
    decalTemplate->SetMaximumVertexCount (128000);
    decalTemplate->SetMaximumTriangleCount (64000);

    }  

    rectSize = 50.0f;
    rectHeight = -20.0f;
  }

  void CSTerrainEditSpace::Empty (const wxString& message)
  {
    mainEditor->Clear ();
    secondaryEditor->Clear ();
    cellList->Clear();
    
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

  void CSTerrainEditSpace::OnButtonAddCell ( wxCommandEvent &event )
  {
    // TODO: context menu to pick
      printf("Add Cell");

      csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
      csRef<iMaterialWrapper> terrainmat = engine->CreateMaterial ("terrain", 0);

      csRef<iTerrainFactoryCell> cellf(factory->AddCell ());
     //defining new cell properties 
      cellf->SetName ("2");    
      cellf->SetPosition (csVector2 (-128.0f + (factory->GetCellCount () - 1) * 256.0f, -128.0f));
      cellf->SetSize (csVector3 (256.0f, 16.0f, 256.0f));
      cellf->SetGridWidth (257);
      cellf->SetGridHeight (257);
      cellf->SetMaterialMapWidth (256);
      cellf->SetMaterialMapHeight (256);
      cellf->SetMaterialPersistent (false);

      cellf->SetBaseMaterial (terrainmat);

      cellf->GetRenderProperties ()->SetParameter ("block resolution", "16");
      cellf->GetRenderProperties ()->SetParameter ("lod splitcoeff", "16");

      cellf->GetFeederProperties ()->SetParameter("heightmap source", "/lev/terrain/heightmap.png");
      cellf->GetFeederProperties ()->SetParameter("materialmap source", "/lev/terrain/materialmap.png");
    //definition ends.  
  
    csRef<iTerrainCell> cell (terrain->AddCell(cellf));
    UpdateCellList();
  }

  void CSTerrainEditSpace::OnCellSelect ( wxCommandEvent& event )
  {
    
    iTerrainFactoryCell* cell = static_cast<iTerrainFactoryCell*> (cellList->GetClientData (event.GetSelection ()));
    mod = scfQueryInterface<iModifiable> (cell);

    csRef<CS::Utility::iModifiableListener> listener;
    listener.AttachNew (new ModifiableListener ());
    
    
    if (mod.IsValid ()) {
      secondaryEditor->SetModifiable (mod);
      mod->AddListener(listener);     
      } 
      else {
      csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.editor.space.terrainedit",
        "Tried to edit cell not implementing iModifiable.");

      return;
    }
  }

  void CSTerrainEditSpace::UpdateCellList ()
  {
    cellList->Clear ();
    
    for (size_t i = 0; i < factory->GetCellCount (); i++) {
      iTerrainFactoryCell* cell = factory->GetCell (i);
      cellList->Append (wxString::Format (wxT ("Cell #%u"), i), (void*)cell);
    }
  }

   void CSTerrainEditSpace::OnButtonDeleteCell (wxCommandEvent &event ) 
  {
  	csRef<iTerrainCell> cell (terrain->GetCell (terrain->GetCellCount () - 1));
    terrain->RemoveCell (cell);
    csRef<iTerrainFactoryCell> cellf (factory->GetCell (factory->GetCellCount () - 1));
    factory->RemoveCell (cellf);
    UpdateCellList();
    
  }   

  void CSTerrainEditSpace::UpdateModifier (int x, int y)
  {
    //printf("%i \t %i \n", x , y);
    
    csRef<iModifiableDataFeeder> feeder = scfQueryInterface<iModifiableDataFeeder> (factory->GetFeeder ());
   
     csRef<iContextCamera> contextCamera = scfQueryInterface<iContextCamera> (editor->GetContext ());

    
    iCamera* camera = contextCamera->GetCamera();
    iView* view = contextCamera->GetView();

    csVector2 v2d (x, y);
    csVector3 v3d = view->InvProject (v2d, 1000.0f);    
    csVector3 startBeam = camera->GetTransform ().GetOrigin ();
    csVector3 endBeam = camera->GetTransform ().This2Other (v3d);   
  
    csRef<iMeshObject> meshObject = scfQueryInterface<iMeshObject> (terrain);
    csVector3 position;

    if (!meshObject->HitBeamObject (startBeam, endBeam, position, nullptr))
    {
    RemoveModifier ();
    return;
    }    

    if ((position - lastPosition).Norm () < 0.1f)
    {
    return;
    } 
    
    
    lastPosition = position;

    RemoveModifier();

    // Create the terrain modifier
    //TODO: why this minus on Z???
    //modifier = feeder->AddModifier (csVector3 (position.x, rectHeight, -position.z), rectSize, rectSize);   


    //Decals
    csVector3 up (0.f, 1.f, 0.f);
    csVector3 direction (0.f, 1.f, 0.f);    
    iMeshWrapper* meshWrapper = meshObject->GetMeshWrapper (); 

    decal = decalManager->CreateDecal (decalTemplate, meshWrapper, position, up, direction, rectSize, rectSize);
    
  
  } 


  void CSTerrainEditSpace::RemoveModifier () 
  {
    // Remove the previous decal
    
    csRef<iModifiableDataFeeder> feeder = scfQueryInterface<iModifiableDataFeeder> (factory->GetFeeder ());
    if (modifier) feeder->RemoveModifier (modifier);
    modifier.Invalidate ();

    // Remove the previous decal
    if (decal) decalManager->DeleteDecal (decal);
    decal = nullptr; 
  }

  void CSTerrainEditSpace::Paint () 
  {
    // Remove the previous decal
    printf("Inside paint !");
    modifier.Invalidate ();
  }

  CSTerrainEditSpace::EventListener::EventListener (CSTerrainEditSpace* editor)
    : scfImplementationType (this), editor (editor)
  {
    // Find the main CS event queue
    csRef<iEventQueue> eventQueue =
      csQueryRegistry<iEventQueue> (editor->object_reg);

    // Register to the 'mouse move' event
    csEventID events[] = {
      csevMouseMove (editor->object_reg, 0),
      csevMouseClick (editor->object_reg, 0),
      csevMouseDown (editor->object_reg, 0),
      CS_EVENTLIST_END
    };

    eventQueue->RegisterListener (this, events);
  }

  bool CSTerrainEditSpace::EventListener::HandleEvent (iEvent &event)
  {
    iEventNameRegistry* nameRegistry = csEventNameRegistry::GetRegistry (editor->object_reg);
    
    if (event.Name == csevMouseMove (nameRegistry, 0))
    {
      int mouse_x = csMouseEventHelper::GetX (&event);
      int mouse_y = csMouseEventHelper::GetY (&event);

      printf ("mouse move at position %i-%i\n", mouse_x, mouse_y);

      editor->UpdateModifier(mouse_x, mouse_y);
    }
    
    else if (event.Name == csevMouseClick(nameRegistry, 0))
    {
      int mouse_x = csMouseEventHelper::GetX (&event);
      int mouse_y = csMouseEventHelper::GetY (&event);

      printf ("mouse click at position %i-%i\n", mouse_x, mouse_y);

      if (csMouseEventHelper::GetButton (&event) == csmbLeft)
      {
        editor->Paint();
        printf("Left Click");
      }

      return false;
    }

    else if (event.Name == csevMouseDown(nameRegistry, 0))
    {

      int mouse_x = csMouseEventHelper::GetX (&event);
      int mouse_y = csMouseEventHelper::GetY (&event);

      if (csMouseEventHelper::GetButton (&event) == csmbWheelUp)
      {
        rectSize += 5.0f;
        if (rectSize > 100.f)
          rectSize = 100.f;
          editor->UpdateModifier(mouse_x, mouse_y);
      }
      else if (csMouseEventHelper::GetButton (&event) == csmbWheelDown)
      {
        rectSize -= 5.0f;
        if (rectSize < 5.0f)
          rectSize = 5.0f;
          editor->UpdateModifier(mouse_x, mouse_y);
      }

        return false;
    }

    return false;
  }

  void ModifiableListener::ValueChanged (CS::Utility::iModifiable* modifiable, size_t parameterIndex)
  {
    csVariant value;
    modifiable->GetParameterValue (parameterIndex, value);
    csPrintf ("New value set for parameter %zu: %s\n", parameterIndex, value.Description ().GetData ());

    size_t first_cell = 0;
    for (size_t i = 0; i < factory->GetCellCount(); i++) 
    {
      iTerrainFactoryCell* cell = factory->GetCell (i);
      iTerrainCell* cell_sys = terrain->GetCell (first_cell);
      terrain->RemoveCell (cell_sys);
      terrain->AddCell (cell);
    }
  }
}
CS_PLUGIN_NAMESPACE_END (CSEditor)
