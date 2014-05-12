/*
  Copyright (C) 2013 by Christian Van Brussel

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
#ifndef CSE_NAMESPACEDOCS_H
#define CSE_NAMESPACEDOCS_H

/** \file
 * Documentation for namespaces in CSEditing
 */

/**
 * Main namespace for the Crystal Space editing environment
 */
namespace CSEE
{
  /**
   * Graphical User Interface tools and components for the edition of Crystal
   * Space objects.
   */
  namespace Editor
  {
    /**
     * Default components for the cseditor application.
     */
    namespace Component
    {}

    /**
     * Default context interfaces for the cseditor application.
     */
    namespace Context
    {}

    /**
     * Interfaces of the core cseditor framework.
     */
    namespace Core
    {}

    /**
     * Generic tools for GUI management.
     */
    namespace Utility
    {}
  }
}

#endif
