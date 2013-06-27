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
#include <cssysdef.h>

#include <csutil/objreg.h>
#include <csutil/weakrefarr.h>
#include <iutil/document.h>
#include <iutil/plugin.h>
#include <ivaria/reporter.h>

#include "ieditor/component.h"
#include "ieditor/context.h"
#include "ieditor/header.h"
#include "ieditor/layout.h"
#include "ieditor/menu.h"
#include "ieditor/operator.h"

#include "editor.h"
#include "layouts.h"
#include "spacemanager.h"
#include "window.h"

#include <wx/button.h>
#include <wx/colordlg.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>

#include "data/editor/images/sceneIcon.xpm"  

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

SpaceFactory::SpaceFactory (ComponentManager* manager)
  : scfImplementationType (this), manager (manager), icon (sceneIcon_xpm),
  allowMultiple (true)
{
}

const char* SpaceFactory::GetIdentifier () const
{
  return identifier;
}

const char* SpaceFactory::GetLabel () const
{
  return label;
}

const wxBitmap& SpaceFactory::GetIcon () const
{
  return icon;
}

bool SpaceFactory::GetMultipleAllowed () const
{
  return allowMultiple;
}

csPtr<iSpace> SpaceFactory::CreateInstance (wxWindow* parent)
{
  csRef<iBase> base = iSCF::SCF->CreateInstance (identifier);
  if (!base)
  {
    if (!iSCF::SCF->ClassRegistered  (identifier))
      csReport (manager->editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.spacefactory",
		"The space component %s is not registered to SCF",
		CS::Quote::Single (identifier));

    else csReport (manager->editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		   "crystalspace.editor.core.spacefactory",
		   "Failed to instantiate space factory %s",
		   CS::Quote::Single (identifier.GetData ()));

    return csPtr<iSpace> (nullptr);
  }

  csRef<iSpace> ref = scfQueryInterface<iSpace> (base);
  if (!ref)
  {
    csReport (manager->editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.core.spacefactory",
	      "The instanciation of the space factory %s is not of type iSpace",
	      CS::Quote::Single (identifier.GetData ()));
    return csPtr<iSpace> (nullptr);
  }

  base->DecRef ();
  ref->Initialize (manager->editor->manager->object_reg, manager->editor, this, parent);

  SpaceEntry entry;
  entry.space = ref;

  for (size_t i = 0; i < manager->spaceData.GetSize (); i++)
    if (manager->spaceData[i].factory == this)
    {
      const ComponentManager::SpaceData& data = manager->spaceData[i];
      for (size_t j = 0; j < data.panelFactories.GetSize (); j++)
      {
	PanelEntry panelEntry;
	panelEntry.panel = data.panelFactories[j]->CreateInstance ();
	entry.panels.Push (panelEntry);
      }
    }

  spaces.Push (entry);

  return csPtr<iSpace> (ref);
}

size_t SpaceFactory::GetCount ()
{
  Compact ();
  return spaces.GetSize ();
}

size_t SpaceFactory::GetEnabledCount ()
{
  Compact ();
  size_t count = 0;
  for (size_t i = 0; i < spaces.GetSize (); i++)
    if (spaces[i].space && spaces[i].space->GetEnabled ())
      count++;
  return count;
}

void SpaceFactory::Compact ()
{
  for (size_t i = spaces.GetSize (); i > 0 ; i--)
    if (!spaces[i - 1].space)
      spaces.DeleteIndexFast (i - 1);
}

//----------------------------------------------------------------------

PanelFactory::PanelFactory (Editor* editor)
   : scfImplementationType (this), editor (editor)
 {}

const char* PanelFactory::GetIdentifier () const
{
  return identifier;
}

const char* PanelFactory::GetLabel () const
{
  return label;
}

const char* PanelFactory::GetSpace () const
{
  return space;
}

csPtr<iPanel> PanelFactory::CreateInstance ()
{
  csRef<iBase> base = iSCF::SCF->CreateInstance (identifier);
  if (!base)
  {
    if (!iSCF::SCF->ClassRegistered  (identifier))
      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.panelfactory",
		"The panel component %s is not registered to SCF",
		CS::Quote::Single (identifier));

    else csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		   "crystalspace.editor.core.panelfactory",
		   "Failed to instantiate panel factory %s",
		   CS::Quote::Single (identifier.GetData ()));

    return csPtr<iPanel> (nullptr);
  }

  csRef<iPanel> ref = scfQueryInterface<iPanel> (base);
  if (!ref)
  {
    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.core.panelfactory",
	      "The instanciation of the panel factory %s is not of type iPanel",
	      CS::Quote::Single (identifier.GetData ()));
    return csPtr<iPanel> (nullptr);
  }

  base->DecRef ();
  ref->Initialize (editor->manager->object_reg, editor, this);

  return csPtr<iPanel> (ref);
}

//----------------------------------------------------------------------

ComponentManager::ComponentManager (Editor* editor)
  : scfImplementationType (this), editor (editor)
{
  csRef<iEventNameRegistry> registry =
    csQueryRegistry<iEventNameRegistry> (editor->manager->object_reg);
  RegisterQueue (editor->context->GetEventQueue (),
		 registry->GetID ("crystalspace.editor.context"));
}

ComponentManager::~ComponentManager ()
{
}

bool ComponentManager::RegisterComponent (const char* pluginName)
{
  // Check if this component is already registered
  if (components.Contains (pluginName))
  {
    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_WARNING,
	      "crystalspace.editor.core.componentmanager",
	      "The editor component %s is already registered",
	      CS::Quote::Single (pluginName));
    return true;
  }

  // Create the editor component
  csRef<iBase> base = iSCF::SCF->CreateInstance (pluginName);
  if (!base)
  {
    if (!iSCF::SCF->ClassRegistered  (pluginName))
      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.componentmanager",
		"The editor component %s is not registered to SCF",
		CS::Quote::Single (pluginName));

    else csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		   "crystalspace.editor.core.componentmanager",
		   "Failed to instantiate editor component %s",
		   CS::Quote::Single (pluginName));

    return false;
  }

  csRef<iEditorComponent> ref = scfQueryInterface<iEditorComponent> (base);
  if (!ref)
  {
    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.core.componentmanager",
	      "The instanciation of the editor component %s is not of type iEditorComponent",
	      CS::Quote::Single (pluginName));
    return false;
  }

  base->DecRef ();
  if (!ref->Initialize (editor))
    return false;

  components.PutUnique (pluginName, ref);

  csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_NOTIFY,
	    "crystalspace.editor.core.componentmanager",
	    "Editor component %s registered",
	    CS::Quote::Single (pluginName));

  return true;
}

bool ComponentManager::RegisterSpace (const char* pluginName)
{
  // Check if this space is already registered
  for (size_t i = 0; i < spaceData.GetSize (); i++)
    if (spaceData[i].factory->identifier == pluginName)
    {
      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_WARNING,
		"crystalspace.editor.core.componentmanager",
		"The space %s is already registered",
		CS::Quote::Single (pluginName));
      return true;
    }

  // Create the space factory
  csRef<iDocumentNode> klass = iSCF::SCF->GetPluginMetadataNode (pluginName);
  if (klass)
  {
    SpaceData data;
    data.factory.AttachNew (new SpaceFactory (this));
    data.factory->identifier = pluginName;
    csRef<iDocumentNode> m = klass->GetNode ("allowMultiple");
    if (m) data.factory->allowMultiple = strcmp (m->GetContentsValue (), "true") == 0;
    csRef<iDocumentNode> label = klass->GetNode ("description");
    if (label) data.factory->label = label->GetContentsValue ();
      
    spaceData.Push (data);

    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_NOTIFY,
	      "crystalspace.editor.core.componentmanager",
	      "Editor space %s registered",
	      CS::Quote::Single (pluginName));

    return true;
  }

  // Report for the missing plugin metadata node
  if (!iSCF::SCF->ClassRegistered  (pluginName))
    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.core.componentmanager",
	      "The space component %s is not registered to SCF",
	      CS::Quote::Single (pluginName));

  else csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		 "crystalspace.editor.core.componentmanager",
		 "No SCF metadata found for the space component %s",
		 CS::Quote::Single (pluginName));

  return false;
}

bool ComponentManager::RegisterHeader (const char* pluginName)
{
  csRef<iBase> base = iSCF::SCF->CreateInstance (pluginName);
  if (!base)
  {
    if (!iSCF::SCF->ClassRegistered  (pluginName))
      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.componentmanager",
		"The header component %s is not registered to SCF",
		CS::Quote::Single (pluginName));

    else csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		   "crystalspace.editor.core.componentmanager",
		   "Failed to instantiate header component %s",
		   CS::Quote::Single (pluginName));

    return false;
  }

  csRef<iHeader> header = scfQueryInterface<iHeader> (base);
  if (!header)
  {
    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.core.componentmanager",
	      "The instanciation of the header component %s is not of type iHeader",
	      CS::Quote::Single (pluginName));
    return false;
  }

  base->DecRef ();

  csRef<iDocumentNode> klass = iSCF::SCF->GetPluginMetadataNode (pluginName);
  if (klass)
  {
    csRef<iDocumentNode> space = klass->GetNode ("space");
    if (!space)
    {
      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.componentmanager",
		"No space defined in the plugin metadata of the header %s",
		CS::Quote::Single (pluginName));
      return false;
    }

    // Search for the associated space factory
    for (size_t i = 0; i < spaceData.GetSize (); i++)
      if (spaceData[i].factory->identifier == space->GetContentsValue ())
      {
	// Check that a previous header is not already registered
	if (spaceData[i].header)
	{
	  csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		    "crystalspace.editor.core.componentmanager",
		    "The registration of the header %s failed because another header "
		    "component was already registered for the space %s",
		    CS::Quote::Single (pluginName), CS::Quote::Single (space->GetContentsValue ()));
	  return false;
	}

	// Register the header
	spaceData[i].header = header;

	csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_NOTIFY,
		  "crystalspace.editor.core.componentmanager",
		  "Editor header %s registered",
		  CS::Quote::Single (pluginName));

	return true;
      }

    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.core.componentmanager",
	      "The space defined in the plugin metadata of the header %s is not valid (%s)",
	      CS::Quote::Single (pluginName), CS::Quote::Single (space->GetContentsValue ()));
    return false;
  }

  csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	    "crystalspace.editor.core.componentmanager",
	    "Failed to register header");
  return false;
}

bool ComponentManager::RegisterPanel (const char* pluginName)
{
  // Create the panel factory
  csRef<iDocumentNode> klass = iSCF::SCF->GetPluginMetadataNode (pluginName);
  if (klass)
  {
    csRef<iDocumentNode> space = klass->GetNode ("space");
    if (!space)
    {
      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.componentmanager",
		"No space defined in the plugin metadata of the panel %s",
		CS::Quote::Single (pluginName));
      return false;
    }

    // Search for the associated space factory
    for (size_t i = 0; i < spaceData.GetSize (); i++)
      if (spaceData[i].factory->identifier == space->GetContentsValue ())
      {
	// Check if this panel is already registered
	for (size_t j = 0; j < spaceData[i].panelFactories.GetSize (); j++)
	  if (!strcmp (spaceData[i].panelFactories[j]->GetIdentifier (), pluginName))
	  {
	    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_WARNING,
		      "crystalspace.editor.core.componentmanager",
		      "The panel %s is already registered",
		      CS::Quote::Single (pluginName));
	    return true;
	  }

	// Create the panel factory
	csRef<PanelFactory> factory;
	factory.AttachNew (new PanelFactory (editor));
	factory->identifier = pluginName;
	factory->space = space->GetContentsValue ();
	csRef<iDocumentNode> label = klass->GetNode ("description");
	if (label) factory->label = label->GetContentsValue ();

	spaceData[i].panelFactories.Push (factory);

	csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_NOTIFY,
		  "crystalspace.editor.core.componentmanager",
		  "Editor panel %s registered",
		  CS::Quote::Single (pluginName));

	return true;
      }

    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.core.componentmanager",
	      "The space defined in the plugin metadata of the header %s is not valid (%s)",
	      CS::Quote::Single (pluginName), CS::Quote::Single (space->GetContentsValue ()));
    return false;
  }

  // Report for the missing plugin metadata node
  if (!iSCF::SCF->ClassRegistered  (pluginName))
    csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	      "crystalspace.editor.core.componentmanager",
	      "The panel component %s is not registered to SCF",
	      CS::Quote::Single (pluginName));

  else csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		 "crystalspace.editor.core.componentmanager",
		 "No SCF metadata found for the panel component %s",
		 CS::Quote::Single (pluginName));

  return false;
}

bool ComponentManager::RegisterPanel (iPanelFactory* panelFactory)
{
  // Search for the associated space factory
  for (size_t i = 0; i < spaceData.GetSize (); i++)
    if (spaceData[i].factory->identifier == panelFactory->GetSpace ())
    {
      spaceData[i].panelFactories.Push (panelFactory);

      // Add a panel to all active spaces
      SpaceFactory* factory = spaceData[i].factory;
      for (size_t j = 0; j < factory->spaces.GetSize (); j++)
      {
	SpaceFactory::PanelEntry panelEntry;
	panelEntry.panel = panelFactory->CreateInstance ();
	factory->spaces[j].panels.Push (panelEntry);
      }

      return true;
    }

  csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	    "crystalspace.editor.core.componentmanager",
	    "The space defined in the panel factory %s is not valid (%s)",
	    CS::Quote::Single (panelFactory->GetLabel ()), CS::Quote::Single (panelFactory->GetSpace ()));
  return false;
}

bool ComponentManager::UnregisterPanel (iPanelFactory* panel)
{
  // Search for the associated space factory
  for (size_t i = 0; i < spaceData.GetSize (); i++)
    if (spaceData[i].factory->identifier == panel->GetSpace ())
    {
      size_t index = spaceData[i].panelFactories.Find (panel);
      if (index == csArrayItemNotFound) return false;
      spaceData[i].panelFactories.DeleteIndexFast (index);

      // Remove the existing panels on all active spaces
      SpaceFactory* factory = spaceData[i].factory;
      for (size_t j = 0; j < factory->spaces.GetSize (); j++)
      {
	SpaceFactory::PanelEntry& panelEntry = factory->spaces[j].panels[index];

	// Remove the associated widget from the GUI
	if (panelEntry.collapsiblePane)
	{
	  wxWindow* window = factory->spaces[j].space->GetwxWindow ();
	  if (window && window->GetSizer ())
	    window->GetSizer ()->Detach (panelEntry.collapsiblePane);
	  delete panelEntry.collapsiblePane;
	}

	// Delete the panel
	factory->spaces[j].panels.DeleteIndexFast (index);
      }

      return true;
    }

  csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	    "crystalspace.editor.core.componentmanager",
	    "The space defined in the panel factory %s is not valid (%s)",
	    CS::Quote::Single (panel->GetLabel ()), CS::Quote::Single (panel->GetSpace ()));
  return false;
}

iSpaceFactory* ComponentManager::FindSpaceFactory (const char* pluginName, size_t& index) const
{
  for (index = 0; index < spaceData.GetSize (); index++)
    if (spaceData[index].factory->identifier == pluginName)
      return spaceData[index].factory;

  return nullptr;
}

iEditorComponent* ComponentManager::FindComponent (const char* pluginName) const
{
  return *components.GetElementPointer (pluginName);
}

bool ComponentManager::HandleEvent (iEvent &event)
{
  // Redraw all spaces
  for (size_t i = 0; i < spaceData.GetSize (); i++)
  {
    SpaceFactory* f = spaceData[i].factory;
    for (size_t j = 0; j < f->spaces.GetSize (); j++)
    {
      iSpace* space = f->spaces[j].space;
      if (space && space->GetEnabled ()) ReDraw (&f->spaces[j], &spaceData[i]);
    }
  }

  return false;
}

void ComponentManager::ReDraw (iSpace* space)
{
  if (!space || !space->GetFactory ()) return;

  for (size_t i = 0; i < spaceData.GetSize (); i++)
  {
    SpaceFactory* f = spaceData[i].factory;
    if (f->identifier == space->GetFactory ()->GetIdentifier ())
    {
      for (size_t j = 0; j < f->spaces.GetSize (); j++)
	if (f->spaces[j].space == space)
	{
	  ReDraw (&f->spaces[j], &spaceData[i]);
	  return;
	}

      csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
		"crystalspace.editor.core.spacefactory",
		"The space %s is no longer active",
		CS::Quote::Single (space->GetFactory ()->GetIdentifier ()));
    }
  }

  csReport (editor->manager->object_reg, CS_REPORTER_SEVERITY_ERROR,
	    "crystalspace.editor.core.spacefactory",
	    "Could not find the factory of the space %s",
	    CS::Quote::Single (space->GetFactory ()->GetIdentifier ()));
}

void ComponentManager::ReDraw (SpaceFactory::SpaceEntry* entry, SpaceData* spaceData)
{
  wxWindow* window = entry->space->GetwxWindow ();

  // Draw the header
  if (spaceData->header)
  {
    if (window && window->GetParent ())
    {
      ViewControl* ctrl = static_cast<ViewControl*> (window->GetParent ());
      if (ctrl)
      {
        ctrl->SetLayout (nullptr);
        csRef<iLayout> layout;
        layout.AttachNew (new HeaderLayout (editor->manager->object_reg,
					    editor, ctrl->GetRegion ()));
        ctrl->SetLayout (layout);
        spaceData->header->Draw (editor->context, layout);
        ctrl->GetRegion ()->GetParent ()->Layout ();
      }
    }
  }

  // Draw the panels
  if (window)
  {
    if (entry->panels.GetSize ())
    {
      // Create the size if needed
      wxSizer* sizer = window->GetSizer ();
      if (!sizer)
      {
	sizer = new wxBoxSizer (wxVERTICAL);
	window->SetSizer (sizer, true);
      }

      // Update the state of each panel
      for (size_t i = 0; i < entry->panels.GetSize (); i++)
      {
	SpaceFactory::PanelEntry& panelEntry = entry->panels[i];

	// Check if the panel is visible
        if (panelEntry.panel->PollVisible (editor->context))
        {
	  // Check if the panel needs to be redrawn
	  if (!panelEntry.collapsiblePane
	      || panelEntry.panel->PollRedraw (editor->context))
	  {
	    panelEntry.collapsiblePane = new CollapsiblePane
	      (editor->manager->object_reg, window, panelEntry.panel->GetFactory ()->GetLabel ());

	    csRef<iLayout> layout;
	    layout.AttachNew (new PanelLayout (editor->manager->object_reg,
					       editor, panelEntry.collapsiblePane->GetPane ()));
	    panelEntry.collapsiblePane->SetLayout (layout);
	    panelEntry.panel->Draw (editor->context, layout);

	    panelEntry.collapsiblePane->Expand ();
	    // TODO: insert at index
	    //sizer->Add (panelEntry.collapsiblePane, 0, wxGROW | wxALL, 10);
	    sizer->Add (panelEntry.collapsiblePane, 0, wxGROW | wxALL, 10);
	  }

	  else if (panelEntry.collapsiblePane)
	  {
	    // Make the panel visible
	    panelEntry.collapsiblePane->Show ();
	  }
        }

	else if (panelEntry.collapsiblePane)
	{
	  // Hide the panel
	  panelEntry.collapsiblePane->Hide ();
	}
      }

      sizer->Layout ();
    }
  }
}

void ComponentManager::Update ()
{
  // Update the editor components
  for (csHash<csRef<iEditorComponent>, csString>::GlobalIterator it =
	 components.GetIterator (); it.HasNext (); )
  {
    iEditorComponent* component = it.Next ();
    component->Update ();
  }

  // Update the spaces
  for (size_t i = 0; i < spaceData.GetSize (); i++)
  {
    SpaceFactory* f = spaceData[i].factory;
    for (size_t j = 0; j < f->spaces.GetSize (); j++)
      if (f->spaces[j].space) f->spaces[j].space->Update ();
  }
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)
