/* CDPreferences.h
 * Copyright (C) 2001 Dustin Graves <dgraves@computer.org>
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

#ifndef _CDPREFERENCES_H_
#define _CDPREFERENCES_H_

class CDWindow;

class CDPreferences : public FXDialogBox
{
  FXDECLARE(CDPreferences)
protected:
  CDWindow* cdwindow;
  FXList* devlist;
  CDPreferences(){}
public:
  CDPreferences(CDWindow* owner);

  virtual void show(FXuint placement);

  long onCmdDeviceAdd(FXObject*,FXSelector,void*);
  long onCmdDeviceRemove(FXObject*,FXSelector,void*);
  long onUpdDeviceRemove(FXObject*,FXSelector,void*);
  long onCmdDefaultDevs(FXObject*,FXSelector,void*);
  long onCmdServerList(FXObject*,FXSelector,void*);
  long onCmdAdvanced(FXObject*,FXSelector,void*);

  enum
  {
    ID_DEVICEADD=FXDialogBox::ID_LAST,
    ID_DEVICEREM,
    ID_DEFAULTDEVS,
    ID_SERVERLIST,
    ID_ADVANCEDCDDB,
    ID_ADVANCEDCDINDEX,
    ID_LAST
  };
};

#endif
