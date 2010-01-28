/* CDInfo.h
 * Copyright (C) 2001,2004,2009-2010 Dustin Graves <dgraves@computer.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef _CDINFO_H_
#define _CDINFO_H_

struct CDData;

enum
{
  CDINFO_IDLE     = 0, // No operation has been performed
  CDINFO_PENDING  = 1, // Operation in progress
  CDINFO_INTERACT = 2, // Operation requires user input to complete
  CDINFO_DONE     = 3, // Operation completed successfully
  CDINFO_ERROR    = 4  // Error encountered while retreiving data
};

class CDInfo
{
public:
  // Get status
  virtual FXuint getStatus() const = 0;

  // Get error string
  virtual FXString getErrorString() const = 0;

  // Request disc info; returns true if request will be handled and false if request can not be processed
  virtual FXbool requestData() = 0;

  // Fill existing CDData object with disc info; returns true if data is available and false if data is unavailable 
  // due to retrieval error, etc
  virtual FXbool getData(CDData* data) = 0;

  // Reset the object's state to idle
  virtual FXbool reset() = 0;

  // Request user input through custom process defined for the object (for objects which may require user interaction)
  virtual FXbool getUserInput(FXWindow* owner = 0) = 0;

  // Get FXGUISignal object used to synchronize threaded info retrieval
  virtual FXGUISignal* getGUISignal() const = 0;

  // Destructor
  virtual ~CDInfo() { }
};

#endif
