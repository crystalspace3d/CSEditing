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
#ifndef __COMPONENTS_MODIFIABLE_H__
#define __COMPONENTS_MODIFIABLE_H__

#include <csutil/csbaseeventh.h>
#include <csutil/scf_implementation.h>
#include <iutil/comp.h>
#include <ivaria/reporter.h>

#include "cseditor/modifiableeditor.h"
#include "ieditor/editor.h"
#include "ieditor/component.h"
#include "ieditor/panel.h"

using namespace CS::Utility;
using namespace CSE::Editor::Core;
using namespace CSE::Editor::Context;
using namespace CSE::Editor::Utility;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

class ModifiablePanelFactory;

class ModifiableManager
  : public wxEvtHandler,
  public scfImplementation2<ModifiableManager, iEditorComponent,
  iModifiableListener>,
  public csBaseEventHandler
{
public:
  ModifiableManager (iBase* parent);
  virtual ~ModifiableManager();

  //-- iEditorComponent
  virtual bool Initialize (iEditor* editor);
  virtual void Update ();
  virtual void Save (iDocumentNode* node) const;
  virtual bool Load (iDocumentNode* node);
  
  //-- iEventHandler
  virtual bool HandleEvent (iEvent &event);

  //-- iModifiableListener
  virtual void ValueChanged (iModifiable* modifiable, size_t parameterIndex);

private:
  void Unregister ();
  iModifiable* FindModifiable (iObject* object);

private:
  iEditor* editor;
  iObject* activeObject;
  csRef<iModifiable> modifiable;
  csRefArray<ModifiablePanelFactory> factories;
  csRef<iModifiableDescription> description;
  csRef<iModifiable> viewmeshModifiable;
  csRef<iModifiableDescription> viewmeshDescription;
  bool listening;

  friend class ModifiablePanel;
  friend class ModifiablePanelFactory;
};

class ModifiablePanelFactory
  : public scfImplementation1<ModifiablePanelFactory, iPanelFactory>
{
public:
  ModifiablePanelFactory
    (ModifiableManager* manager, iModifiableDescription* description, size_t offset);
  ~ModifiablePanelFactory ();

  //-- iPanelFactory
  virtual csPtr<iPanel> CreateInstance ();
  virtual const char* GetIdentifier () const;
  virtual const char* GetLabel () const;
  virtual const char* GetSpace () const;

private:
  ModifiableManager* manager;
  iModifiableDescription* description;
  size_t offset;

  friend class ModifiablePanel;
};

class ModifiablePanel
  : public scfImplementation1<ModifiablePanel, iPanel>
{
public:
  ModifiablePanel (ModifiablePanelFactory* factory);

  //-- iPanel
  virtual bool Initialize (iObjectRegistry* obj_reg, iEditor* editor,
			   iPanelFactory* factory);
  virtual iPanelFactory* GetFactory () const;
  virtual bool PollVisible (iContext* context) const;
  virtual bool PollRedraw (iContext* context) const;
  virtual void Draw (iContext* context, iLayout* layout);
  virtual void Save (iDocumentNode* node) const;
  virtual bool Load (iDocumentNode* node);

private:
  ModifiablePanelFactory* factory;
  ModifiableEditor* modifiableEditor;
};

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif // __COMPONENTS_MODIFIABLE_H__
