/* CDutils.cpp
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

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "fox/fx.h"
#include "cdlyte.h"
#include "CDutils.h"

FXbool checkDevice(const FXString& device)
{
  if(device.empty())
    return FALSE;

#ifdef WIN32
  if(device.len()<2)||!ischar(device[0])||device[1]!=':')
    return FALSE;

  FXString drive=FXStringFormat("%c:\\",devnam[0]);
  if(GetDriveType(drive)!=DRIVE_CDROM)
    return FALSE;
#else
  cddesc_t cd;
  struct disc_info di;
  struct stat st;
  if(stat(device.text(),&st)!=0||!(S_ISBLK(st.st_mode)||S_ISCHR(st.st_mode)))
    return FALSE;

  cd=cd_init_device((char*)device.text());
  if(cd<0)
    return FALSE;

  cd_init_disc_info(&di);
  if(cd_stat(cd,&di)<0)
  {
    cd_free_disc_info(&di);
    return FALSE;
  }
  cd_free_disc_info(&di);
#endif

  return TRUE;
}

void scanDevices(std::vector<FXString>& devices)
{
  FXString device;
#ifdef WIN32
  FXint;
  for(i='A';i<='Z';i++)
  {
    device=FXStringFormat("%c:\\",i);
    if(GetDriveType(device)==DRIVE_CDROM)
    {
      devices.push_back(device);
    }
  }
#else
  std::vector<FXString> actual;

  // Check for the followinf devices: /dev/cdrom, /dev/dvd, /dev/cdrw
  // /dev/hd?, /dev/scd?, /dev/sr?, /dev/cd?, /dev/cdroms/cdrom?
  // /dev/cd?c, /dev/acd?c, /dev/matcd?c, /dev/mcd?c, /dev/scd?c, /dev/rsr?c
#endif
}
