/* CDdefs.h
 * Copyright (C) 2004,2009-2010 Dustin Graves <dgraves@computer.org>
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

#ifndef _CDDEFS_H_
#define _CDDEFS_H_

#include <vector>
#include <list>

//Version number - for windows
#define PROG_VERSION "1.0.0"
#define PROG_PACKAGE "fxcd"

class CDBMPIcon;

typedef std::vector<CDBMPIcon*> CDBMPArray;
typedef std::vector<FXWindow*>  FXWinArray;

class CDInfoTask;

typedef std::list<CDInfoTask*> CDInfoTaskList;

#endif
