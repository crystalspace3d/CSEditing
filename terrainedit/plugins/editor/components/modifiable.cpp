/*
    Copyright (C) 2013 by Christian Van Brussel
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
#include <csutil/scf.h>
#include <csutil/stringquote.h>
#include <iengine/engine.h>
#include <iengine/mesh.h>
#include <iutil/modifiable.h>
#include <iutil/object.h>

#include <wx/window.h>

#include "icontext/objectselection.h"
#include "ieditor/context.h"
#include "ieditor/layout.h"

#include "modifiable.h"

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

//------------------------------------  ModifiableManager  ------------------------------------

SCF_IMPLEMENT_FACTORY (ModifiableManager)

ModifiableManager::ModifiableManager (iBase* parent)
  : scfImplementationType (this, parent), activeObject (nullptr), listening (false)
{
}

ModifiableManager::~ModifiableManager()
{
  // Unregister all panel factories
  if (modifiable) Unregister ();
}

bool ModifiableManager::Initialize (iEditor* editor)
{
  this->editor = editor;

  // Register to the 'setactiveobject' context events
  csRef<iEventNameRegistry> registry =
    csQueryRegistry<iEventNameRegistry> (editor->GetContext ()->GetObjectRegistry ());
  RegisterQueue (editor->GetContext ()->GetEventQueue (),
		 registry->GetID ("crystalspace.editor.context.selection.setactiveobject"));

  return true;
}

void ModifiableManager::Update ()
{
}

void ModifiableManager::Save (iDocumentNode* node) const
{
}

bool ModifiableManager::Load (iDocumentNode* node)
{
  return false;
}

bool ModifiableManager::HandleEvent (iEvent &event)
{
  // Check if there is an active object
  csRef<iContextObjectSelection> objectSelection =
    scfQueryInterface<iContextObjectSelection> (editor->GetContext ());
  if (!objectSelection) return false;

  if (!objectSelection->GetActiveObject ()) return false;

  if (activeObject == objectSelection->GetActiveObject ())
    return false;
  activeObject = objectSelection->GetActiveObject ();

  // Unregister and delete the previous panel factories
  if (modifiable) Unregister ();

  // Search for a iModifiable interface on the active object
  modifiable = FindModifiable (activeObject);
  if (!modifiable) return false;

  // Create an editor for the modifiable
  description = modifiable->GetDescription (editor->GetContext ()->GetObjectRegistry ());
  // TODO: why an IncRef() here?
  description->IncRef ();

  csRef<ModifiablePanelFactory> factory;
  factory.AttachNew (new ModifiablePanelFactory (this, description, 0));
  editor->GetComponentManager ()->RegisterPanel (factory);
  factories.Push (factory);

  // Create an editor for all child modifiable
  size_t offset = description->GetParameterCount ();
  for (size_t i = 0; i < description->GetChildrenCount (); i++)
  {
    iModifiableDescription* childDescription = description->GetChild (i);
    // TODO: why an IncRef() here?
    childDescription->IncRef ();
    factory.AttachNew
      (new ModifiablePanelFactory (this, childDescription, offset));
    editor->GetComponentManager ()->RegisterPanel (factory);
    factories.Push (factory);
    offset += childDescription->GetParameterCount ();
  }

  // If we are in 'viewmesh' mode, then attempt to find the iMovable of the child
  // mesh
  csRef<iMeshFactoryWrapper> meshFactory =
    scfQueryInterface<iMeshFactoryWrapper> (activeObject);
  if (!meshFactory) return false;

  csRef<iEngine> engine = csQueryRegistry<iEngine>
    (editor->GetContext ()->GetObjectRegistry ());
  iMeshWrapper* mesh = engine->FindMeshObject ("viewmesh");
  if (!mesh) return false;

  if (mesh->GetMeshObject ()->GetFactory ()
      != meshFactory->GetMeshObjectFactory ())
    return false;

  viewmeshModifiable = scfQueryInterface<iModifiable> (mesh->GetMeshObject ());
  if (!viewmeshModifiable) return false;

  viewmeshDescription = viewmeshModifiable->GetDescription
    (editor->GetContext ()->GetObjectRegistry ());

  // Listen to the state changes of this modifiable
  modifiable->AddListener (this);
  listening = true;

  return false;
}

void ModifiableManager::ValueChanged (iModifiable* modifiable, size_t parameterIndex)
{
  // Apply the value change to the child mesh
  if (!viewmeshModifiable) return;

  csStringID id = description->GetParameter (parameterIndex)->GetID ();
  size_t index = viewmeshDescription->FindParameter (id);
  if (index == (size_t) ~0)
  {
#ifdef CS_DEBUG
    csReport (editor->GetContext ()->GetObjectRegistry (),
	      CS_REPORTER_SEVERITY_WARNING,
	      "crystalspace.editor.component.modifiable",
	      "Could not find a parameter %s in the child mesh",
	      CS::Quote::Single (description->GetParameter (parameterIndex)->GetLabel ()));
#endif
    return;
  }

  csVariant value;
  modifiable->GetParameterValue (parameterIndex, value);
  viewmeshModifiable->SetParameterValue (index, value);
}

void ModifiableManager::Unregister ()
{
  // Unregister and delete the panel factories
  for (size_t i = 0; i < factories.GetSize (); i++)
    editor->GetComponentManager ()->UnregisterPanel (factories[i]);
  factories.DeleteAll ();

  // Invalidate the viewmesh modifiable
  viewmeshModifiable.Invalidate ();
  viewmeshDescription.Invalidate ();

  // Unregister the modifiable listener
  if (listening)
  {
    modifiable->RemoveListener (this);
    listening = false;
  }
}

iModifiable* ModifiableManager::FindModifiable (iObject* object)
{
  csRef<iModifiable> modifiable = 
    scfQueryInterface<iModifiable> (object);
  if (modifiable) return modifiable;

  csRef<iMeshFactoryWrapper> meshFactory =
    scfQueryInterface<iMeshFactoryWrapper> (object);
  if (meshFactory)
    modifiable = scfQueryInterface<iModifiable>
      (meshFactory->GetMeshObjectFactory ());
  if (modifiable) return modifiable;

  csRef<iMeshWrapper> mesh =
    scfQueryInterface<iMeshWrapper> (object);
  if (mesh)
    modifiable = scfQueryInterface<iModifiable>
      (mesh->GetMeshObject ());
  if (modifiable) return modifiable;

  return nullptr;
}

//------------------------------------  ModifiablePanelFactory  ------------------------------------

ModifiablePanelFactory::ModifiablePanelFactory
(ModifiableManager* manager, iModifiableDescription* description, size_t offset)
  : scfImplementationType (this), manager (manager), description (description),
  offset (offset)
{}

ModifiablePanelFactory::~ModifiablePanelFactory ()
{}

csPtr<iPanel> ModifiablePanelFactory::CreateInstance ()
{
  return new ModifiablePanel (this);
}

const char* ModifiablePanelFactory::GetIdentifier () const
{
  return "crystalspace.editor.panel.modifiable";
}

const char* ModifiablePanelFactory::GetLabel () const
{
  return description->GetName ();
}

const char* ModifiablePanelFactory::GetSpace () const
{
  return "crystalspace.editor.space.properties";
}

//------------------------------------  ModifiablePanel  ------------------------------------

ModifiablePanel::ModifiablePanel (ModifiablePanelFactory* factory)
  : scfImplementationType (this), factory (factory), modifiableEditor (nullptr)
{}

bool ModifiablePanel::Initialize (iObjectRegistry* obj_reg, iEditor* editor,
				  iPanelFactory* factory)
{
  return true;
}

iPanelFactory* ModifiablePanel::GetFactory () const
{
  return factory;
}

bool ModifiablePanel::PollVisible (iContext* context) const
{
  return true;
}

bool ModifiablePanel::PollRedraw (iContext* context) const
{
  return false;
}

void ModifiablePanel::Draw (iContext* context, iLayout* layout)
{
  modifiableEditor = new ModifiableEditor
    (factory->manager->editor->GetContext ()->GetObjectRegistry (),
     layout->GetwxWindow (), wxID_ANY, wxDefaultPosition,
     layout->GetwxWindow ()->GetSize (), 0L, wxT ("Modifiable editor"));
  modifiableEditor->SetDescriptionEnabled (false);
  modifiableEditor->SetRecursive (false);
  modifiableEditor->SetModifiable
    (factory->manager->modifiable, factory->offset, factory->description);
  layout->AppendWindow (modifiableEditor);
}

void ModifiablePanel::Save (iDocumentNode* node) const
{
}

bool ModifiablePanel::Load (iDocumentNode* node)
{
  return true;
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

