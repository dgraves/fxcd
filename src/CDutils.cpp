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

#include <algorithm>
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
  FXint i;
  FXString device;
#ifdef WIN32
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
  FXint j;
  FXbool exists;
  FXString link;
  FXchar *special[]={"/dev/cdrom","/dev/dvd","/dev/cdrw",NULL};
  FXchar *letters[]={"/dev/hd%d",NULL};
  FXchar *numbers[]={"/dev/scd%d","/dev/sr%d","/dev/cd%d","dev/cdrom%d","/dev/cdroms/cdrom%d",
                     "/dev/cd%dc","/dev/acd%dc","/dev/matcd%dc","/dev/mcd%dc","/dev/scd%dc","/dev/rsr%dc",NULL};

  for(i=0;special[i]!=NULL;i++)
  {
    device=special[i];
    if(checkDevice(device))
    {
      // The device exists - make sure it is not a duplicate
      link=FXFile::symlink(device);
      if(link.empty())
        link=device;
      if(std::find(actual.begin(),actual.end(),link)==actual.end())
      {
        actual.push_back(link);
        devices.push_back(device);
      }
    }
  }

  for(i=0;letters[i]!=NULL;i++)
  {
    exists=TRUE;
    for(j=0;exists;j++)
    {
      device=FXStringFormat(letters[i],'a'+j);
      if(checkDevice(device))
      {
        // The device exists - make sure it is not a duplicate
        link=FXFile::symlink(device);
        if(link.empty())
          link=device;
        if(std::find(actual.begin(),actual.end(),link)==actual.end())
        {
          actual.push_back(link);
          devices.push_back(device);
        }
      }
      else
      {
        exists=FALSE;
      }
    }
  }

  for(i=0;numbers[i]!=NULL;i++)
  {
    exists=TRUE;
    for(j=0;exists;j++)
    {
      device=FXStringFormat(numbers[i],j);
      if(checkDevice(device))
      {
        // The device exists - make sure it is not a duplicate
        link=FXFile::symlink(device);
        if(link.empty())
          link=device;
        if(std::find(actual.begin(),actual.end(),link)==actual.end())
        {
          actual.push_back(link);
          devices.push_back(device);
        }
      }
      else
      {
        exists=FALSE;
      }
    }
  }
#endif
}
