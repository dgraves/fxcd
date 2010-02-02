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
#include <fox-1.6/fx.h>
#include <cdlyte.h>
#include "CDutils.h"

FXbool checkDevice(const FXString& device)
{
  if(device.empty())
    return FALSE;

#ifdef WIN32
  if(device.length()<2||!isalpha(device[0])||device[1]!=':')
    return FALSE;

  FXString drive=FXStringFormat("%c:\\",device[0]);
  if(GetDriveType(drive.text())!=DRIVE_CDROM)
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

#ifndef WIN32
// Functions internal to this compilation unit
namespace {
  FXbool scanSingleDevice(const FXchar* device,std::vector<FXString>& preferredName,std::vector<FXString>& actualName)
  {
    FXbool exists=FALSE;
    if(checkDevice(device))
    {
      // The device exists - make sure it is not a duplicate
      FXString link=FXFile::symlink(device);
      if(!link.empty())
        link=FXStringFormat("%s/%s",FXPath::directory(device).text(),link.text());  // Make full path from symlink target (relative to symlink)
      else
        link=device;
      if(std::find(actualName.begin(),actualName.end(),link)==actualName.end())
      {
        actualName.push_back(link);
        preferredName.push_back(device);
      }

      exists=TRUE;
    }
    return exists;
  }

  void scanDeviceRange(const FXchar* pattern,std::vector<FXString>& preferredName,std::vector<FXString>& actualName,FXint start=0)
  {
    FXint i;
    FXbool exists=TRUE;
    for(i=start;exists;i++)
    {
      FXString device=FXStringFormat(pattern,i);
      exists=scanSingleDevice(device.text(),preferredName,actualName);
    }
  }
}
#endif

void scanDevices(std::vector<FXString>& devices)
{
  FXint i;
#ifdef WIN32
  FXString device;
  for(i='A';i<='Z';i++)
  {
    device=FXStringFormat("%c:\\",i);
    if(GetDriveType(device.text())==DRIVE_CDROM)
    {
      devices.push_back(device);
    }
  }
#else
  std::vector<FXString> actualName;
  const FXchar *devicePatterns[]={"/dev/scd%d","/dev/sr%d","/dev/cd%d","/dev/cdroms/cdrom%d","/dev/cd%dc",
                                  "/dev/acd%dc","/dev/matcd%dc","/dev/mcd%dc","/dev/scd%dc","/dev/rsr%dc",NULL};

  // Check for cdrom name first - scan the 0 device independently because many systems start at 1
  // for special name and scanDeviceRange stops as soon as a non-existent device is encountered
  scanSingleDevice("/dev/cdrom",devices,actualName);
  scanSingleDevice("/dev/cdrom0",devices,actualName);
  scanDeviceRange("/dev/cdrom%d",devices,actualName,1);

  // Check cdrw name second - scan the 0 device independently because many systems start at 1
  // for special name and scanDeviceRange stops as soon as a non-existent device is encountered
  scanSingleDevice("/dev/cdrw",devices,actualName);
  scanSingleDevice("/dev/cdrw0",devices,actualName);
  scanDeviceRange("/dev/cdrw%d",devices,actualName,1);

  // Check dvd name third - scan the 0 device independently because many systems start at 1
  // for special name and scanDeviceRange stops as soon as a non-existent device is encountered
  scanSingleDevice("/dev/dvd",devices,actualName);
  scanSingleDevice("/dev/dvd0",devices,actualName);
  scanDeviceRange("/dev/dvd%d",devices,actualName,1);

  // Scan block devices
  for(i=0;devicePatterns[i]!=NULL;i++)
  {
    scanDeviceRange(devicePatterns[i],devices,actualName);
  }

  // Check for IDE device names
  scanDeviceRange("/dev/hd%c",devices,actualName,'a');
#endif
}
