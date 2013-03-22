/*
    Copyright (C) 2012 by Christian Van Brussel

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
#ifndef __ICOMPONENT_LOGGER_H__
#define __ICOMPONENT_LOGGER_H__

/**\file 
 * Logger component for the management of system report messages.
 */

/**
 * \addtogroup editorapp
 * @{ */

#include <csutil/scf_interface.h>

class wxTextCtrl;
class wxWindow;

namespace CSE {
namespace Editor {
namespace Component {

/**
 * Logger component for the management of system report messages.
 */
struct iLogger : public virtual iBase
{
  SCF_INTERFACE (iLogger, 1, 0, 0);

  /// Set the maximum amount of reports that will be held in memory.
  virtual void SetMaximumReportCount (size_t count) = 0;

  /// Get the maximum amount of reports that will be held in memory.
  virtual size_t GetMaximumReportCount () const = 0;

  /// Create a text control widget for the display of the reports.
  virtual wxTextCtrl* CreateTextCtrl (wxWindow* parent) = 0;
};

} // namespace Component
} // namespace Editor
} // namespace CSE

/** @} */

#endif // __ICOMPONENT_LOGGER_H__
