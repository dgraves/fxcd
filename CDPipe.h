/* CDPipe.h
 * Copyright (C) 2004 Dustin Graves <dgraves@computer.org>
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

#ifndef _CDPIPE_H_
#define _CDPIPE_H_

class CDPipe
{
protected:
#ifdef WIN32
  HANDLE pipes[2];
#else
  FXint pipes[2];
#endif
public:
  //Construct an anonymous pipe
  CDPipe();

  //Write data to pipe
  FXbool write(FXint data);

  //Read data from pipe
  FXbool read(FXint* data);

  //Get waitable object for reading data
  FXInputHandle getReadHandle();

  //Destructor
  ~CDPipe();
};

#endif
