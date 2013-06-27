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
#include <csutil/scf.h>
#include <iengine/camera.h>
#include <iengine/sector.h>
#include <iutil/object.h>
#include <iutil/objreg.h>
#include <iutil/plugin.h>

#include "icontext/camera.h"
#include "ieditor/context.h"
#include "ieditor/space.h"
#include "ieditor/layout.h"

#include "camerapanel.h"

using namespace CSE::Editor::Context;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{
  
SCF_IMPLEMENT_FACTORY (CameraPanel)

CameraPanel::CameraPanel (iBase* parent) : scfImplementationType (this, parent)
{
}

CameraPanel::~CameraPanel ()
{
}

bool CameraPanel::Initialize (iObjectRegistry* obj_reg, iEditor* editor,
			      iPanelFactory* factory)
{
  this->factory = factory;
  return true;
}

bool CameraPanel::Poll (iContext* context)
{
  csRef<iContextCamera> cameraContext = scfQueryInterface<iContextCamera> (context);
  return cameraContext->GetCamera ();
}

bool CameraPanel::PollVisible (iContext* context) const
{
  csRef<iContextCamera> cameraContext = scfQueryInterface<iContextCamera> (context);
  return cameraContext->GetCamera ();
}

bool CameraPanel::PollRedraw (iContext* context) const
{
  return false;
}

void CameraPanel::Draw (iContext* context, iLayout* layout)
{
  csString ori;
  csRef<iContextCamera> cameraContext = scfQueryInterface<iContextCamera> (context);
  const csVector3 & origin = cameraContext->GetCamera ()->GetTransform ().GetOrigin ();
  ori.Format ("Origin: X:%f Y:%f Z:%f", origin[0], origin[1], origin[2]);
  layout->AppendLabel (ori.GetData ());

  csString sect;
  if (cameraContext->GetCamera ()->GetSector ())
    sect.Format ("Sector: %s", cameraContext->GetCamera ()->GetSector ()->QueryObject ()->GetName ());
  else sect = "Sector: none";
  layout->AppendLabel (sect.GetData ());
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)
