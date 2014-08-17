/*
    Copyright (C) 2014 by Soumitra Saxena

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


#include <iutil/modifiable.h>
#include "cseditor/modifiableeditor.h"

#include <csutil/csbaseeventh.h>
#include <csutil/eventnames.h>
#include <csutil/ref.h>
#include <csutil/scf_implementation.h>
#include <iutil/event.h>
#include <iutil/comp.h>
#include "ieditor/editor.h"
#include "ieditor/space.h"

#include "ivaria/decal.h"
#include "imesh/terrain2.h"
#include "imesh/modifiableterrain.h"

#include "icontext/camera.h"

#include <iutil/document.h>
#include <iutil/vfs.h>

using namespace CSE::Editor::Core;
using namespace CSE::Editor::Utility;
using namespace CS::Utility;


CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{
   /// The Terrain Factory currently being edited
    csRef<iTerrainFactory> factory;


   /// The Terrain System currently being used
    csRef<iTerrainSystem> terrain;


   /// A reference Terrain Factory Cell to define properties for any subsequent new Terrain Factory Cell 
    csRef<iTerrainFactoryCell> terrainFactoryCell;

   /// Dimensions for Terrain Modifier 
    float rectSize;
    float rectHeight;

   /// Boolean that tells when my mouse functions are ready to kick in.
    bool mouseready = 0;

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
     * Terrain System, passing it to the modifiable editor and updating the rest of
     * the GUI.
     */
    void Populate ();
    

    /**
     * Called when the editor should be cleared, e.g. when the selected data isn't
     * a terrain system, with the message containing the reason.
     */
    void Empty (const wxString& message);

    /**
     * Function that is called when the Add cell button is clicked. It adds a Cell to the Factory, defines it,
     and then adds it to the system.
     */
    void OnButtonAddCell (wxCommandEvent &event);

    /**
     * Function that is called when a cell is selected from the list. It loads the secondary modifier corresponding
     to the cell and displays it's properties at the bottom sizer.
     */
    void OnCellSelect (wxCommandEvent& event);
    

    /**
     * Function that updates the Cell list to display the current number of cells in the Factory.
     */

    void UpdateCellList  ();


    /**
     * Functions that are called when the "mode" of the Editor is changed. The Default(0) mode does nothing, the Paint mode(1)
     is used for painting on the terrain using decals , and the Modifier(2) mode is used to modify the terrain under the decal.
     */    

    void OnButtonDefaultMode (wxCommandEvent& event); 
    void OnButtonPaintMode (wxCommandEvent& event); 
    void OnButtonModifierMode (wxCommandEvent& event); 


    /**
     * Function that is called when the Delete cell button is clicked. It deletes a Cell from the Factory and the system.
     */
    void OnButtonDeleteCell (wxCommandEvent& event);

    /**
     * Function that creates the mesh generator. No button is associated to this Function yet, but it has been defined in the cpp.
     */
    void OnButtonCreateMeshGenerator (wxCommandEvent& event);   

    /**
     * Function that Removes the previous Modifier and decal.
     */
    void RemoveModifier ();

    /**
     * Function that is responsible for Updating the Decal and Modifier. It uses the current mouse x and y , and is called on every 
     mouse move event.
     */
    void UpdateModifier ( int x , int y);

    /**
     * Function that is responsible for Painting the current active decal on the Terrain. It is called on every mouse click.
     mouse move event.
     */
    void Paint();

    /**
     * Function that Updates the Material list.
     */    
    void UpdateMaterialList();

    /**
     * Function that sets the current selected Material as the active material.
     */
    void OnMatSelect (wxCommandEvent& event);


  private:
    static const int borderWidth = 4; 
    bool enabled;
    wxBoxSizer* mainSizer, *middleSizer, *middleLSizer, *middleRSizer;
    wxListBox *cellList, *matList;
    csRef<iEditor> editor;

    iObjectRegistry* object_reg;
    csRef<iSpaceFactory> spaceFactory;
    csRef<iTerrainDataFeeder> feeder;

    iObject* result;

      /// Used to edit the general TS propertiees
    ModifiableEditor* mainEditor;
  
    ModifiableEditor* secondaryEditor;  

    csRef<iModifiable> mod; 

    //Mode variable Default = 0 , Decal = 1 , Modifier = 2
    int mode;

    //Decal Implementation Variables   
   
    csRef<iDecalManager> decalManager;
    csRef<iDecalTemplate> decalTemplate;
    iDecal* decal;
    iMaterialList* materialList;
    iMaterialWrapper* activeMat; //The active material at any given time.

    //TerrainModifier Variables
    csRef<iTerrainModifier> modifier;
    csVector3 lastPosition;

    // Various event ids
    iEventNameRegistry* nameRegistry;
    csStringID activateObject; 
    
    enum {
      idMainEditor = 42,
      idSecondaryEditor,
      idButtonAddCell,
      idCellList,
      idButtonDeleteCell,
      idMatList,
      idButtonCreateMeshGenerator,
      idButtonDefaultMode,
      idButtonPaintMode,
      idButtonModifierMode      
      };

    DECLARE_EVENT_TABLE ();

///*
/*
  Event Listener class that helps in implementing Mouse Events.
*/
///
    class EventListener : public scfImplementation1<EventListener, iEventHandler>
    {
    public:
      EventListener (CSTerrainEditSpace* editor);

      //-- iEventHandler
      bool HandleEvent (iEvent &event);

    private:
      CSTerrainEditSpace* editor;

      CS_EVENTHANDLER_NAMES ("crystalspace.editor.space.terraineditor.mouse");
  
      virtual const csHandlerID * GenericPrec (csRef<iEventHandlerRegistry> &r1, 
					       csRef<iEventNameRegistry> &r2, csEventID event) const 
      {
	static csHandlerID precConstraint[1];
	precConstraint[0] = CS_HANDLERLIST_END;
	return precConstraint;
      }

      virtual const csHandlerID * GenericSucc (csRef<iEventHandlerRegistry> &r1, 
					       csRef<iEventNameRegistry> &r2, csEventID event) const 
      {
	static csHandlerID precConstraint[2];
	precConstraint[0] = r1->GetGenericID("crystalspace.utilities.cameramanager");
	precConstraint[1] = CS_HANDLERLIST_END;
	return precConstraint;
      }

      CS_EVENTHANDLER_DEFAULT_INSTANCE_CONSTRAINTS;
    };
    csRef<EventListener> eventListener;
  };



///*
/*
  ModifiableListener implementation to add Value changed events to Modifiers.
*/
///

  class ModifiableListener
: public scfImplementation1<ModifiableListener, CS::Utility::iModifiableListener>
{
public:
  ModifiableListener ()
    : scfImplementationType (this) {}

  virtual void ValueChanged (CS::Utility::iModifiable* modifiable, size_t parameterIndex);
};

}

CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif
