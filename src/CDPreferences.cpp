/* CDPreferences.cpp
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

#include <algorithm>
#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <cdlyte.h>
#include <fox-1.6/fx.h>
#include <fox-1.6/FXArray.h>
#include <fox-1.6/FXElement.h>
#include "CDdefs.h"
#include "CDutils.h"
#include "CDPlayer.h"
#include "CDListBox.h"
#include "CDCanvas.h"
#include "CDWindow.h"
#include "CDServerDialog.h"
#include "CDPreferences.h"

FXDEFMAP(CDPreferences) CDPreferencesMap[]={
  FXMAPFUNC(SEL_COMMAND,CDPreferences::ID_ACCEPT,CDPreferences::onCmdAccept),
  FXMAPFUNC(SEL_COMMAND,CDPreferences::ID_APPLY,CDPreferences::onCmdApply),
  FXMAPFUNC(SEL_SELECTED,CDPreferences::ID_LIST,CDPreferences::onCmdList),
  FXMAPFUNC(SEL_COMMAND,CDPreferences::ID_DEVICEADD,CDPreferences::onCmdDeviceAdd),
  FXMAPFUNC(SEL_COMMAND,CDPreferences::ID_DEVICEREM,CDPreferences::onCmdDeviceRemove),
  FXMAPFUNC(SEL_UPDATE,CDPreferences::ID_DEVICEREM,CDPreferences::onUpdDeviceRemove),
  FXMAPFUNC(SEL_COMMAND,CDPreferences::ID_DEVICESCAN,CDPreferences::onCmdDeviceScan)
};

FXIMPLEMENT(CDPreferences,FXDialogBox,CDPreferencesMap,ARRAYNUMBER(CDPreferencesMap))

CDPreferences::CDPreferences(CDWindow* owner)
: FXDialogBox(owner,"CD Player Preferences",DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE,0,0,400,0, 0,0,0,0),
  showmenubar(owner->menubar->shown()),
  showstatusbar(owner->statusbar->shown()),
  showtooltips(owner->tooltip!=NULL),
  lcdforeclr(owner->lcdforeclr),
  lcdbackclr(owner->lcdbackclr),
  iconclr(owner->iconclr),
  startmode(owner->startmode),
  stoponexit(owner->stoponexit),
  intro(owner->cdplayer.getIntro()),
  introtime(owner->cdplayer.getIntroLength()),
  random(owner->cdplayer.getRandom()),
  repeatmode(owner->cdplayer.getRepeatMode()),
  timemode(owner->canvas->getTimeMode()),
  cdwindow(owner)
{
  showmenubartgt.connect(showmenubar);
  showstatusbartgt.connect(showstatusbar);
  showtooltipstgt.connect(showtooltips);
  lcdforeclrtgt.connect(lcdforeclr);
  lcdbackclrtgt.connect(lcdbackclr);
  iconclrtgt.connect(iconclr);
  startmodetgt.connect(startmode);
  stoponexittgt.connect(stoponexit);
  introtgt.connect(intro);
  introtimetgt.connect(introtime);
  randomtgt.connect(random);
  repeatmodetgt.connect(repeatmode);
  timemodetgt.connect(timemode);

  FXVerticalFrame* contents=new FXVerticalFrame(this,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXHorizontalFrame* buttons=new FXHorizontalFrame(contents,PACK_UNIFORM_WIDTH|LAYOUT_BOTTOM|LAYOUT_FILL_X);
  new FXButton(buttons,"&Accept",NULL,this,CDPreferences::ID_ACCEPT,BUTTON_INITIAL|BUTTON_DEFAULT|LAYOUT_RIGHT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20);
  new FXButton(buttons,"A&pply",NULL,this,CDPreferences::ID_APPLY,LAYOUT_RIGHT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20);
  new FXButton(buttons,"&Cancel",NULL,this,FXDialogBox::ID_CANCEL,LAYOUT_RIGHT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20);
  new FXHorizontalSeparator(contents,SEPARATOR_RIDGE|LAYOUT_BOTTOM|LAYOUT_FILL_X);

  FXHorizontalFrame* primary=new FXHorizontalFrame(contents,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXVerticalFrame* listframe=new FXVerticalFrame(primary,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FIX_WIDTH|LAYOUT_FILL_Y,0,0,120,0, 0,0,0,0, 0,0);
  new FXLabel(listframe,"Category",NULL,JUSTIFY_LEFT|FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_X);

  tree=new FXTreeList(listframe,this,ID_LIST,TREELIST_BROWSESELECT|TREELIST_SHOWS_LINES|TREELIST_SHOWS_BOXES|TREELIST_ROOT_BOXES|LAYOUT_FILL_X|LAYOUT_FILL_Y);

  treeitem.push_back(tree->appendItem(0,"Appearance",NULL,NULL));
  treeitem.push_back(tree->appendItem(treeitem[CDPREFS_APPEARANCE],"Colors",NULL,NULL));
  treeitem.push_back(tree->appendItem(0,"Options",NULL,NULL));
  treeitem.push_back(tree->appendItem(treeitem[CDPREFS_OPTIONS],"Display",NULL,NULL));
  treeitem.push_back(tree->appendItem(treeitem[CDPREFS_OPTIONS],"Player",NULL,NULL));
  treeitem.push_back(tree->appendItem(0,"Hardware",NULL,NULL));

  tree->expandTree(treeitem[CDPREFS_APPEARANCE]);
  tree->expandTree(treeitem[CDPREFS_OPTIONS]);

  switcher=new FXSwitcher(primary,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);

  // Appearance panel1
  FXVerticalFrame* settingsframe=new FXVerticalFrame(switcher,LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXLabel(settingsframe,"Appearance");
  new FXHorizontalSeparator(settingsframe,SEPARATOR_LINE|LAYOUT_FILL_X);

  FXGroupBox* settingsbox=new FXGroupBox(settingsframe,"Player Appearance",GROUPBOX_TITLE_LEFT|FRAME_GROOVE|LAYOUT_FILL_X);
  FXVerticalFrame* appbtnsframe=new FXVerticalFrame(settingsbox,LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXCheckButton(appbtnsframe,"Show Menu Bar",&showmenubartgt,FXDataTarget::ID_VALUE,ICON_BEFORE_TEXT|JUSTIFY_LEFT);
  new FXCheckButton(appbtnsframe,"Show Status Bar",&showstatusbartgt,FXDataTarget::ID_VALUE,ICON_BEFORE_TEXT|JUSTIFY_LEFT);
  new FXCheckButton(appbtnsframe,"Show Tool Tips",&showtooltipstgt,FXDataTarget::ID_VALUE,ICON_BEFORE_TEXT|JUSTIFY_LEFT);

  // Colors panel
  FXVerticalFrame* colorsframe=new FXVerticalFrame(switcher,LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXLabel(colorsframe,"Colors");
  new FXHorizontalSeparator(colorsframe,SEPARATOR_LINE|LAYOUT_FILL_X);

  FXGroupBox* colorsbox=new FXGroupBox(colorsframe,"Display Colors",GROUPBOX_TITLE_LEFT|FRAME_GROOVE|LAYOUT_FILL_X);
  FXMatrix *colorsmatrix=new FXMatrix(colorsbox,3,LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXLabel(colorsmatrix,"Display Foreground Color:",NULL,JUSTIFY_LEFT|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FILL_ROW);
  new FXLabel(colorsmatrix,"Display Background Color:",NULL,JUSTIFY_LEFT|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FILL_ROW);
  new FXLabel(colorsmatrix,"Button Icon Color:",NULL,JUSTIFY_LEFT|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FILL_ROW);
  new FXColorWell(colorsmatrix,FXRGB(0,0,0),&lcdforeclrtgt,FXDataTarget::ID_VALUE,COLORWELL_OPAQUEONLY|FRAME_SUNKEN|FRAME_THICK|LAYOUT_RIGHT|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW,0,0,40,24);
  new FXColorWell(colorsmatrix,FXRGB(0,0,0),&lcdbackclrtgt,FXDataTarget::ID_VALUE,COLORWELL_OPAQUEONLY|FRAME_SUNKEN|FRAME_THICK|LAYOUT_RIGHT|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW,0,0,40,24);
  new FXColorWell(colorsmatrix,FXRGB(0,0,0),&iconclrtgt,FXDataTarget::ID_VALUE,COLORWELL_OPAQUEONLY|FRAME_SUNKEN|FRAME_THICK|LAYOUT_RIGHT|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW,0,0,40,24);

  // Options panel
  FXVerticalFrame* optionsframe=new FXVerticalFrame(switcher,LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXLabel(optionsframe,"Options");
  new FXHorizontalSeparator(optionsframe,SEPARATOR_LINE|LAYOUT_FILL_X);

  FXGroupBox* startbox=new FXGroupBox(optionsframe,"Startup Action",GROUPBOX_TITLE_LEFT|FRAME_GROOVE|LAYOUT_FILL_X);
  FXVerticalFrame* startframe=new FXVerticalFrame(startbox,LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXRadioButton(startframe,"No Action",&startmodetgt,FXDataTarget::ID_OPTION+CDSTART_NONE,ICON_BEFORE_TEXT|JUSTIFY_LEFT);
  new FXRadioButton(startframe,"Start Play on Startup",&startmodetgt,FXDataTarget::ID_OPTION+CDSTART_START,ICON_BEFORE_TEXT|JUSTIFY_LEFT);
  new FXRadioButton(startframe,"Stop Play on Startup",&startmodetgt,FXDataTarget::ID_OPTION+CDSTART_STOP,ICON_BEFORE_TEXT|JUSTIFY_LEFT);

  FXGroupBox* stopbox=new FXGroupBox(optionsframe,"Exit Action",GROUPBOX_TITLE_LEFT|FRAME_GROOVE|LAYOUT_FILL_X);
  FXVerticalFrame* stopframe=new FXVerticalFrame(stopbox,LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXCheckButton(stopframe,"Stop Play on Exit",&stoponexittgt,FXDataTarget::ID_VALUE,ICON_BEFORE_TEXT|JUSTIFY_LEFT);

  // Display panel
  FXVerticalFrame* displayframe=new FXVerticalFrame(switcher,LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXLabel(displayframe,"Display");
  new FXHorizontalSeparator(displayframe,SEPARATOR_LINE|LAYOUT_FILL_X);

  FXGroupBox* timemodebox=new FXGroupBox(displayframe,"Display Time Modes",GROUPBOX_TITLE_LEFT|FRAME_GROOVE|LAYOUT_FILL_X);
  FXVerticalFrame* timemodeframe=new FXVerticalFrame(timemodebox,LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXRadioButton(timemodeframe,"Album",&timemodetgt,FXDataTarget::ID_OPTION+CDTIME_DISC,ICON_BEFORE_TEXT|JUSTIFY_LEFT);
  new FXRadioButton(timemodeframe,"Track",&timemodetgt,FXDataTarget::ID_OPTION+CDTIME_TRACK,ICON_BEFORE_TEXT|JUSTIFY_LEFT);
  new FXRadioButton(timemodeframe,"Album Remaining",&timemodetgt,FXDataTarget::ID_OPTION+CDTIME_DISCREM,ICON_BEFORE_TEXT|JUSTIFY_LEFT);
  new FXRadioButton(timemodeframe,"Track Remaining",&timemodetgt,FXDataTarget::ID_OPTION+CDTIME_TRACKREM,ICON_BEFORE_TEXT|JUSTIFY_LEFT);

  // Player panel
  FXVerticalFrame* playerframe=new FXVerticalFrame(switcher,LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXLabel(playerframe,"Player");
  new FXHorizontalSeparator(playerframe,SEPARATOR_LINE|LAYOUT_FILL_X);

  FXGroupBox* playmodebox=new FXGroupBox(playerframe,"Play Modes",GROUPBOX_TITLE_LEFT|FRAME_GROOVE|LAYOUT_FILL_X);
  FXVerticalFrame* playmodeframe=new FXVerticalFrame(playmodebox,LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXCheckButton(playmodeframe,"Shuffle Play",&randomtgt,FXDataTarget::ID_VALUE,ICON_BEFORE_TEXT|JUSTIFY_LEFT);
  new FXCheckButton(playmodeframe,"Intro Play",&introtgt,FXDataTarget::ID_VALUE,ICON_BEFORE_TEXT|JUSTIFY_LEFT);
  FXHorizontalFrame* introtimeframe=new FXHorizontalFrame(playmodeframe,LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXLabel(introtimeframe,"Intro Length:",NULL,JUSTIFY_LEFT|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FILL_ROW);
  new FXSpinner(introtimeframe,4,&introtimetgt,FXDataTarget::ID_VALUE,FRAME_THICK|FRAME_SUNKEN|LAYOUT_CENTER_Y|LAYOUT_RIGHT);

  FXGroupBox* repeatbox=new FXGroupBox(playerframe,"Repeat Mode",GROUPBOX_TITLE_LEFT|FRAME_GROOVE|LAYOUT_FILL_X);
  FXVerticalFrame* repeatframe=new FXVerticalFrame(repeatbox,LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXRadioButton(repeatframe,"None",&repeatmodetgt,FXDataTarget::ID_OPTION+CDREPEAT_NONE,ICON_BEFORE_TEXT|JUSTIFY_LEFT);
  new FXRadioButton(repeatframe,"Track",&repeatmodetgt,FXDataTarget::ID_OPTION+CDREPEAT_TRACK,ICON_BEFORE_TEXT|JUSTIFY_LEFT);
  new FXRadioButton(repeatframe,"Album",&repeatmodetgt,FXDataTarget::ID_OPTION+CDREPEAT_DISC,ICON_BEFORE_TEXT|JUSTIFY_LEFT);

  // Hardware panel
  FXVerticalFrame* hardframe=new FXVerticalFrame(switcher,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
  new FXLabel(hardframe,"Hardware");
  new FXHorizontalSeparator(hardframe,SEPARATOR_LINE|LAYOUT_FILL_X);

  FXGroupBox* hardbox=new FXGroupBox(hardframe,"CD Audio Device",GROUPBOX_TITLE_LEFT|FRAME_GROOVE|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXHorizontalFrame* devframe=new FXHorizontalFrame(hardbox,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXVerticalFrame* devlistframe=new FXVerticalFrame(devframe,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0, 0,0);
  devlist=new FXList(devlistframe,NULL,0,LIST_BROWSESELECT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXVerticalFrame* devbuttons=new FXVerticalFrame(devframe,PACK_UNIFORM_WIDTH,0,0,0,0, 0,0,0,0);
  FXVerticalFrame* devaddframe=new FXVerticalFrame(devbuttons,FRAME_SUNKEN|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXButton(devaddframe,"Add",NULL,this,ID_DEVICEADD,FRAME_RAISED|LAYOUT_FILL_X,0,0,0,0, 20,20);
  FXVerticalFrame* devremframe=new FXVerticalFrame(devbuttons,FRAME_SUNKEN|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXButton(devremframe,"Remove",NULL,this,ID_DEVICEREM,FRAME_RAISED|LAYOUT_FILL_X,0,0,0,0, 20,20);
  FXVerticalFrame* devscanframe=new FXVerticalFrame(devbuttons,FRAME_SUNKEN|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXButton(devscanframe,"Scan",NULL,this,ID_DEVICESCAN,FRAME_RAISED|LAYOUT_FILL_X,0,0,0,0, 20,20);

  // Add devices to list
  FXint i,n=owner->bandtitle->getNumItems();
  FXString *data;

  // Look for device
  for(i=0;i<n;i++)
  {
    data=(FXString*)owner->bandtitle->getItemData(i);
    devlist->appendItem(*data);
  }
}

void CDPreferences::setPanel(FXuint panel)
{
  tree->setCurrentItem(treeitem[panel],TRUE);
}

FXuint CDPreferences::getPanel() const
{
  FXTreeItem* ti=tree->getCurrentItem();
  std::vector<FXTreeItem*>::const_iterator iter;

  for(iter=treeitem.begin();iter!=treeitem.end();++iter)
  {
    if(*iter==ti)
      return iter-treeitem.begin();
  }

  return 0;  // Default to first item
}

long CDPreferences::onCmdAccept(FXObject*,FXSelector,void*)
{
  handle(this,MKUINT(ID_APPLY,SEL_COMMAND),NULL);
  getApp()->stopModal(this,TRUE);
  hide();
  return 1;
}

long CDPreferences::onCmdApply(FXObject*,FXSelector,void*)
{
  FXString adderr=FXString::null;
  FXString remerr=FXString::null;
  std::list<FXString>::iterator iter;

  if(!showmenubar&&cdwindow->menubar->shown())
  {
    cdwindow->menubar->hide();
    cdwindow->resize(cdwindow->getWidth(),cdwindow->getDefaultHeight());
  }
  else if(showmenubar&&!cdwindow->menubar->shown())
  {
    cdwindow->menubar->show();
    cdwindow->resize(cdwindow->getWidth(),cdwindow->getDefaultHeight());
  }

  if(!showstatusbar&&cdwindow->statusbar->shown())
  {
    cdwindow->statusbar->hide();
    cdwindow->resize(cdwindow->getWidth(),cdwindow->getDefaultHeight());
  }
  else if(showstatusbar&&!cdwindow->statusbar->shown())
  {
    cdwindow->statusbar->show();
    cdwindow->resize(cdwindow->getWidth(),cdwindow->getDefaultHeight());
  }

  if(showtooltips&&cdwindow->tooltip==NULL)
  {
    cdwindow->tooltip=new FXToolTip(getApp(),TOOLTIP_NORMAL);
    cdwindow->tooltip->create();
  }
  else if(!showtooltips&&cdwindow->tooltip!=NULL)
  {
    delete cdwindow->tooltip;
    cdwindow->tooltip=NULL;
  }

  cdwindow->setDisplayForeground(lcdforeclr);
  cdwindow->setDisplayBackground(lcdbackclr);
  cdwindow->setIconColor(iconclr);

  cdwindow->startmode=startmode;
  cdwindow->stoponexit=stoponexit;
  cdwindow->cdplayer.setIntro(intro);
  cdwindow->cdplayer.setIntroLength(introtime);
  cdwindow->cdplayer.setRandom(random);
  cdwindow->cdplayer.setRepeatMode(repeatmode);
  cdwindow->canvas->setTimeMode(timemode);

  for(iter=adddev.begin();iter!=adddev.end();++iter)
  {
    if(!cdwindow->addDevice(*iter))
    {
      adderr+=*iter;
      adderr+="\n";
    }
  }

  if(!adderr.empty())
  {
    FXMessageBox::error(this,MBOX_OK,"Add Device Error","Error adding the following CD Audio Devices:\n\n%s",adderr.text());
  }

  for(iter=remdev.begin();iter!=remdev.end();++iter)
  {
    if(!cdwindow->removeDevice(*iter))
    {
      remerr+=*iter;
      remerr+="\n";
    }
  }

  if(!remerr.empty())
  {
    FXMessageBox::error(this,MBOX_OK,"Remove Device Error","Error adding the following CD Audio Devices:\n\n%s",remerr.text());
  }

  return 1;
}

long CDPreferences::onCmdList(FXObject*,FXSelector,void* ptr)
{
  std::vector<FXTreeItem*>::const_iterator iter;

  for(iter=treeitem.begin();iter!=treeitem.end();++iter)
  {
    if(*iter==(FXTreeItem*)ptr)
    {
      switcher->handle(this,MKUINT(FXSwitcher::ID_OPEN_FIRST+(iter-treeitem.begin()),SEL_COMMAND),NULL);
      break;
    }
  }

  return 1;
}

long CDPreferences::onCmdDeviceAdd(FXObject*,FXSelector,void*)
{
  FXInputDialog dialog(this,"Add CD-ROM Device","Device name:");
  if(dialog.execute())
  {
    FXString devnam=dialog.getText();

    if(!checkDevice(devnam))
    {
      FXMessageBox::error(this,MBOX_OK,"Invalid CD Audio Device","%s is not a valid CD audio device.  ",devnam.text());
    }
    else
    {
      std::list<FXString>::iterator iter=std::find(remdev.begin(),remdev.end(),devnam);
      if(iter!=remdev.end())
        remdev.erase(iter);
      else
        adddev.push_back(devnam);
      devlist->appendItem(devnam);
    }
  }
  return 1;
}

long CDPreferences::onCmdDeviceRemove(FXObject*,FXSelector,void*)
{
  FXint item=devlist->getCurrentItem();
  FXString devnam=devlist->getItemText(item);
  std::list<FXString>::iterator iter=std::find(adddev.begin(),adddev.end(),devnam);
  if(iter!=adddev.end())
    adddev.erase(iter);
  else
    remdev.push_back(devnam);
  devlist->removeItem(item);
  return 1;
}

long CDPreferences::onUpdDeviceRemove(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(devlist->getNumItems()>1)?ID_ENABLE:ID_DISABLE;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDPreferences::onCmdDeviceScan(FXObject*,FXSelector,void*)
{
  FXint i,n;
  std::vector<FXString> devices;
  std::vector<FXString>::iterator iter;

  scanDevices(devices);

  for(iter=devices.begin();iter!=devices.end();++iter)
  {
    n=devlist->getNumItems();
    for(i=0;i<n;i++)
    {
      if(*iter==devlist->getItemText(i))
        break;
    }

    if(i==n)
    {
      devlist->appendItem(*iter);
      if(std::find(adddev.begin(),adddev.end(),*iter)==adddev.end())
        adddev.push_back(*iter);
    }
  }

  return 1;
}

/*

CDPreferences::CDPreferences(CDWindow* owner)
: FXDialogBox(owner,"CD Player Preferences",DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE,0,0,0,0, 10,10,10,10, 4,4),
  cdwindow(owner)
{
  FXHorizontalFrame* buttons=new FXHorizontalFrame(this,PACK_UNIFORM_WIDTH|LAYOUT_SIDE_BOTTOM|LAYOUT_RIGHT);
  new FXButton(buttons,"&OK",NULL,this,FXDialogBox::ID_ACCEPT,FRAME_RAISED|FRAME_THICK|LAYOUT_RIGHT,0,0,0,0, 20,20);
  new FXButton(buttons,"&Cancel",NULL,this,FXDialogBox::ID_CANCEL,FRAME_RAISED|FRAME_THICK|LAYOUT_RIGHT,0,0,0,0, 20,20);

  new FXHorizontalSeparator(this,SEPARATOR_RIDGE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

  FXHorizontalFrame* tabframe=new FXHorizontalFrame(this,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);

  FXTabBook* tabbook=new FXTabBook(tabframe,NULL,0,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXTabItem* settab=new FXTabItem(tabbook,"&Options",NULL,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXVerticalFrame* setframe=new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_FILL_Y);

  FXHorizontalFrame* setdefault=new FXHorizontalFrame(setframe,LAYOUT_FILL_X|LAYOUT_BOTTOM);
  new FXButton(setdefault,"Defaults",NULL,cdwindow,CDWindow::ID_DEFAULTOPTIONS,FRAME_THICK|FRAME_RAISED,0,0,0,0, 20,20);

  FXHorizontalFrame* settop=new FXHorizontalFrame(setframe,PACK_UNIFORM_WIDTH|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXHorizontalFrame* intime=new FXHorizontalFrame(settop,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
  new FXCheckButton(intime,"Intro Play",cdwindow,CDWindow::ID_INTRO,CHECKBUTTON_NORMAL|LAYOUT_CENTER_Y);
  new FXSpinner(intime,4,cdwindow,CDWindow::ID_INTROTIME,FRAME_THICK|FRAME_SUNKEN|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
  new FXCheckButton(settop,"Shuffle Play",cdwindow,CDWindow::ID_RANDOM,ICON_BEFORE_TEXT|JUSTIFY_LEFT|LAYOUT_FILL_X|LAYOUT_CENTER_Y);

  FXHorizontalFrame* setmid=new FXHorizontalFrame(setframe,PACK_UNIFORM_WIDTH|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
  FXGroupBox* repeatmode=new FXGroupBox(setmid,"Repeat Mode",GROUPBOX_TITLE_LEFT|FRAME_RIDGE|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXVerticalFrame* repeatframe=new FXVerticalFrame(repeatmode,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
  new FXRadioButton(repeatframe,"None",cdwindow->repeatTarget,FXDataTarget::ID_OPTION+CDREPEAT_NONE,RADIOBUTTON_NORMAL|LAYOUT_CENTER_Y);
  new FXRadioButton(repeatframe,"Track",cdwindow->repeatTarget,FXDataTarget::ID_OPTION+CDREPEAT_TRACK,RADIOBUTTON_NORMAL|LAYOUT_CENTER_Y);
  new FXRadioButton(repeatframe,"Album",cdwindow->repeatTarget,FXDataTarget::ID_OPTION+CDREPEAT_DISC,RADIOBUTTON_NORMAL|LAYOUT_CENTER_Y);

  FXGroupBox* timemode=new FXGroupBox(setmid,"Time Mode",GROUPBOX_TITLE_LEFT|FRAME_RIDGE|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXVerticalFrame* timeframe=new FXVerticalFrame(timemode,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
  new FXRadioButton(timeframe,"Album",cdwindow->timeTarget,FXDataTarget::ID_OPTION+CDTIME_DISC,RADIOBUTTON_NORMAL|LAYOUT_CENTER_Y);
  new FXRadioButton(timeframe,"Track",cdwindow->timeTarget,FXDataTarget::ID_OPTION+CDTIME_TRACK,RADIOBUTTON_NORMAL|LAYOUT_CENTER_Y);
  new FXRadioButton(timeframe,"Album Remaining",cdwindow->timeTarget,FXDataTarget::ID_OPTION+CDTIME_DISCREM,RADIOBUTTON_NORMAL|LAYOUT_CENTER_Y);
  new FXRadioButton(timeframe,"Track Remaining",cdwindow->timeTarget,FXDataTarget::ID_OPTION+CDTIME_TRACKREM,RADIOBUTTON_NORMAL|LAYOUT_CENTER_Y);

  FXHorizontalFrame* setbot=new FXHorizontalFrame(setframe,PACK_UNIFORM_WIDTH|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
  FXVerticalFrame* stopstart=new FXVerticalFrame(setbot,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
  FXGroupBox* startmode=new FXGroupBox(stopstart,"Startup Action",GROUPBOX_TITLE_LEFT|FRAME_RIDGE|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXVerticalFrame* startframe=new FXVerticalFrame(startmode,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
  new FXRadioButton(startframe,"No Action",cdwindow->startTarget,FXDataTarget::ID_OPTION+CDSTART_NONE,CHECKBUTTON_NORMAL|LAYOUT_CENTER_Y);
  new FXRadioButton(startframe,"Start Play on Startup",cdwindow->startTarget,FXDataTarget::ID_OPTION+CDSTART_START,CHECKBUTTON_NORMAL|LAYOUT_CENTER_Y);
  new FXRadioButton(startframe,"Stop Play on Startup",cdwindow->startTarget,FXDataTarget::ID_OPTION+CDSTART_STOP,CHECKBUTTON_NORMAL|LAYOUT_CENTER_Y);
  FXGroupBox* stopmode=new FXGroupBox(stopstart,"Exit Action",GROUPBOX_TITLE_LEFT|FRAME_RIDGE|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXVerticalFrame* stopframe=new FXVerticalFrame(stopmode,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
  new FXCheckButton(stopframe,"Stop Play on Exit",cdwindow,CDWindow::ID_STOPONEXIT,CHECKBUTTON_NORMAL|LAYOUT_CENTER_Y);


  //Appearance
  FXTabItem* apptab=new FXTabItem(tabbook,"&Appearance",NULL,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXVerticalFrame* appframe=new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_FILL_Y);

  FXHorizontalFrame* appdefault=new FXHorizontalFrame(appframe,LAYOUT_FILL_X|LAYOUT_BOTTOM);
  new FXButton(appdefault,"Defaults",NULL,cdwindow,CDWindow::ID_DEFAULTAPPEARANCE,FRAME_THICK|FRAME_RAISED,0,0,0,0, 20,20);

  FXGroupBox* colors=new FXGroupBox(appframe,"Colors",GROUPBOX_TITLE_LEFT|FRAME_RIDGE|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXMatrix *cmatrix=new FXMatrix(colors,3,MATRIX_BY_ROWS|PACK_UNIFORM_HEIGHT|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
  new FXLabel(cmatrix,"Display Foreground Color:",NULL,JUSTIFY_LEFT|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FILL_ROW);
  new FXLabel(cmatrix,"Display Background Color:",NULL,JUSTIFY_LEFT|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FILL_ROW);
  new FXLabel(cmatrix,"Button Icon Color:",NULL,JUSTIFY_LEFT|LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_FILL_ROW);
  new FXColorWell(cmatrix,FXRGB(0,0,0),cdwindow,CDWindow::ID_COLORFORE,COLORWELL_OPAQUEONLY|FRAME_SUNKEN|FRAME_THICK|LAYOUT_LEFT|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW,0,0,40,24);
  new FXColorWell(cmatrix,FXRGB(0,0,0),cdwindow,CDWindow::ID_COLORBACK,COLORWELL_OPAQUEONLY|FRAME_SUNKEN|FRAME_THICK|LAYOUT_LEFT|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW,0,0,40,24);
  new FXColorWell(cmatrix,FXRGB(0,0,0),cdwindow,CDWindow::ID_COLORICONS,COLORWELL_OPAQUEONLY|FRAME_SUNKEN|FRAME_THICK|LAYOUT_LEFT|LAYOUT_CENTER_Y|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW,0,0,40,24);

  FXGroupBox* fonts=new FXGroupBox(appframe,"Fonts",GROUPBOX_TITLE_LEFT|FRAME_RIDGE|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXMatrix *fmatrix=new FXMatrix(fonts,1,MATRIX_BY_ROWS|PACK_UNIFORM_HEIGHT|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
  new FXLabel(fmatrix,"Display Font:",NULL,JUSTIFY_LEFT|LAYOUT_CENTER_Y|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
  new FXButton(fmatrix,"Set...",NULL,cdwindow,CDWindow::ID_FONT,FRAME_THICK|FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_CENTER_Y|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW,0,0,0,0, 20,20);

  FXVerticalFrame* appgen=new FXVerticalFrame(appframe,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  new FXCheckButton(appgen,"Show Menu Bar",cdwindow,CDWindow::ID_TOGGLEMENU,CHECKBUTTON_NORMAL|LAYOUT_LEFT|LAYOUT_CENTER_Y|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
  new FXCheckButton(appgen,"Show Status Bar",cdwindow,CDWindow::ID_TOGGLESTATUS,CHECKBUTTON_NORMAL|LAYOUT_LEFT|LAYOUT_CENTER_Y|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
  new FXCheckButton(appgen,"Show Tool Tips",cdwindow,CDWindow::ID_TOGGLETIPS,CHECKBUTTON_NORMAL|LAYOUT_LEFT|LAYOUT_CENTER_Y|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);


  //Hardware
  FXTabItem* hardtab=new FXTabItem(tabbook,"&Hardware",NULL,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXVerticalFrame* hardframe=new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_FILL_Y);

  FXHorizontalFrame* harddefault=new FXHorizontalFrame(hardframe,LAYOUT_FILL_X|LAYOUT_BOTTOM);
  new FXButton(harddefault,"Defaults",NULL,this,ID_DEFAULTDEVS,FRAME_THICK|FRAME_RAISED,0,0,0,0, 20,20);

  FXGroupBox* harddev=new FXGroupBox(hardframe,"CD-ROM Device",GROUPBOX_TITLE_LEFT|FRAME_RIDGE|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXHorizontalFrame* devframe=new FXHorizontalFrame(harddev,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXVerticalFrame* devlistframe=new FXVerticalFrame(devframe,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0, 0,0);
  devlist=new FXList(devlistframe,10,NULL,0,LIST_BROWSESELECT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXVerticalFrame* devbuttons=new FXVerticalFrame(devframe,PACK_UNIFORM_WIDTH|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
  new FXButton(devbuttons,"&Add",NULL,this,ID_DEVICEADD,FRAME_THICK|FRAME_RAISED,0,0,0,0, 20,20);
  new FXButton(devbuttons,"&Remove",NULL,this,ID_DEVICEREM,FRAME_THICK|FRAME_RAISED,0,0,0,0, 20,20);


  //Internet
  FXTabItem* infotab=new FXTabItem(tabbook,"&Internet",NULL,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXVerticalFrame* infoframe=new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_FILL_Y);

  FXHorizontalFrame* infodefault=new FXHorizontalFrame(infoframe,LAYOUT_FILL_X|LAYOUT_BOTTOM);
  new FXButton(infodefault,"Defaults",NULL,cdwindow,CDWindow::ID_DEFAULTINTERNET,FRAME_THICK|FRAME_RAISED,0,0,0,0, 20,20);

  FXVerticalFrame* infoserv=new FXVerticalFrame(infoframe,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  new FXCheckButton(infoserv,"Use Remote Data Source",cdwindow,CDWindow::ID_REMOTEINFO,CHECKBUTTON_NORMAL|LAYOUT_CENTER_Y);
  new FXCheckButton(infoserv,"Check Local Source First",cdwindow,CDWindow::ID_LOCALFIRST,CHECKBUTTON_NORMAL|LAYOUT_CENTER_Y);
  FXVerticalFrame* proxyframe=new FXVerticalFrame(infoserv,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  new FXCheckButton(proxyframe,"Use Proxy Server",cdwindow,CDWindow::ID_PROXYSERVER,CHECKBUTTON_NORMAL|LAYOUT_CENTER_Y);
  FXHorizontalFrame* infoproxy=new FXHorizontalFrame(proxyframe,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  new FXLabel(infoproxy,"Proxy:",NULL,LAYOUT_CENTER_Y);
  new FXTextField(infoproxy,0,cdwindow->proxyAddrTarget,FXDataTarget::ID_VALUE,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X);
  FXSpinner* proxyspinner=new FXSpinner(infoproxy,4,cdwindow->proxyPortTarget,FXDataTarget::ID_VALUE,FRAME_THICK|FRAME_SUNKEN|LAYOUT_RIGHT);
  proxyspinner->setRange(0,65535);

  FXGroupBox* infocddb=new FXGroupBox(infoframe,"CDDB Settings",GROUPBOX_TITLE_LEFT|FRAME_RIDGE|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXVerticalFrame* cddbframe=new FXVerticalFrame(infocddb,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXHorizontalFrame* cddbbuttons=new FXHorizontalFrame(cddbframe,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  new FXCheckButton(cddbbuttons,"Use CDDB",cdwindow,CDWindow::ID_CDDB,CHECKBUTTON_NORMAL|LAYOUT_CENTER_Y);
  new FXButton(cddbbuttons,"Advanced...",NULL,this,ID_ADVANCEDCDDB,FRAME_THICK|FRAME_RAISED|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
  FXHorizontalFrame* cddbproto=new FXHorizontalFrame(cddbframe,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  new FXLabel(cddbproto,"Method:",NULL,LAYOUT_CENTER_Y);
  new FXRadioButton(cddbproto,"HTTP",cdwindow->cddbProtoTarget,FXDataTarget::ID_OPTION+PROTO_HTTP,RADIOBUTTON_NORMAL|LAYOUT_CENTER_Y);
  new FXRadioButton(cddbproto,"CDDBP",cdwindow->cddbProtoTarget,FXDataTarget::ID_OPTION+PROTO_CDDBP,RADIOBUTTON_NORMAL|LAYOUT_CENTER_Y);
  new FXButton(cddbproto,"Get Server List",NULL,this,ID_SERVERLIST,FRAME_THICK|FRAME_RAISED|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
  FXHorizontalFrame* cddbserv=new FXHorizontalFrame(cddbframe,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  new FXLabel(cddbserv,"Server:",NULL,LAYOUT_CENTER_Y);
  new FXTextField(cddbserv,0,cdwindow->cddbAddrTarget,FXDataTarget::ID_VALUE,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X);
  FXSpinner* cddbspinner=new FXSpinner(cddbserv,4,cdwindow,CDWindow::ID_CDDBPORT,FRAME_THICK|FRAME_SUNKEN|LAYOUT_RIGHT);
  cddbspinner->setRange(0,65535);
}

void CDPreferences::show(FXuint placement)
{
  //Load bandTitle contents to list
  FXint i,n=cdwindow->bandTitle->getNumItems();
  FXString* devnam;
  devlist->clearItems();
  for(i=0;i<n;i++)
  {
    devnam=(FXString*)cdwindow->bandTitle->getItemData(i);
    devlist->appendItem(*devnam);
  }
  FXDialogBox::show(placement);
}

long CDPreferences::onCmdDeviceAdd(FXObject*,FXSelector,void*)
{
  FXInputDialog dialog(this,"Add CD-ROM Device","Device name:");
  if(dialog.execute())
  {
    FXString devnam=dialog.getText();
    devlist->appendItem(devnam);
    cdwindow->handle(this,MKUINT(CDWindow::ID_CDROMADD,SEL_COMMAND),(void*)&devnam);
  }
  return 1;
}

long CDPreferences::onCmdDeviceRemove(FXObject*,FXSelector,void*)
{
  FXint item=devlist->getCurrentItem();
  devlist->removeItem(item);
  cdwindow->handle(this,MKUINT(CDWindow::ID_CDROMREM,SEL_COMMAND),(void*)item);
  return 1;
}

long CDPreferences::onUpdDeviceRemove(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(devlist->getNumItems()<2)?ID_DISABLE:ID_ENABLE;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDPreferences::onCmdDefaultDevs(FXObject*,FXSelector,void*)
{
  cdwindow->handle(this,MKUINT(CDWindow::ID_DEFAULTHARDWARE,SEL_COMMAND),NULL);

  //Load bandTitle contents to list
  FXint i,n=cdwindow->bandTitle->getNumItems();
  FXString* devnam;
  devlist->clearItems();
  for(i=0;i<n;i++)
  {
    devnam=(FXString*)cdwindow->bandTitle->getItemData(i);
    devlist->appendItem(*devnam);
  }

  return 1;
}

long CDPreferences::onCmdServerList(FXObject*,FXSelector,void*)
{
  struct cddb_serverlist list;
  if(cdwindow->cdinfo.getCDDBServerList(&list))
  {
    CDServerDialog dialog(this,&list);
    if(dialog.execute())
    {
      FXint index=dialog.getSelection();
      struct cddb_host* host=&list.list_host[index];

      cdwindow->cdinfo.setCDDBProtocol((host->host_protocol==CDDB_MODE_HTTP)?PROTO_HTTP:PROTO_CDDBP);
      cdwindow->cdinfo.setCDDBAddress(host->host_server.server_name);
      cdwindow->cdinfo.setCDDBPort(host->host_server.server_port);
      if(host->host_protocol==CDDB_MODE_HTTP)
      {
	//For current broken get we need to trim down string
	//cdwindow->setCDDBScript(host->host_addressing);
	FXString script(host->host_addressing);
	cdwindow->cdinfo.setCDDBScript(script.before(' '));
      }
    }
  }
  else
  {
    FXMessageBox::error(this,MBOX_OK,"Get Server List Error","Could not connect to %s:%d",cdwindow->cdinfo.getCDDBAddress().text(),cdwindow->cdinfo.getCDDBPort());
  }

  return 1;
}

long CDPreferences::onCmdAdvanced(FXObject*,FXSelector sel,void*)
{
  FXInputDialog dialog(this,"Advanced Server Settings","CDDB Script to Execute");
  dialog.setText(cdwindow->cdinfo.getCDDBScript());
  if(dialog.execute())
  {
    cdwindow->cdinfo.setCDDBScript(dialog.getText());
  }
  return 1;
}

*/
