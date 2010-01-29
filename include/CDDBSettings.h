/* CDDBSettings.h
 * Copyright (C) 2009-2010 Dustin Graves <dgraves@computer.org>
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

#ifndef _CDDBSETTINGS_H_
#define _CDDBSETTINGS_H_

enum
{
  CDDB_PROTOCOL_CDDBP=0,
  CDDB_PROTOCOL_HTTP=1
};

class CDDBSettings
{
public:
  FXbool proxy;
  FXuint cddbproto;
  FXushort cddbpport;
  FXushort cddbport;
  FXushort proxyport;
  FXString cddbaddr;
  FXString proxyaddr;
  FXString cddbexec;
  FXbool promptmultiple;  // Prompt for user selection when multiple matches exist
  FXbool localcopy;
public:
  CDDBSettings();
  CDDBSettings(const CDDBSettings& settings);
};

#endif
