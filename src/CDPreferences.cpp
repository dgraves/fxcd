/* CDPreferences.cpp
 * Copyright (C) 2001,2009 Dustin Graves <dgraves@computer.org>
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
#include "CDData.h"
#include "CDPlayer.h"
#include "CDListBox.h"
#include "CDCanvas.h"
#include "CDInfo.h"
#include "CDDBInfo.h"
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
  FXMAPFUNC(SEL_COMMAND,CDPreferences::ID_DEVICESCAN,CDPreferences::onCmdDeviceScan),
  FXMAPFUNC(SEL_COMMAND,CDPreferences::ID_ADVANCEDCDDB,CDPreferences::onCmdAdvancedCDDB),
  FXMAPFUNC(SEL_COMMAND,CDPreferences::ID_CDDBPORT,CDPreferences::onCmdCDDBPort),
  FXMAPFUNC(SEL_UPDATE,CDPreferences::ID_CDDBPORT,CDPreferences::onUpdCDDBPort),
  FXMAPFUNC(SEL_COMMAND,CDPreferences::ID_CDDBSERVERLIST,CDPreferences::onCmdCDDBServerList),
  FXMAPFUNC(SEL_COMMAND,CDPreferences::ID_DEFAULTINTERNET,CDPreferences::onCmdDefaultInternet)
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
  usecddb(owner->usecddb),
  cddbsettings(owner->cddbsettings),
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
  usecddbtgt.connect(usecddb);
  proxytgt.connect(cddbsettings.proxy);
  proxyporttgt.connect(cddbsettings.proxyport);
  proxyaddrtgt.connect(cddbsettings.proxyaddr);
  cddbprototgt.connect(cddbsettings.cddbproto);
  cddbpporttgt.connect(cddbsettings.cddbpport);
  cddbporttgt.connect(cddbsettings.cddbport);
  cddbaddrtgt.connect(cddbsettings.cddbaddr);
  cbbdpromptmultipletgt.connect(cddbsettings.promptmultiple);
  cbbdlocalcopytgt.connect(cddbsettings.localcopy);

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
  treeitem.push_back(tree->appendItem(0,"Internet",NULL, NULL));

  tree->expandTree(treeitem[CDPREFS_APPEARANCE]);
  tree->expandTree(treeitem[CDPREFS_OPTIONS]);

  switcher=new FXSwitcher(primary,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);

  // Appearance panel
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
#ifndef WIN32
  // Disable UI components for operations not supported on Windows
  new FXRadioButton(startframe,"Stop Play on Startup",&startmodetgt,FXDataTarget::ID_OPTION+CDSTART_STOP,ICON_BEFORE_TEXT|JUSTIFY_LEFT);

  FXGroupBox* stopbox=new FXGroupBox(optionsframe,"Exit Action",GROUPBOX_TITLE_LEFT|FRAME_GROOVE|LAYOUT_FILL_X);
  FXVerticalFrame* stopframe=new FXVerticalFrame(stopbox,LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXCheckButton(stopframe,"Stop Play on Exit",&stoponexittgt,FXDataTarget::ID_VALUE,ICON_BEFORE_TEXT|JUSTIFY_LEFT);
#endif

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

  // Internet panel
  FXVerticalFrame* internetframe=new FXVerticalFrame(switcher,LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXLabel(internetframe,"Internet");
  new FXHorizontalSeparator(internetframe,SEPARATOR_LINE|LAYOUT_FILL_X);

  FXGroupBox* infobox=new FXGroupBox(internetframe,"CD Info",GROUPBOX_TITLE_LEFT|FRAME_GROOVE|LAYOUT_FILL_X);
  FXVerticalFrame* infoframe=new FXVerticalFrame(infobox,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
  FXHorizontalFrame* cddbbuttons=new FXHorizontalFrame(infoframe,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  new FXCheckButton(cddbbuttons,"Obtain info from CDDB",&usecddbtgt,FXDataTarget::ID_VALUE,CHECKBUTTON_NORMAL|LAYOUT_CENTER_Y);
  new FXButton(cddbbuttons,"Advanced...",NULL,this,ID_ADVANCEDCDDB,FRAME_THICK|FRAME_RAISED|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
  new FXCheckButton(infoframe,"Prompt when multiple matches exist",&cbbdpromptmultipletgt,FXDataTarget::ID_VALUE,CHECKBUTTON_NORMAL|LAYOUT_CENTER_Y);
  new FXCheckButton(infoframe,"Keep local copy",&cbbdlocalcopytgt,FXDataTarget::ID_VALUE,CHECKBUTTON_NORMAL|LAYOUT_CENTER_Y);


  FXHorizontalFrame* cddbproto=new FXHorizontalFrame(infoframe,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  new FXLabel(cddbproto,"Method:",NULL,LAYOUT_CENTER_Y);
  new FXRadioButton(cddbproto,"HTTP",&cddbprototgt,FXDataTarget::ID_OPTION+CDDB_PROTOCOL_HTTP,RADIOBUTTON_NORMAL|LAYOUT_CENTER_Y|LAYOUT_RIGHT);
  new FXRadioButton(cddbproto,"CDDBP",&cddbprototgt,FXDataTarget::ID_OPTION+CDDB_PROTOCOL_CDDBP,RADIOBUTTON_NORMAL|LAYOUT_CENTER_Y|LAYOUT_RIGHT);

  FXHorizontalFrame* cddbserv=new FXHorizontalFrame(infoframe,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  new FXLabel(cddbserv,"Server:",NULL,LAYOUT_CENTER_Y);
  new FXTextField(cddbserv,0,&cddbaddrtgt,FXDataTarget::ID_VALUE,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X);
  FXSpinner* cddbspinner=new FXSpinner(cddbserv,4,this,ID_CDDBPORT,FRAME_THICK|FRAME_SUNKEN|LAYOUT_RIGHT);
  cddbspinner->setRange(0,65535);

  new FXButton(infoframe,"Get Server List",NULL,this,ID_CDDBSERVERLIST,FRAME_THICK|FRAME_RAISED|LAYOUT_CENTER_Y|LAYOUT_RIGHT);


  FXGroupBox* proxybox=new FXGroupBox(internetframe,"Proxy Settings",GROUPBOX_TITLE_LEFT|FRAME_GROOVE|LAYOUT_FILL_X);
  FXVerticalFrame* proxyframe=new FXVerticalFrame(proxybox,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  new FXCheckButton(proxyframe,"Use Proxy Server",&proxytgt,FXDataTarget::ID_VALUE,CHECKBUTTON_NORMAL|LAYOUT_CENTER_Y);
  FXHorizontalFrame* infoproxy=new FXHorizontalFrame(proxyframe,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  new FXLabel(infoproxy,"Proxy:",NULL,LAYOUT_CENTER_Y);
  new FXTextField(infoproxy,0,&proxyaddrtgt,FXDataTarget::ID_VALUE,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X);
  FXSpinner* proxyspinner=new FXSpinner(infoproxy,4,&proxyporttgt,FXDataTarget::ID_VALUE,FRAME_THICK|FRAME_SUNKEN|LAYOUT_RIGHT);
  proxyspinner->setRange(0,65535);

  new FXButton(internetframe,"Defaults",NULL,this,ID_DEFAULTINTERNET,FRAME_THICK|FRAME_RAISED|LAYOUT_RIGHT,0,0,0,0, 20,20);
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

  cdwindow->usecddb=usecddb;
  cdwindow->cddbsettings=cddbsettings;
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

long CDPreferences::onCmdAdvancedCDDB(FXObject*,FXSelector sel,void*)
{
  FXInputDialog dialog(this,"Advanced CDDB Server Settings","CDDB Script to Execute");
  dialog.setText(cddbsettings.cddbexec);
  if(dialog.execute())
  {
    cddbsettings.cddbexec=dialog.getText();
  }
  return 1;
}

long CDPreferences::onCmdCDDBPort(FXObject* sender,FXSelector,void*)
{
  FXint port=0;
  sender->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_GETINTVALUE),(void*)&port);
  (cddbsettings.cddbproto==CDDB_PROTOCOL_HTTP)?cddbsettings.cddbport:cddbsettings.cddbpport=(FXushort)port;
  return 1;
}

long CDPreferences::onUpdCDDBPort(FXObject* sender,FXSelector,void*)
{
  FXint port=(cddbsettings.cddbproto==CDDB_PROTOCOL_HTTP)?cddbsettings.cddbport:cddbsettings.cddbpport;
  sender->handle(this,FXSEL(SEL_COMMAND,FXWindow::ID_SETINTVALUE),(void*)&port);
  return 1;
}

long CDPreferences::onCmdCDDBServerList(FXObject*,FXSelector,void*)
{
  CDDBInfo info(cddbsettings);
  struct cddb_serverlist list;

  cddb_init_cddb_serverlist(&list);
  if(info.getCDDBServerList(&list))
  {
    CDServerDialog dialog(this,&list);
    if(dialog.execute(PLACEMENT_OWNER))
    {
      FXint index=dialog.getSelection();
      struct cddb_host* host=&list.list_host[index];

      cddbsettings.cddbproto=(host->host_protocol==CDDB_MODE_HTTP)?CDDB_PROTOCOL_HTTP:CDDB_PROTOCOL_CDDBP;
      cddbsettings.cddbaddr=host->host_server.server_name;
      (host->host_protocol==CDDB_MODE_HTTP)?cddbsettings.cddbport:cddbsettings.cddbpport=host->host_server.server_port;
      if(host->host_protocol==CDDB_MODE_HTTP)
      {
	//For current broken get we need to trim down string
	//cdwindow->setCDDBScript(host->host_addressing);
	FXString script(host->host_addressing);
        cddbsettings.cddbexec=script.before(' ');
      }
    }
  }
  else
  {
    FXMessageBox::error(this,MBOX_OK,"Get Server List Error","Could not connect to %s:%d",info.getCDDBAddress().text(),info.getCDDBPort());
  }

  cddb_free_cddb_serverlist(&list);

  return 1;
}

long CDPreferences::onCmdDefaultInternet(FXObject*,FXSelector,void*)
{
  CDDBInfo::CDDBSettings defaults;
  cddbsettings=defaults;
  usecddb=TRUE;
  return 1;
}
