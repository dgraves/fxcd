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

#include "cdlyte.h"
#include "fox/fx.h"
#include "fox/FXArray.h"
#include "fox/FXElement.h"
#include "CDPlayer.h"
#include "CDInfo.h"
#include "CDWindow.h"
#include "CDServerDialog.h"
#include "CDPreferences.h"

FXDEFMAP(CDPreferences) CDPreferencesMap[]={
  FXMAPFUNC(SEL_COMMAND,CDPreferences::ID_DEVICEADD,CDPreferences::onCmdDeviceAdd),
  FXMAPFUNC(SEL_COMMAND,CDPreferences::ID_DEVICEREM,CDPreferences::onCmdDeviceRemove),
  FXMAPFUNC(SEL_UPDATE,CDPreferences::ID_DEVICEREM,CDPreferences::onUpdDeviceRemove),
  FXMAPFUNC(SEL_COMMAND,CDPreferences::ID_DEFAULTDEVS,CDPreferences::onCmdDefaultDevs),
  FXMAPFUNC(SEL_COMMAND,CDPreferences::ID_SERVERLIST,CDPreferences::onCmdServerList),
  FXMAPFUNC(SEL_COMMAND,CDPreferences::ID_ADVANCEDCDDB,CDPreferences::onCmdAdvanced)
};

FXIMPLEMENT(CDPreferences,FXDialogBox,CDPreferencesMap,ARRAYNUMBER(CDPreferencesMap))

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
