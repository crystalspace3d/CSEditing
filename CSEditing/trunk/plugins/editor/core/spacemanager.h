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
#ifndef __SPACEMANAGER_H__
#define __SPACEMANAGER_H__

#include <csutil/csbaseeventh.h>
#include <csutil/hash.h>
#include <csutil/refarr.h>

#include "ieditor/context.h"
#include "ieditor/space.h"
#include "ieditor/panel.h"

#include <wx/bitmap.h>

using namespace CSE::Editor::Core;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

  // TODO: split/rename file in components/manager.h/cpp

class CollapsiblePane;
class ComponentManager;
class Editor;
class EditorManager;

class SpaceFactory
  : public scfImplementation1<SpaceFactory, iSpaceFactory>
{
public:
  SpaceFactory (ComponentManager* manager);

  virtual csPtr<iSpace> CreateInstance (wxWindow* parent);
  virtual const char* GetIdentifier () const;
  virtual const char* GetLabel () const;
  virtual const wxBitmap& GetIcon () const;
  virtual bool GetMultipleAllowed () const;
  virtual size_t GetCount ();
  virtual size_t GetEnabledCount ();

private:
  void Compact ();

private:
  ComponentManager* manager;
  csString identifier;
  csString label;
  wxBitmap icon;
  bool allowMultiple;

  struct PanelEntry
  {
    csRef<iPanel> panel;
    CollapsiblePane* collapsiblePane;

    PanelEntry ()
    : collapsiblePane (nullptr) {}
  };

  struct SpaceEntry
  {
    csWeakRef<iSpace> space;
    csArray<PanelEntry> panels;
  };
  csArray<SpaceEntry> spaces;

  friend class ComponentManager;
};

class PanelFactory
  : public scfImplementation1<PanelFactory, iPanelFactory>
{
public:
  PanelFactory (Editor* editor);

  virtual csPtr<iPanel> CreateInstance ();
  virtual const char* GetIdentifier () const;
  virtual const char* GetLabel () const;
  virtual const char* GetSpace () const;

private:
  Editor* editor;
  csString label;
  csString space;
  csString identifier;

  friend class ComponentManager;
};

class ComponentManager
  : public scfImplementation1<ComponentManager, iComponentManager>, public csBaseEventHandler
{
public:
  ComponentManager (Editor* editor);
  virtual ~ComponentManager ();
  
  //-- iComponentManager
  virtual bool RegisterComponent (const char* pluginName);
  virtual bool RegisterSpace (const char* pluginName);
  virtual bool RegisterHeader (const char* pluginName);
  virtual bool RegisterPanel (const char* pluginName);
  virtual bool RegisterPanel (iPanelFactory* panel);
  virtual bool UnregisterPanel (iPanelFactory* panel);

  virtual iEditorComponent* FindComponent (const char* pluginName) const;
  virtual iSpaceFactory* FindSpaceFactory (const char* pluginName, size_t& index) const;
  
  //-- iEventHandler
  bool HandleEvent (iEvent &event);

  void Update ();
  void ReDraw (iSpace* space);

public:
  Editor* editor;
  struct SpaceData
  {
    csRef<SpaceFactory> factory;
    csRefArray<iPanelFactory> panelFactories;
    csRef<iHeader> header;
  };
  csArray<SpaceData> spaceData;

private:
  csHash<csRef<iEditorComponent>, csString> components;

private:
  void ReDraw (SpaceFactory::SpaceEntry* entry, SpaceData* spaceData);
};

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif
