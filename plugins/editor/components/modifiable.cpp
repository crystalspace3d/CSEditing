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
  : scfImplementationType (this, parent), activeObject (nullptr)
{
}

ModifiableManager::~ModifiableManager()
{
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
  csRef<iContextObjectSelection> objectSelection =
    scfQueryInterface<iContextObjectSelection> (editor->GetContext ());
  if (!objectSelection) return false;

  // Check that the active object of the context has changed
  if (activeObject == objectSelection->GetActiveObject ())
    return false;

  // Unregister the previous panels
  if (activeObject) Unregister ();

  // Search for a iModifiable interface on the active object
  activeObject = objectSelection->GetActiveObject ();
  if (!activeObject) return false;

  csRef<CS::Utility::iModifiable> modifiable =
    scfQueryInterface<CS::Utility::iModifiable> (activeObject);

  csRef<iMeshFactoryWrapper> meshFactory =
    scfQueryInterface<iMeshFactoryWrapper> (activeObject);
  if (meshFactory)
    modifiable = scfQueryInterface<CS::Utility::iModifiable>
      (meshFactory->GetMeshObjectFactory ());
  if (!modifiable) return false;

  factory.AttachNew (new ModifiablePanelFactory (this, modifiable, "Test"));
  editor->GetComponentManager ()->RegisterPanel (factory);

  return false;
}

void ModifiableManager::Unregister ()
{
  if (factory) editor->GetComponentManager ()->UnregisterPanel (factory);
}

//------------------------------------  ModifiablePanelFactory  ------------------------------------

ModifiablePanelFactory::ModifiablePanelFactory
(ModifiableManager* manager, CS::Utility::iModifiable* modifiable, const char* label)
  : scfImplementationType (this), manager (manager), modifiable (modifiable), label (label)
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
  return label;
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
  modifiableEditor->SetModifiable (factory->modifiable);
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

