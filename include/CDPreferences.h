/* CDPreferences.h
 * Copyright (C) 2001,2004 Dustin Graves <dgraves@computer.org>
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

#include <vector>

enum
{
  CDPREFS_APPEARANCE=0,
  CDPREFS_COLORS=1,
  CDPREFS_OPTIONS=2,
  CDPREFS_DISPLAY=3,
  CDPREFS_PLAYER=4,
  CDPREFS_HARDWARE=5
};

class CDWindow;

class CDPreferences : public FXDialogBox
{
  FXDECLARE(CDPreferences)
protected:
  FXbool  showmenubar;
  FXbool  showstatusbar;
  FXbool  showtooltips;
  FXColor lcdforeclr;
  FXColor lcdbackclr;
  FXColor iconclr;
  FXuint  startmode;
  FXbool  stoponexit;
  FXbool  intro;
  FXuint  introtime;
  FXbool  random;
  FXuint  repeatmode;
  FXuint  timemode;
protected:
  FXDataTarget showmenubartgt;
  FXDataTarget showstatusbartgt;
  FXDataTarget showtooltipstgt;
  FXDataTarget lcdforeclrtgt;
  FXDataTarget lcdbackclrtgt;
  FXDataTarget iconclrtgt;
  FXDataTarget startmodetgt;
  FXDataTarget stoponexittgt;
  FXDataTarget introtgt;
  FXDataTarget introtimetgt;
  FXDataTarget randomtgt;
  FXDataTarget repeatmodetgt;
  FXDataTarget timemodetgt;
protected:
  CDWindow*                cdwindow;
  FXTreeList*              tree;
  std::vector<FXTreeItem*> treeitem;
  FXSwitcher*              switcher;
  FXList*                  devlist;
protected:
  CDPreferences() { }
public:
  long onCmdAccept(FXObject*,FXSelector,void*);
  long onCmdList(FXObject*,FXSelector,void*);
  long onCmdDeviceAdd(FXObject*,FXSelector,void*);
  long onCmdDeviceRemove(FXObject*,FXSelector,void*);
  long onUpdDeviceRemove(FXObject*,FXSelector,void*);
  long onCmdDefaultDevs(FXObject*,FXSelector,void*);
public:
  enum
  {
    ID_ACCEPT=FXDialogBox::ID_LAST,
    ID_LIST,
    ID_DEVICEADD,
    ID_DEVICEREM,
    ID_DEFAULTDEVS,
    ID_LAST
  };
public:
  CDPreferences(CDWindow* owner);

  void setPanel(FXuint panel);

  FXuint getPanel() const;
};

#endif
