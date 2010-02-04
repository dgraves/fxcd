/* CDWindow.cpp
 * Copyright (C) 2001,2004,2009-2010 Dustin Graves <dgraves@computer.org>
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

#include <cdlyte.h>
#include <fox-1.6/fx.h>
#include <fox-1.6/FXArray.h>
#include <fox-1.6/FXElement.h>
#include "CDdefs.h"
#include "CDutils.h"
#include "CDData.h"
#include "CDPlayer.h"
#include "CDDBSettings.h"
#include "CDInfo.h"
#include "CDDBInfo.h"
#include "CDInfoTask.h"
#include "CDListBox.h"
#include "CDSeekButton.h"
#include "CDBMPIcon.h"
#include "CDCanvas.h"
#include "CDPreferences.h"
#include "CDWindow.h"
#include "icons.h"

FXDEFMAP(CDWindow) CDWindowMap[]={
  FXMAPFUNC(SEL_CLOSE,0,CDWindow::onCmdQuit),
  FXMAPFUNC(SEL_SIGNAL,CDWindow::ID_QUIT,CDWindow::onCmdQuit),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_QUIT,CDWindow::onCmdQuit),
  FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,CDWindow::ID_CANVAS,CDWindow::onMouseUp),
  FXMAPFUNC(SEL_PAINT,CDWindow::ID_CANVAS,CDWindow::onPaint),
  FXMAPFUNC(SEL_TIMEOUT,CDWindow::ID_TIMEOUT,CDWindow::onTimeout),
  FXMAPFUNC(SEL_IO_READ,CDWindow::ID_GUISIGNAL,CDWindow::onGUISignal),

  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_TOGGLEMENUBAR,CDWindow::onCmdToggleMenuBar),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_TOGGLEMENUBAR,CDWindow::onUpdToggleMenuBar),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_TOGGLESTATUSBAR,CDWindow::onCmdToggleStatusBar),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_TOGGLESTATUSBAR,CDWindow::onUpdToggleStatusBar),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_TOGGLETOOLTIPS,CDWindow::onCmdToggleTooltips),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_TOGGLETOOLTIPS,CDWindow::onUpdToggleTooltips),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_ABOUT,CDWindow::onCmdAbout),

  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_STATUSDISC,CDWindow::onUpdStatusDisc),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_STATUSTRACK,CDWindow::onUpdStatusTrack),

  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_RANDOM,CDWindow::onCmdRandom),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_RANDOM,CDWindow::onUpdRandom),

  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_PREFS,CDWindow::onCmdPrefs),

  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_BAND,CDWindow::onCmdBand),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_TRACK,CDWindow::onCmdTrack),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_TRACK,CDWindow::onUpdTrack),

  FXMAPFUNC(SEL_CHANGED,CDWindow::ID_VOLUME,CDWindow::onCmdVolume),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_VOLUME,CDWindow::onCmdVolume),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_VOLUME,CDWindow::onUpdVolume),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_MUTE,CDWindow::onCmdMute),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_MUTE,CDWindow::onUpdMute),
  FXMAPFUNC(SEL_CHANGED,CDWindow::ID_BALANCE,CDWindow::onCmdBalance),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_BALANCE,CDWindow::onCmdBalance),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_BALANCE,CDWindow::onUpdBalance),

  FXMAPFUNCS(SEL_LEFTBUTTONPRESS,CDWindow::ID_SEEKREVERSE,CDWindow::ID_SEEKFORWARD,CDWindow::onActivateSeeker),
  FXMAPFUNCS(SEL_LEFTBUTTONRELEASE,CDWindow::ID_SEEKREVERSE,CDWindow::ID_SEEKFORWARD,CDWindow::onDeactivateSeeker),
  FXMAPFUNC(SEL_TIMEOUT,CDWindow::ID_SEEKRATETIMEOUT,CDWindow::onSeekRateTimeout),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_SEEKREVERSE,CDWindow::onCmdSeekReverse),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_SEEKFORWARD,CDWindow::onCmdSeekForward),
  FXMAPFUNCS(SEL_UPDATE,CDWindow::ID_SEEKREVERSE,CDWindow::ID_SEEKFORWARD,CDWindow::onUpdSeekBtns),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_STOP,CDWindow::onCmdStop),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_STOP,CDWindow::onUpdStop),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_PREV,CDWindow::onCmdPrev),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_NEXT,CDWindow::onCmdNext),
  FXMAPFUNCS(SEL_UPDATE,CDWindow::ID_PREV,CDWindow::ID_NEXT,CDWindow::onUpdSkipBtns),

  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_PLAY,CDWindow::onCmdPlay),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_PLAY,CDWindow::onUpdPlay),

  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_EJECT,CDWindow::onCmdEject)
};

FXIMPLEMENT(CDWindow,FXMainWindow,CDWindowMap,ARRAYNUMBER(CDWindowMap))

#define TIMED_UPDATE      200
#define NODISC_MSG        "Data or no disc loaded"
#define DEFAULTFORE       FXRGB(0,160,255)
#define DEFAULTBACK       FXRGB(0,0,0)

CDWindow::CDWindow(FXApp* app)
: FXMainWindow(app,PROG_PACKAGE,NULL,NULL,DECOR_ALL,0,0,0,0),
  stoponexit(TRUE),
  startmode(CDSTART_NONE),
  lcdforeclr(FXRGB(255,255,255)),
  lcdbackclr(FXRGB(0,0,0)),
  iconclr(FXRGB(0,0,0)),
  font(NULL),
  seektrack(0),
  seekrate(1),
  initseekrate(1),
  fastseekrate(5),
  fastseekstart(5),
  usecddb(TRUE),
  tooltip(NULL),
  prefsbox(NULL)
{
  // Create icons for mute button
  mutebmp.push_back(new CDBMPIcon(getApp(),nomute_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  mutebmp.push_back(new CDBMPIcon(getApp(),mute_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));

  // Create icons for button controls
  btnbmp.push_back(new CDBMPIcon(getApp(),reverse_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  btnbmp.push_back(new CDBMPIcon(getApp(),forward_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  btnbmp.push_back(new CDBMPIcon(getApp(),stop_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  btnbmp.push_back(new CDBMPIcon(getApp(),play_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  btnbmp.push_back(new CDBMPIcon(getApp(),pause_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  btnbmp.push_back(new CDBMPIcon(getApp(),prev_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  btnbmp.push_back(new CDBMPIcon(getApp(),next_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  btnbmp.push_back(new CDBMPIcon(getApp(),eject_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));

  // Create icons for display
  lcdbmp.push_back(new CDBMPIcon(getApp(),intro_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  lcdbmp.push_back(new CDBMPIcon(getApp(),rand_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  lcdbmp.push_back(new CDBMPIcon(getApp(),repeatn_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  lcdbmp.push_back(new CDBMPIcon(getApp(),repeatt_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  lcdbmp.push_back(new CDBMPIcon(getApp(),repeatd_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  lcdbmp.push_back(new CDBMPIcon(getApp(),prefs_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));

  // Create disabled icons
  disbmp.push_back(new CDBMPIcon(getApp(),introd_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  disbmp.push_back(new CDBMPIcon(getApp(),randd_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));

  // Create menubar and statusbar
  menubar=new FXMenuBar(this,FRAME_RAISED|LAYOUT_SIDE_TOP|LAYOUT_FILL_X);

  statusbar=new FXStatusBar(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

  // Add status items to statusbar
  statusbar->getStatusLine()->setNormalText("");
  FXLabel* label=new FXLabel(statusbar,"00:00",NULL,FRAME_SUNKEN|LAYOUT_RIGHT);
  label->setTipText("Album Length");
  label->setTarget(this);
  label->setSelector(ID_STATUSDISC);
  label=new FXLabel(statusbar,"00:00",NULL,FRAME_SUNKEN|LAYOUT_RIGHT);
  label->setTipText("Track Length");
  label->setTarget(this);
  label->setSelector(ID_STATUSTRACK);

  // Main frame
  FXVerticalFrame* contents=new FXVerticalFrame(this,LAYOUT_FILL_X|LAYOUT_FILL_Y);

  // LCD contents
  lcdframe=new FXHorizontalFrame(contents,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 2,2,2,2, 0,0);
  FXVerticalFrame* canvasItems=new FXVerticalFrame(lcdframe,LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0, 0,0);
  canvas=new CDCanvas(canvasItems,this,ID_CANVAS,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,114,32);
  FXHorizontalFrame* lcdbtns=new FXHorizontalFrame(canvasItems,LAYOUT_FILL_X);
  lcdwin.push_back(new FXToggleButton(lcdbtns,FXString::null,FXString::null,disbmp[0],lcdbmp[0],&introtgt,FXDataTarget::ID_VALUE,0, 0,0,0,0, 0,0,0,0));
  lcdwin.push_back(new FXToggleButton(lcdbtns,FXString::null,FXString::null,disbmp[1],lcdbmp[1],this,ID_RANDOM,0, 0,0,0,0, 0,0,0,0));
  FXSwitcher* switcher=new FXSwitcher(lcdbtns,0, 0,0,0,0, 0,0,0,0);
  switcher->setTarget(&repeatmodetgt);
  switcher->setSelector(FXDataTarget::ID_VALUE);
  lcdwin.push_back(new FXButton(switcher,FXString::null,lcdbmp[2],&repeatmodetgt,FXDataTarget::ID_OPTION+CDREPEAT_TRACK,0, 0,0,0,0, 0,0,0,0));
  lcdwin.push_back(new FXButton(switcher,FXString::null,lcdbmp[3],&repeatmodetgt,FXDataTarget::ID_OPTION+CDREPEAT_DISC,0, 0,0,0,0, 0,0,0,0));
  lcdwin.push_back(new FXButton(switcher,FXString::null,lcdbmp[4],&repeatmodetgt,FXDataTarget::ID_OPTION+CDREPEAT_NONE,0, 0,0,0,0, 0,0,0,0));
  lcdwin.push_back(new FXButton(lcdbtns,FXString::null,lcdbmp[5],this,ID_PREFS,0, 0,0,0,0, 0,0,0,0));
  lcdwin.push_back(lcdbtns);

  FXVerticalFrame* textfields=new FXVerticalFrame(lcdframe,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0, 0,0);
  bandtitle=new CDListBox(textfields,this,ID_BAND,LAYOUT_FILL_X);
  albumtitle=new FXLabel(textfields,NODISC_MSG,NULL,JUSTIFY_LEFT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  tracktitle=new CDListBox(textfields,this,ID_TRACK,LAYOUT_FILL_X);

  // Volume controls
  new FXHorizontalSeparator(contents,SEPARATOR_GROOVE|LAYOUT_FILL_X);
  FXHorizontalFrame* mixer=new FXHorizontalFrame(contents,PACK_UNIFORM_WIDTH|LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,2,0, 0,0,0,0);

  FXHorizontalFrame* volume=new FXHorizontalFrame(mixer,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  new FXLabel(volume,"Volume: ",NULL,JUSTIFY_LEFT|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  FXSlider* volumeSlider=new FXSlider(volume,this,ID_VOLUME,SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR|LAYOUT_FILL_X|LAYOUT_CENTER_Y);
  volumeSlider->setRange(0,100);
  volumeSlider->setIncrement(1);

  FXHorizontalFrame* mutebalance=new FXHorizontalFrame(mixer,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,2,0, 0,0,0,0);
  new FXToggleButton(mutebalance,"\tAudio off","\tAudio on",mutebmp[0],mutebmp[1],this,ID_MUTE,TOGGLEBUTTON_TOOLBAR|FRAME_RAISED,0,0,0,0, 2,2,1,1);

  FXHorizontalFrame* balanceFrame=new FXHorizontalFrame(mutebalance,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  new FXLabel(balanceFrame,"Balance: ",NULL,JUSTIFY_LEFT|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  FXSlider* balanceSlider=new FXSlider(balanceFrame,this,ID_BALANCE,SLIDER_HORIZONTAL|SLIDER_INSIDE_BAR|LAYOUT_FILL_X|LAYOUT_CENTER_Y);
  balanceSlider->setRange(0,200);
  balanceSlider->setIncrement(2);

  // Button controls
  new FXHorizontalSeparator(contents,SEPARATOR_GROOVE|LAYOUT_FILL_X);
  FXHorizontalFrame* buttons=new FXHorizontalFrame(contents,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0, 0,0);
  FXVerticalFrame* btnfrm1=new FXVerticalFrame(buttons,FRAME_SUNKEN,0,0,0,0, 0,0,0,0, 0,0);
  new CDSeekButton(btnfrm1,"\tSeek backward",btnbmp[0],this,ID_SEEKREVERSE,FRAME_RAISED,0,0,0,0, 2,2,1,1);
  FXVerticalFrame* btnfrm2=new FXVerticalFrame(buttons,FRAME_SUNKEN,0,0,0,0, 0,0,0,0, 0,0);
  new CDSeekButton(btnfrm2,"\tSeek forward",btnbmp[1],this,ID_SEEKFORWARD,FRAME_RAISED,0,0,0,0, 2,2,1,1);
  new FXFrame(buttons,LAYOUT_FIX_WIDTH,0,0,4);
  FXVerticalFrame* btnfrm3=new FXVerticalFrame(buttons,FRAME_SUNKEN,0,0,0,0, 0,0,0,0, 0,0);
  new FXButton(btnfrm3,"\tStop play",btnbmp[2],this,ID_STOP,FRAME_RAISED,0,0,0,0, 2,2,1,1);
  FXVerticalFrame* btnfrm4=new FXVerticalFrame(buttons,FRAME_SUNKEN,0,0,0,0, 0,0,0,0, 0,0);
  new FXToggleButton(btnfrm4,"\tPlay track","\tPause track",btnbmp[3],btnbmp[4],this,ID_PLAY,FRAME_RAISED,0,0,0,0, 2,2,1,1);
  new FXFrame(buttons,LAYOUT_FIX_WIDTH,0,0,4);
  FXVerticalFrame* btnfrm5=new FXVerticalFrame(buttons,FRAME_SUNKEN,0,0,0,0, 0,0,0,0, 0,0);
  new FXButton(btnfrm5,"\tPrevious track",btnbmp[5],this,ID_PREV,FRAME_RAISED,0,0,0,0, 2,2,1,1);
  FXVerticalFrame* btnfrm6=new FXVerticalFrame(buttons,FRAME_SUNKEN,0,0,0,0, 0,0,0,0, 0,0);
  new FXButton(btnfrm6,"\tNext track",btnbmp[6],this,ID_NEXT,FRAME_RAISED,0,0,0,0, 2,2,1,1);
  FXVerticalFrame* btnfrm7=new FXVerticalFrame(buttons,FRAME_SUNKEN|LAYOUT_RIGHT,0,0,0,0, 0,0,0,0, 0,0);
  new FXButton(btnfrm7,"\tEject",btnbmp[7],this,ID_EJECT,FRAME_RAISED,0,0,0,0, 2,2,1,1);

  // Menu contents
  filemenu=new FXMenuPane(this);
    new FXMenuTitle(menubar,"&File",NULL,filemenu);
    new FXMenuCommand(filemenu,"&Quit\tCtrl-Q",NULL,this,ID_QUIT);
  optionsmenu=new FXMenuPane(this);
    new FXMenuTitle(menubar,"&Options",NULL,optionsmenu);
    new FXMenuCheck(optionsmenu,"&Intro",&introtgt,FXDataTarget::ID_VALUE);
    new FXMenuCheck(optionsmenu,"&Shuffle",this,ID_RANDOM);
    new FXMenuSeparator(optionsmenu);
    timemenu=new FXMenuPane(this);
      new FXMenuCascade(optionsmenu,"&Time Mode",NULL,timemenu);
      new FXMenuRadio(timemenu,"&Album",&timemodetgt,FXDataTarget::ID_OPTION+CDTIME_DISC);
      new FXMenuRadio(timemenu,"&Track",&timemodetgt,FXDataTarget::ID_OPTION+CDTIME_TRACK);
      new FXMenuRadio(timemenu,"Albu&m Remaining",&timemodetgt,FXDataTarget::ID_OPTION+CDTIME_DISCREM);
      new FXMenuRadio(timemenu,"Trac&k Remaining",&timemodetgt,FXDataTarget::ID_OPTION+CDTIME_TRACKREM);
    repeatmenu=new FXMenuPane(this);
      new FXMenuCascade(optionsmenu,"&Repeat Mode",NULL,repeatmenu);
      new FXMenuRadio(repeatmenu,"&None",&repeatmodetgt,FXDataTarget::ID_OPTION+CDREPEAT_NONE);
      new FXMenuRadio(repeatmenu,"&Track",&repeatmodetgt,FXDataTarget::ID_OPTION+CDREPEAT_TRACK);
      new FXMenuRadio(repeatmenu,"&Album",&repeatmodetgt,FXDataTarget::ID_OPTION+CDREPEAT_DISC);
#ifndef WIN32
    // Disable UI components for operations not supported on Windows
    new FXMenuSeparator(optionsmenu);
    new FXMenuCheck(optionsmenu,"Stop Play on &Exit",&stoponexittgt,FXDataTarget::ID_VALUE);
#endif
    new FXMenuSeparator(optionsmenu);
    new FXMenuCommand(optionsmenu,"&Preferences...\tCtrl+P",NULL,this,ID_PREFS);
  viewmenu=new FXMenuPane(this);
    new FXMenuTitle(menubar,"&View",NULL,viewmenu);
    new FXMenuCheck(viewmenu,"&Menubar",this,ID_TOGGLEMENUBAR);
    new FXMenuCheck(viewmenu,"&Statusbar",this,ID_TOGGLESTATUSBAR);
  helpmenu=new FXMenuPane(this);
    new FXMenuTitle(menubar,"&Help",NULL,helpmenu);
    new FXMenuCheck(helpmenu,"&Tool tips\tCtl-B",this,ID_TOGGLETOOLTIPS);
    new FXMenuSeparator(helpmenu);
    new FXMenuCommand(helpmenu,"&About "PROG_PACKAGE"...",NULL,this,ID_ABOUT);


  // Create popup menu
  popupmenu=new FXMenuPane(this);
  new FXMenuCascade(popupmenu,"&File",NULL,filemenu);
  new FXMenuCascade(popupmenu,"&Options",NULL,optionsmenu);
  new FXMenuCascade(popupmenu,"&View",NULL,viewmenu);
  new FXMenuCascade(popupmenu,"&Help",NULL,helpmenu);

  // Connect data targets
  stoponexittgt.connect(stoponexit);
  startmodetgt.connect(startmode);
  timemodetgt.connect(canvas->timemode);
  repeatmodetgt.connect(cdplayer.repeatMode);
  introtgt.connect(cdplayer.intro);
}

void CDWindow::create()
{
  CDBMPArray::iterator iter;

  FXMainWindow::create();

  // Create icons
  for(iter=mutebmp.begin();iter!=mutebmp.end();++iter)
    (*iter)->create();

  for(iter=btnbmp.begin();iter!=btnbmp.end();++iter)
    (*iter)->create();

  for(iter=lcdbmp.begin();iter!=lcdbmp.end();++iter)
    (*iter)->create();

  for(iter=disbmp.begin();iter!=disbmp.end();++iter)
    (*iter)->create();

  readRegistry();
  checkDevices();
  handle(this,MKUINT(ID_BAND,SEL_COMMAND),(void*)bandtitle->getCurrentItem());

  getApp()->addTimeout(this,ID_TIMEOUT,TIMED_UPDATE);

  //Start action
  if((startmode==CDSTART_STOP)&&(cdplayer.getStatus()==CDLYTE_PLAYING||cdplayer.getStatus()==CDLYTE_PAUSED))
    cdplayer.stop();
  else if((startmode==CDSTART_START)&&(cdplayer.getStatus()==CDLYTE_PAUSED))
    cdplayer.resume();
  else if((startmode==CDSTART_START)&&(cdplayer.getStatus()!=CDLYTE_PLAYING))
    cdplayer.play();
}

void CDWindow::readRegistry()
{
  FXint x,y,w,h;
  FXColor dfg,dbg,ic;
  FXString fontspec;

  // Get position
  x=getApp()->reg().readIntEntry("SETTINGS","x",50);
  y=getApp()->reg().readIntEntry("SETTINGS","y",50);

  w=getApp()->reg().readIntEntry("SETTINGS","width",300);
  h=getApp()->reg().readIntEntry("SETTINGS","height",getDefaultHeight());

  if(!getApp()->reg().readIntEntry("SETTINGS","showmenubar",TRUE))
    menubar->hide();
  if(!getApp()->reg().readIntEntry("SETTINGS","showstatusbar",TRUE))
    statusbar->hide();
  if(getApp()->reg().readIntEntry("SETTINGS","showtooltips",TRUE))
  {
    tooltip=new FXToolTip(getApp(),TOOLTIP_NORMAL);
    tooltip->create();
  }

  canvas->setTimeMode(getApp()->reg().readUnsignedEntry("SETTINGS","timemode",canvas->getTimeMode()));
  startmode=getApp()->reg().readIntEntry("SETTINGS","startaction",startmode);
#ifdef WIN32
  if(startmode==CDSTART_STOP) startmode=CDSTART_NONE;
#else
  // Disable preferences for operations not supported on Windows
  stoponexit=getApp()->reg().readIntEntry("SETTINGS","stoponexit",stoponexit);
#endif
  cdplayer.setRepeatMode(getApp()->reg().readUnsignedEntry("SETTINGS","repeatmode",cdplayer.getRepeatMode()));

  // Colors and fonts
  dfg=getApp()->reg().readColorEntry("SETTINGS","lcdforecolor",DEFAULTFORE);
  dbg=getApp()->reg().readColorEntry("SETTINGS","lcdbackcolor",DEFAULTBACK);
  ic=getApp()->reg().readColorEntry("SETTINGS","iconcolor",DEFAULTBACK);
  fontspec=getApp()->reg().readStringEntry("SETTINGS","font",getDisplayFont()->getFont().text());

  // Seek rate settings
  initseekrate=getApp()->reg().readUnsignedEntry("SETTINGS","initseekrate",initseekrate);
  fastseekrate=getApp()->reg().readUnsignedEntry("SETTINGS","fastseekrate",fastseekrate);
  fastseekstart=getApp()->reg().readUnsignedEntry("SETTINGS","fastseekstart",fastseekstart);

  // CD Info settings
  usecddb=getApp()->reg().readIntEntry("SETTINGS","usecddb",usecddb);
  cddbsettings.proxy=getApp()->reg().readIntEntry("SETTINGS","proxy",cddbsettings.proxy);
  cddbsettings.proxyaddr=getApp()->reg().readStringEntry("SETTINGS","proxyaddr",cddbsettings.proxyaddr.text());
  cddbsettings.proxyport=getApp()->reg().readUnsignedEntry("SETTINGS","proxyport",cddbsettings.proxyport);
  cddbsettings.cddbproto=getApp()->reg().readUnsignedEntry("SETTINGS","cddbproto",cddbsettings.cddbproto);
  cddbsettings.cddbaddr=getApp()->reg().readStringEntry("SETTINGS","cddbaddr",cddbsettings.cddbaddr.text());
  cddbsettings.cddbpport=getApp()->reg().readUnsignedEntry("SETTINGS","cddbpport",cddbsettings.cddbpport);
  cddbsettings.cddbport=getApp()->reg().readUnsignedEntry("SETTINGS","cddbport",cddbsettings.cddbport);
  cddbsettings.cddbexec=getApp()->reg().readStringEntry("SETTINGS","cddbscript",cddbsettings.cddbexec.text());
  cddbsettings.promptmultiple=getApp()->reg().readIntEntry("SETTINGS","cddbpromptmultiple",cddbsettings.promptmultiple);
  cddbsettings.localcopy=getApp()->reg().readIntEntry("SETTINGS","cddblocalcopy",cddbsettings.localcopy);

  // Apply settings
  setDisplayForeground(dfg);
  setDisplayBackground(dbg);
  setIconColor(ic);

  font=new FXFont(getApp(),fontspec);
  setDisplayFont(font);

  move(x,y);
  resize(w,h);
}

void CDWindow::writeRegistry()
{
  getApp()->reg().writeIntEntry("SETTINGS","x",getX());
  getApp()->reg().writeIntEntry("SETTINGS","y",getY());

  getApp()->reg().writeIntEntry("SETTINGS","width",getWidth());
  getApp()->reg().writeIntEntry("SETTINGS","height",getHeight());

  getApp()->reg().writeIntEntry("SETTINGS","showmenubar",menubar->shown());
  getApp()->reg().writeIntEntry("SETTINGS","showstatusbar",statusbar->shown());
  getApp()->reg().writeIntEntry("SETTINGS","showtooltips",(tooltip!=NULL)?TRUE:FALSE);

  getApp()->reg().writeUnsignedEntry("SETTINGS","timemode",canvas->getTimeMode());
#ifndef WIN32
  // Disable preferences for operations not supported on Windows
  getApp()->reg().writeIntEntry("SETTINGS","stoponexit",stoponexit);
#endif
  getApp()->reg().writeIntEntry("SETTINGS","startaction",startmode);
  getApp()->reg().writeUnsignedEntry("SETTINGS","repeatmode",cdplayer.getRepeatMode());

  getApp()->reg().writeColorEntry("SETTINGS","lcdforecolor",lcdforeclr);
  getApp()->reg().writeColorEntry("SETTINGS","lcdbackcolor",lcdbackclr);
  getApp()->reg().writeColorEntry("SETTINGS","iconcolor",iconclr);

  getApp()->reg().writeStringEntry("SETTINGS","font",getDisplayFont()->getFont().text());

  getApp()->reg().writeUnsignedEntry("SETTINGS","initseekrate",initseekrate);
  getApp()->reg().writeUnsignedEntry("SETTINGS","fastseekrate",fastseekrate);
  getApp()->reg().writeUnsignedEntry("SETTINGS","fastseekstart",fastseekstart);

  getApp()->reg().writeIntEntry("SETTINGS","usecddb",usecddb);
  getApp()->reg().writeIntEntry("SETTINGS","proxy",cddbsettings.proxy);
  getApp()->reg().writeStringEntry("SETTINGS","proxyaddr",cddbsettings.proxyaddr.text());
  getApp()->reg().writeUnsignedEntry("SETTINGS","proxyport",cddbsettings.proxyport);
  getApp()->reg().writeUnsignedEntry("SETTINGS","cddbproto",cddbsettings.cddbproto);
  getApp()->reg().writeStringEntry("SETTINGS","cddbaddr",cddbsettings.cddbaddr.text());
  getApp()->reg().writeUnsignedEntry("SETTINGS","cddbpport",cddbsettings.cddbpport);
  getApp()->reg().writeUnsignedEntry("SETTINGS","cddbport",cddbsettings.cddbport);
  getApp()->reg().writeStringEntry("SETTINGS","cddbscript",cddbsettings.cddbexec.text());
  getApp()->reg().writeIntEntry("SETTINGS","cddbpromptmultiple",cddbsettings.promptmultiple);
  getApp()->reg().writeIntEntry("SETTINGS","cddblocalcopy",cddbsettings.localcopy);

  getApp()->reg().write();
}

FXbool CDWindow::checkDevices()
{
  FXint i,n,player=0;
  FXuint total=0;
  FXString entnam;
  FXString* devname=NULL;

  // Look in registry for CD-ROMs
  if(getApp()->reg().existingSection("DEVICES"))
  {
    n=getApp()->reg().readIntEntry("DEVICES","total",0);
    for(i=0;i<n;i++)
    {
      entnam.format("device%d",i);
      const FXchar* entstr=getApp()->reg().readStringEntry("DEVICES",entnam.text(),NULL);
      if(entstr!=NULL)
      {
	total++;
	devname=new FXString(entstr);
        bandtitle->appendItem(*devname,NULL,(void*)devname);
      }
    }
  }

  // Scan for devices if the registry is empty
  if(total==0)
  {
    std::vector<FXString> devices;
    std::vector<FXString>::iterator iter;

    scanDevices(devices);
    for(iter=devices.begin();iter!=devices.end();++iter)
    {
      devname=new FXString(*iter);
      bandtitle->appendItem(*devname,NULL,(void*)devname);

      // Add item to registry
      entnam.format("device%d",total);
      getApp()->reg().writeStringEntry("DEVICES",entnam.text(),devname->text());
      total++;
    }

    // Commit to registry
    getApp()->reg().writeIntEntry("DEVICES","total",total);
    getApp()->reg().write();
  }

  //See if a device is currently active
  n=bandtitle->getNumItems();
  for(i=0;i<n;i++)
  {
    devname=(FXString*)bandtitle->getItemData(i);
    if(cdplayer.init(*devname))
    {
      if(cdplayer.getStatus()==CDLYTE_PLAYING||cdplayer.getStatus()==CDLYTE_PAUSED)
      {
	player=i;
	cdplayer.finish();
	break;
      }
      cdplayer.finish();
    }
  }

  bandtitle->setCurrentItem(player);
  bandtitle->setNumVisible(total);

  return true;
}

FXbool CDWindow::loadDiscData()
{
  if(!cdplayer.isValid()||!cdplayer.isDiscPresent()||!cdplayer.isAudioDisc())
  {
    FXint band=bandtitle->getCurrentItem();
    FXString* deviceStr=(FXString*)bandtitle->getItemData(band);
    FXString title;

    //Relabel for empty media
    title.format("%s - <%s>",NODISC_MSG,deviceStr->text());
    bandtitle->setItemText(band,title);
    bandtitle->setTipText(title);
    albumtitle->setText(NODISC_MSG);
    albumtitle->setTipText(NODISC_MSG);
    tracktitle->clearItems();
    tracktitle->appendItem(NODISC_MSG);
    tracktitle->setTipText(NODISC_MSG);
    tracktitle->setCurrentItem(0);
    tracktitle->setNumVisible(1);
  }
  else
  {
    // Display default disc info
    CDData data;
    genDefaultInfo(&data);
    displayDiscData(data);

    if(usecddb)
    {
      // Request data from external source
      FXGUISignal* signal=new FXGUISignal(getApp(),this,ID_GUISIGNAL);
      CDDBInfo* info=new CDDBInfo(cddbsettings,cdplayer,signal);
      signal->setData(info);

      CDInfoTask* task=new CDInfoTask(info);
      tasklist.push_back(task);
      task->start();
    }
  }

  return TRUE;
}

void CDWindow::genDefaultInfo(CDData* data)
{
  FXint i,s,n;

  data->artist="Unknown Artist";
  data->title="Unknown Album";
  data->year="0";
  data->genre="Unknown";

  s=cdplayer.getStartTrack();
  n=cdplayer.getNumTracks();
  for(i=0;i<n;i++)
  {
    data->trackTitle.append(FXStringFormat("Track %d",i+s));
    data->trackArtist.append("Unknown Artist");
  }
}

void CDWindow::displayDiscData(const CDData& data)
{
  FXint band=bandtitle->getCurrentItem();
  FXString* deviceStr=(FXString*)bandtitle->getItemData(band);
  FXint i,s,n;
  FXString title;

  title.format("%s - <%s>",data.artist.text(),deviceStr->text());
  bandtitle->setItemText(band,title);
  bandtitle->setTipText(title);
  albumtitle->setText(data.title);
  albumtitle->setTipText(data.title);

  //Load all tracks
  s=cdplayer.getStartTrack();
  n=cdplayer.getNumTracks();
  tracktitle->clearItems();
  for(i=0;i<n;i++)
  {
    struct track_info track;
    cdplayer.getTrackInfo(i,track);
    if(track.track_type==CDLYTE_TRACK_AUDIO)
    {
      title.format("%d. %s (%d:%02d)",i+s,data.trackTitle[i].text(),track.track_length.minutes,track.track_length.seconds);
      tracktitle->appendItem(title);
    }
  }

  //Size our drop down list - no empty list spaces
  tracktitle->setNumVisible((cdplayer.getNumTracks()>8)?8:cdplayer.getNumTracks());
}

void CDWindow::setDisplayFont(FXFont* font)
{
  bandtitle->setFont(font);
  tracktitle->setFont(font);
  albumtitle->setFont(font);
  resize(290,getDefaultHeight());
}

FXFont* CDWindow::getDisplayFont() const
{
  return albumtitle->getFont();
}

void CDWindow::setDisplayForeground(FXColor color)
{
  CDBMPArray::iterator iter;

  //Needed for dual color icons.  If they are equal, modify slightly.
  if(color==lcdbackclr)
  {
    FXuchar r=FXREDVAL(color),g=FXGREENVAL(color),b=FXBLUEVAL(color);
    color=FXRGB((r<255)?r+1:r-1,(g<255)?g+1:g-1,(b<255)?b+1:b-1);
  }

  //Fonts
  bandtitle->setForeColor(color);
  albumtitle->setTextColor(color);
  tracktitle->setForeColor(color);

  //Widgets
  canvas->setDisplayForeground(color);

  //Icons
  for(iter=mutebmp.begin();iter!=mutebmp.end();++iter)
    (*iter)->swapColor(lcdforeclr,color);

  for(iter=lcdbmp.begin();iter!=lcdbmp.end();++iter)
    (*iter)->swapColor(lcdforeclr,color);

  lcdforeclr=color;
}

FXColor CDWindow::getDisplayForeground() const
{
  return lcdforeclr;
}

void CDWindow::setDisplayBackground(FXColor color)
{
  CDBMPArray::iterator biter;
  FXWinArray::iterator witer;

  //Needed for dual color icons.  If they are equal, modify slightly.
  if(color==lcdforeclr)
  {
    FXuchar r=FXREDVAL(color),g=FXGREENVAL(color),b=FXBLUEVAL(color);
    color=FXRGB((r<255)?r+1:r-1,(g<255)?g+1:g-1,(b<255)?b+1:b-1);
  }

  lcdframe->setBackColor(color);

  bandtitle->setBackColor(color);
  albumtitle->setBackColor(color);
  tracktitle->setBackColor(color);

  //Widgets
  canvas->setDisplayBackground(color);

  for(witer=lcdwin.begin();witer!=lcdwin.end();++witer)
    (*witer)->setBackColor(color);

  //Icons
  for(biter=mutebmp.begin();biter!=mutebmp.end();++biter)
    (*biter)->swapColor(lcdbackclr,color);

  lcdbackclr=color;
}

FXColor CDWindow::getDisplayBackground() const
{
  return lcdbackclr;
}

void CDWindow::setIconColor(FXColor color)
{
  CDBMPArray::iterator iter;

  for(iter=btnbmp.begin();iter!=btnbmp.end();++iter)
    (*iter)->swapColor(iconclr,color);

  iconclr=color;
}

FXColor CDWindow::getIconColor() const
{
  return iconclr;
}

FXbool CDWindow::addDevice(const FXString& devnam)
{
  FXint n=getApp()->reg().readIntEntry("DEVICES","total",0);
  FXString *name=new FXString(devnam);
  FXString entry;

  // Check device validity
  if(!checkDevice(devnam))
    return FALSE;

  // Add to list
  bandtitle->appendItem(devnam,NULL,(void*)name);
  bandtitle->setNumVisible(n+1);

  // Get data for device

  // Add to registry
  entry.format("device%d",n);
  getApp()->reg().writeIntEntry("DEVICES","total",n+1);
  getApp()->reg().writeStringEntry("DEVICES",entry.text(),devnam.text());
  getApp()->reg().write();

  return TRUE;
}

FXbool CDWindow::removeDevice(const FXString& devnam)
{
  FXint i,n=bandtitle->getNumItems();
  FXint item=-1;
  FXString *data,entry;
  FXbool stopngo=FALSE;

  // Look for device
  for(i=0;i<n;i++)
  {
    data=(FXString*)bandtitle->getItemData(i);
    if(*data==devnam)
    {
      item=i;
      break;
    }
  }

  if(item<0)
    return FALSE;

  stopngo=(item==bandtitle->getCurrentItem())?TRUE:FALSE;

  // Remove item from list
  bandtitle->removeItem(item);
  bandtitle->setNumVisible(n-1);

  // Remove device data

  // Remove from registry
  n=getApp()->reg().readIntEntry("DEVICES","total",0);
  getApp()->reg().writeIntEntry("DEVICES","total",n-1);
  for(i=item;i<(n-1);i++)
  {
    // Move each item up
    entry.format("device%d",i+1);
    const FXchar* entstr=getApp()->reg().readStringEntry("DEVICES",entry.text(),NULL);

    entry.format("device%d",i);
    getApp()->reg().writeStringEntry("DEVICES",entry.text(),entstr);
  }
  entry.format("device%d",n-1);
  getApp()->reg().deleteEntry("DEVICES",entry.text());
  getApp()->reg().write();

  // Reset player if the active device was deleted
  if(stopngo)
    handle(this,MKUINT(ID_BAND,SEL_COMMAND),(void*)bandtitle->getCurrentItem());

  return TRUE;
}

long CDWindow::onMouseUp(FXObject*,FXSelector,void* ptr)
{
  FXEvent* event=(FXEvent*)ptr;
  if(!event->moved)
  {
    popupmenu->popup(NULL,event->root_x,event->root_y);
    getApp()->runModalWhileShown(popupmenu);
  }
  return 1;
}

long CDWindow::onPaint(FXObject*,FXSelector,void*)
{
  struct disc_info di;

  cd_init_disc_info(&di);
  cdplayer.getDiscInfo(di);
  canvas->doDraw(cdplayer.getCurrentTrack(),&di);
  cd_free_disc_info(&di);
  return 1;
}

//Timer to keep track of disc state
long CDWindow::onTimeout(FXObject*,FXSelector,void*)
{
  if(cdplayer.isValid())
  {
    cdplayer.update();

    if(cdplayer.isDiscPresent())
    {
      //We'll need to load the info if the disk is new
      if(albumtitle->getText()==NODISC_MSG)
        loadDiscData();
    }
    else
    {
      //No disc is present
      if(albumtitle->getText()!=NODISC_MSG)
        loadDiscData();
    }
  }

  canvas->setBlinkMode((cdplayer.getStatus()==CDLYTE_PAUSED)?TRUE:FALSE);
  canvas->update();

  getApp()->addTimeout(this,ID_TIMEOUT,TIMED_UPDATE);

  return 1;
}

long CDWindow::onGUISignal(FXObject* sender,FXSelector,void* ptr)
{
  CDInfo* info=(CDInfo*)ptr;

  // Check state
  if(info->getStatus()==CDINFO_INTERACT)
  {
    info->getUserInput(this);
  }
  else
  {
    // Find the associated info task object
    CDInfoTaskList::iterator iter;
    CDInfoTask* task=NULL;
    for(iter=tasklist.begin(); iter!=tasklist.end(); ++iter)
    {
      if((*iter)->getInfo()==info)
      {
        task=*iter;
        break;
      }
    }

    // Only process info if it was in the task list, and was the latest task
    if(task!=NULL)
    {
      if(tasklist.back()==task)
      {
        // The task is done or has encountered an error
        if(info->getStatus()==CDINFO_DONE)
        {
          // Load the data
          CDData data;
          if(info->getData(&data))
          {
            displayDiscData(data);
          }
        }
      }

      // Delete the info task
      tasklist.erase(iter);
      task->join();
      delete task;
    }

    // Delete the CDInfo object
    delete info;

    // Delete the GUISignal
    delete sender;
  }

  return 1;
}

long CDWindow::onCmdToggleMenuBar(FXObject*,FXSelector,void*)
{
  if(menubar->shown())
    menubar->hide();
  else
    menubar->show();
  resize(getWidth(),getDefaultHeight());

  return 1;
}

long CDWindow::onUpdToggleMenuBar(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(menubar->shown())?ID_CHECK:ID_UNCHECK;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdToggleStatusBar(FXObject*,FXSelector,void*)
{
  if(statusbar->shown())
    statusbar->hide();
  else
    statusbar->show();
  resize(getWidth(),getDefaultHeight());

  return 1;
}

long CDWindow::onUpdToggleStatusBar(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(statusbar->shown())?ID_CHECK:ID_UNCHECK;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdToggleTooltips(FXObject*,FXSelector,void*)
{
  if(tooltip!=NULL)
  {
    delete tooltip;
    tooltip=NULL;
  }
  else
  {
    tooltip=new FXToolTip(getApp(),TOOLTIP_NORMAL);
    tooltip->create();
  }

  return 1;
}

long CDWindow::onUpdToggleTooltips(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(tooltip!=NULL)?ID_CHECK:ID_UNCHECK;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdAbout(FXObject*,FXSelector,void*)
{
  FXString msg("Compact Disc Player\nVersion "PROG_VERSION"\n\nCopyright (C) 2000-2010 Dustin Graves (dgraves@computer.org)\n\n"\
"This software uses the FOX Platform Independent GUI Toolkit Library.\n"\
"The FOX Library is Copyright (C) 1997,2000-2010 Jeroen van der Zijp and is\n"\
"available freely under the GNU Lesser Public License at the following site:\n"\
"http://www.fox-toolkit.org");

  FXDialogBox about(this,"About Box",DECOR_TITLE|DECOR_BORDER);
  FXHorizontalFrame* buttons=new FXHorizontalFrame(&about,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);
  new FXButton(buttons,"&Close",NULL,&about,FXDialogBox::ID_ACCEPT,BUTTON_INITIAL|BUTTON_DEFAULT|LAYOUT_RIGHT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20);
  new FXHorizontalSeparator(&about,SEPARATOR_RIDGE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);
  FXVerticalFrame* aboutframe=new FXVerticalFrame(&about,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  new FXLabel(aboutframe,"About "PROG_PACKAGE);
  new FXHorizontalSeparator(aboutframe,SEPARATOR_LINE|LAYOUT_FILL_X);
  FXHorizontalFrame* aboutlabels=new FXHorizontalFrame(aboutframe,LAYOUT_FILL_X|LAYOUT_FILL_Y);
//  new FXLabel(aboutlabels,NULL,bigcalc,JUSTIFY_LEFT|LAYOUT_CENTER_Y,0,0,0,0, 20,20,20,20);
  new FXLabel(aboutlabels,msg,NULL,JUSTIFY_LEFT|LAYOUT_CENTER_Y,0,0,0,0, 0,20,20,20);

  about.execute(PLACEMENT_OWNER);

  return 1;
}

long CDWindow::onCmdQuit(FXObject*,FXSelector,void*)
{
  if(stoponexit==TRUE)
  {
    if(cdplayer.getStatus()==CDLYTE_PLAYING||cdplayer.getStatus()==CDLYTE_PAUSED)
      cdplayer.stop();
    cdplayer.finish();
  }

  writeRegistry();

  getApp()->stop();

  return 1;
}

long CDWindow::onUpdStatusDisc(FXObject* sender,FXSelector,void*)
{
  FXString str("00:00");
  if(cdplayer.isDiscPresent())
  {
    struct disc_timeval length;
    cdplayer.getPlayLength(length);
    str.format("%02d:%02d",length.minutes,length.seconds);
  }

  sender->handle(this,MKUINT(ID_SETSTRINGVALUE,SEL_COMMAND),(void*)&str);
  return 1;
}

long CDWindow::onUpdStatusTrack(FXObject* sender,FXSelector,void*)
{
  FXString str("00:00");
  if(cdplayer.isDiscPresent())
  {
    struct disc_timeval length;
    cdplayer.getTrackLength(cdplayer.getCurrentTrack(),length);
    str.format("%02d:%02d",length.minutes,length.seconds);
  }
  sender->handle(this,MKUINT(ID_SETSTRINGVALUE,SEL_COMMAND),(void*)&str);
  return 1;
}

long CDWindow::onCmdPrefs(FXObject*,FXSelector,void*)
{
  if(prefsbox==NULL)
  {
    prefsbox=new CDPreferences(this);
    prefsbox->create();
    prefsbox->show(PLACEMENT_OWNER);
  }
  else
  {
    prefsbox->show(PLACEMENT_OWNER);
  }

  return 1;
}

long CDWindow::onCmdRandom(FXObject*,FXSelector,void*)
{
  cdplayer.setRandom(!cdplayer.getRandom());
  return 1;
}

long CDWindow::onUpdRandom(FXObject* sender,FXSelector,void*)
{
  FXuint msg=cdplayer.getRandom()?ID_CHECK:ID_UNCHECK;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdBand(FXObject*,FXSelector,void* ptr)
{
  FXint band=(FXint)(FXival)ptr;
  if(band>=0&&band<bandtitle->getNumItems())
  {
    FXString* devname=(FXString*)bandtitle->getItemData(band);

    // Stop any device that is currently in use
    if(cdplayer.isValid())
    {
      cdplayer.stop();
      cdplayer.finish();
    }

    // Open CD-ROM device
    cdplayer.init(devname->text());

    // Load data for disc
    loadDiscData();
  }
  return 1;
}

long CDWindow::onCmdTrack(FXObject*,FXSelector,void* ptr)
{
  cdplayer.setCurrentTrack(((FXint)(FXival)ptr)+1);
  if(cdplayer.getStatus()!=CDLYTE_PLAYING&&cdplayer.getStatus()!=CDLYTE_PAUSED)
    cdplayer.play();
  return 1;
}

long CDWindow::onUpdTrack(FXObject*,FXSelector,void*)
{
  FXint track=(cdplayer.isValid()&&cdplayer.isDiscPresent()&&cdplayer.isAudioDisc())?cdplayer.getCurrentTrack()-cdplayer.getStartTrack():0;
  if(track>-1&&track<tracktitle->getNumItems()&&track<cdplayer.getNumTracks())
  {
    tracktitle->setCurrentItem(track);
    tracktitle->setTipText(tracktitle->getItemText(track));
  }
  return 1;
}

long CDWindow::onCmdVolume(FXObject*,FXSelector,void* ptr)
{
  FXString str;
  FXint vollevel=(FXint)(FXival)ptr;

  cdplayer.setVolume(vollevel);

  return 1;
}

long CDWindow::onUpdVolume(FXObject* sender,FXSelector,void*)
{
  FXint vollevel=cdplayer.getVolume();
  sender->handle(this,MKUINT(ID_SETVALUE,SEL_COMMAND),(void*)(FXival)vollevel);

  return 1;
}

long CDWindow::onCmdMute(FXObject*,FXSelector,void*)
{
  cdplayer.setMute(!cdplayer.getMute());
  return 1;
}

long CDWindow::onUpdMute(FXObject* sender,FXSelector,void*)
{
  FXuint msg=cdplayer.getMute()?ID_CHECK:ID_UNCHECK;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdBalance(FXObject*,FXSelector,void* ptr)
{
  FXint value=(FXint)(FXival)ptr;
  FXdouble volbalance=0.0;

  //determine percentege of balance to what side
  if(value>100)  //Balance goes right
    volbalance=((FXdouble)value-100.0)/100.0;
  else if(value<100)          //Balance goes left
    volbalance=((FXdouble)value/100.0)-1.0;
  else
    volbalance=0.0;

  cdplayer.setBalance(volbalance);

  return 1;
}

long CDWindow::onUpdBalance(FXObject* sender,FXSelector,void*)
{
  FXdouble volbalance=cdplayer.getBalance();
  FXint value=100+(FXint)(100*volbalance);

  sender->handle(this,MKUINT(ID_SETVALUE,SEL_COMMAND),(void*)(FXival)value);

  return 1;
}

long CDWindow::onActivateSeeker(FXObject*,FXSelector,void*)
{
  seektrack=cdplayer.getCurrentTrack();
  cdplayer.getTrackTime(seektime);
  seekrate=initseekrate;
  getApp()->addTimeout(this,ID_SEEKRATETIMEOUT,fastseekstart*1000);
  return 0;
}

long CDWindow::onDeactivateSeeker(FXObject*,FXSelector,void*)
{
  getApp()->removeTimeout(this,ID_SEEKRATETIMEOUT);
  return 0;
}

long CDWindow::onSeekRateTimeout(FXObject*,FXSelector,void*)
{
  seekrate=fastseekrate;
  return 1;
}

long CDWindow::onCmdSeekReverse(FXObject*,FXSelector,void*)
{
  seektime.seconds-=seekrate;
  if(seektime.seconds<0)
  {
    seektime.minutes--;
    seektime.seconds+=60;
  }

  //Check to see if we skipped over track boundary
  if(seektime.minutes<0)
  {
    //What should it do for first track?  Stay there I think.
    FXbool random=cdplayer.getRandom();
    if((seektrack==cdplayer.getStartTrack())&&!random)
    {
      seektime.minutes=0;
      seektime.seconds=0;
    }
    else
    {
      if(random)
      {
        seektrack=cdplayer.randomTrack();
	if(seektrack==0)
	{
	  cdplayer.stop();
	  return 1;
	}
      }
      else
        seektrack--;

      if(cdplayer.getIntro())
      {
	FXuint seconds=cdplayer.getIntroLength();
        seektime.minutes=seconds/60;
        seektime.seconds=seconds%60;
      }
      else
      {
        cdplayer.getTrackLength(seektrack,seektime);
      }
    }
  }

  cdplayer.playTrackPos(seektrack,&seektime);

  return 1;
}

long CDWindow::onCmdSeekForward(FXObject*,FXSelector,void*)
{
  struct disc_timeval boundary;
  if(cdplayer.getIntro())
  {
    FXuint seconds=cdplayer.getIntroLength();
    boundary.minutes=seconds/60;
    boundary.seconds=seconds%60;
  }
  else
  {
    cdplayer.getTrackLength(seektrack,boundary);
  }

  seektime.seconds+=seekrate;
  if(seektime.seconds>59)
  {
    seektime.minutes++;
    seektime.seconds-=60;
  }

  //Check to see if we skipped over track boundary
  if((seektime.minutes>=boundary.minutes)&&(seektime.seconds>boundary.seconds))
  {
    FXint mode=cdplayer.getRepeatMode();
    if(cdplayer.getRandom())
    {
      if(mode!=CDREPEAT_TRACK)
      {
        seektrack=cdplayer.randomTrack();
        if(seektrack==0)
	{
	  cdplayer.stop();
	  return 1;
	}
      }
    }
    else
    {
      if(mode!=CDREPEAT_TRACK)
      {
        if(seektrack<cdplayer.getNumTracks())
          seektrack++;
        else if(mode==CDREPEAT_DISC)
	  seektrack=cdplayer.getStartTrack();
        else
	{
	  cdplayer.stop();
	  return 1;
	}
      }
    }
    seektime.minutes=0;
    seektime.seconds=0;
  }

  cdplayer.playTrackPos(seektrack,&seektime);

  return 1;
}

long CDWindow::onUpdSeekBtns(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(cdplayer.isValid()&&cdplayer.isDiscPresent()&&cdplayer.isAudioDisc()&&(cdplayer.getStatus()==CDLYTE_PLAYING||cdplayer.getStatus()==CDLYTE_PAUSED))?ID_ENABLE:ID_DISABLE;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdStop(FXObject*,FXSelector,void*)
{
  cdplayer.stop();
  return 1;
}

long CDWindow::onUpdStop(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(cdplayer.isValid()&&cdplayer.isDiscPresent()&&cdplayer.isAudioDisc()&&(cdplayer.getStatus()==CDLYTE_PLAYING||cdplayer.getStatus()==CDLYTE_PAUSED))?ID_ENABLE:ID_DISABLE;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdPlay(FXObject*,FXSelector,void*)
{
  if(cdplayer.getStatus()==CDLYTE_PLAYING)
    cdplayer.pause();
  else if(cdplayer.getStatus()==CDLYTE_PAUSED)
    cdplayer.resume();
  else
    cdplayer.play();

  return 1;
}

long CDWindow::onUpdPlay(FXObject* sender,FXSelector,void*)
{
  FXint state=FALSE;
  FXuint msg=ID_DISABLE;

  if(cdplayer.isValid()&&cdplayer.isDiscPresent()&&cdplayer.isAudioDisc())
  {
    state=(cdplayer.getStatus()==CDLYTE_PLAYING)?TRUE:FALSE;
    msg=ID_ENABLE;
  }

  sender->handle(this,MKUINT(ID_SETVALUE,SEL_COMMAND),(void*)state);
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);

  return 1;
}

long CDWindow::onCmdPrev(FXObject*,FXSelector,void*)
{
  cdplayer.skipPrev();
  return 1;
}

long CDWindow::onCmdNext(FXObject*,FXSelector,void*)
{
  cdplayer.skipNext();
  return 1;
}

long CDWindow::onUpdSkipBtns(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(cdplayer.isValid()&&cdplayer.isDiscPresent()&&cdplayer.isAudioDisc())?ID_ENABLE:ID_DISABLE;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdEject(FXObject*,FXSelector,void*)
{
  cdplayer.openTray();
  return 1;
}

CDWindow::~CDWindow()
{
  CDBMPArray::iterator iter;
  CDInfoTaskList::iterator task;
  FXint i,n=bandtitle->getNumItems();
  FXString* devnam=NULL;

  getApp()->removeTimeout(this,ID_TIMEOUT);
  getApp()->removeTimeout(this,ID_SEEKRATETIMEOUT);

  for(i=0;i<n;i++)
  {
    devnam=(FXString*)bandtitle->getItemData(i);
    delete devnam;
  }

  delete font;

  delete helpmenu;
  delete repeatmenu;
  delete timemenu;
  delete optionsmenu;
  delete viewmenu;
  delete filemenu;
  delete popupmenu;

  for(iter=disbmp.begin();iter!=disbmp.end();++iter)
    delete (*iter);

  for(iter=lcdbmp.begin();iter!=lcdbmp.end();++iter)
    delete (*iter);

  for(iter=btnbmp.begin();iter!=btnbmp.end();++iter)
    delete (*iter);

  for(iter=mutebmp.begin();iter!=mutebmp.end();++iter)
    delete (*iter);

  for(task=tasklist.begin();task!=tasklist.end();++task)
  {
    (*task)->cancel();
    delete (*task)->getInfo()->getGUISignal();
    delete (*task)->getInfo();
    delete (*task);    
  }
}
