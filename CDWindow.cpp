/* CDWindow.cpp
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
#include "CDSeekButton.h"
#include "CDPreferences.h"
#include "CDBMPIcon.h"
#include "CDWindow.h"
#include "icons.h"

FXDEFMAP(CDWindow) CDWindowMap[]={
  FXMAPFUNC(SEL_CLOSE,0,CDWindow::onCmdQuit),
  FXMAPFUNC(SEL_SIGNAL,CDWindow::ID_QUIT,CDWindow::onCmdQuit),
  FXMAPFUNC(SEL_PAINT,CDWindow::ID_CANVAS,CDWindow::onPaint),
  FXMAPFUNC(SEL_LEFTBUTTONPRESS,CDWindow::ID_CANVAS,CDWindow::onMouseDown),
  FXMAPFUNC(SEL_TIMEOUT,CDWindow::ID_TIMEOUT,CDWindow::onTimeout),

  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_QUIT,CDWindow::onCmdQuit),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_STATUSDISC,CDWindow::onUpdStatusDisc),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_STATUSTRACK,CDWindow::onUpdStatusTrack),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_INTRO,CDWindow::onCmdIntro),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_INTRO,CDWindow::onUpdIntro),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_INTROTIME,CDWindow::onCmdIntroTime),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_INTROTIME,CDWindow::onUpdIntroTime),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_RANDOM,CDWindow::onCmdRandom),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_RANDOM,CDWindow::onUpdRandom),
  FXMAPFUNCS(SEL_COMMAND,CDWindow::ID_REPEATNONESWITCH,CDWindow::ID_REPEATDISCSWITCH,CDWindow::onCmdRepeatSwitch),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_REPEATSWITCH,CDWindow::onUpdRepeatSwitch),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_PREFS,CDWindow::onCmdPrefs),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_STOPONEXIT,CDWindow::onCmdStopOnExit),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_STOPONEXIT,CDWindow::onUpdStopOnExit),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_REMOTEINFO,CDWindow::onCmdRemoteInfo),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_REMOTEINFO,CDWindow::onUpdRemoteInfo),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_LOCALFIRST,CDWindow::onCmdLocalFirst),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_LOCALFIRST,CDWindow::onUpdLocalFirst),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_PROXYSERVER,CDWindow::onCmdProxyServer),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_PROXYSERVER,CDWindow::onUpdProxyServer),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_CDDB,CDWindow::onCmdCDDB),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_CDDB,CDWindow::onUpdCDDB),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_CDDBPORT,CDWindow::onCmdCDDBPort),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_CDDBPORT,CDWindow::onUpdCDDBPort),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_GETINFO,CDWindow::onCmdGetInfo),
  FXMAPFUNCS(SEL_COMMAND,CDWindow::ID_COLORFORE,CDWindow::ID_COLORICONS,CDWindow::onCmdColor),
  FXMAPFUNCS(SEL_CHANGED,CDWindow::ID_COLORFORE,CDWindow::ID_COLORICONS,CDWindow::onCmdColor),
  FXMAPFUNCS(SEL_UPDATE,CDWindow::ID_COLORFORE,CDWindow::ID_COLORICONS,CDWindow::onUpdColor),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_FONT,CDWindow::onCmdFont),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_CDROMADD,CDWindow::onCmdCDROMAdd),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_CDROMREM,CDWindow::onCmdCDROMRemove),

  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_TOGGLEMENU,CDWindow::onCmdToggleMenu),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_TOGGLEMENU,CDWindow::onUpdToggleMenu),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_TOGGLESTATUS,CDWindow::onCmdToggleStatus),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_TOGGLESTATUS,CDWindow::onUpdToggleStatus),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_TOGGLETIPS,CDWindow::onCmdToggleTips),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_TOGGLETIPS,CDWindow::onUpdToggleTips),

  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_DEFAULTOPTIONS,CDWindow::onCmdDefaultOptions),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_DEFAULTAPPEARANCE,CDWindow::onCmdDefaultAppearance),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_DEFAULTINTERNET,CDWindow::onCmdDefaultInternet),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_DEFAULTHARDWARE,CDWindow::onCmdDefaultHardware),

  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_BAND,CDWindow::onCmdBand),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_TRACK,CDWindow::onCmdTrack),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_TRACK,CDWindow::onUpdTrack),

  FXMAPFUNC(SEL_CHANGED,CDWindow::ID_VOLUME,CDWindow::onCmdVolume),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_VOLUME,CDWindow::onCmdVolume),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_VOLUME,CDWindow::onUpdVolume),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_MUTE,CDWindow::onCmdMute),
  FXMAPFUNC(SEL_CHANGED,CDWindow::ID_BALANCE,CDWindow::onCmdBalance),
  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_BALANCE,CDWindow::onCmdBalance),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_BALANCE,CDWindow::onUpdBalance),

  FXMAPFUNCS(SEL_LEFTBUTTONPRESS,CDWindow::ID_SEEKREVERSE,CDWindow::ID_SEEKFORWARD,CDWindow::onActivateSeeker),
  FXMAPFUNCS(SEL_LEFTBUTTONRELEASE,CDWindow::ID_SEEKREVERSE,CDWindow::ID_SEEKFORWARD,CDWindow::onDeactivateSeeker),
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

  FXMAPFUNC(SEL_COMMAND,CDWindow::ID_EJECT,CDWindow::onCmdEject),
  FXMAPFUNC(SEL_UPDATE,CDWindow::ID_EJECT,CDWindow::onUpdEject)
};

FXIMPLEMENT(CDWindow,FXMainWindow,CDWindowMap,ARRAYNUMBER(CDWindowMap))

#define TIMED_UPDATE       200
#define NODISC_MSG         "Data or no disc loaded"
#define DEFAULTFORE       FXRGB(0,160,255)
#define DEFAULTBACK       FXRGB(0,0,0)

CDWindow::CDWindow(FXApp* app)
: FXMainWindow(app,"fxcd",NULL,NULL,DECOR_TITLE|DECOR_MINIMIZE|DECOR_CLOSE|DECOR_BORDER|DECOR_MENU,0,0,0,0, 0,0),
  timer(NULL),
  tooltip(NULL),
  font(NULL),
  lcdforecolor(FXRGB(255,255,255)),
  lcdbackcolor(FXRGB(0,0,0)),
  iconcolor(FXRGB(0,0,0)),
  remoteInfo(FALSE),
  localFirst(TRUE),
  useCDDB(TRUE),
  stopOnExit(TRUE),
  startMode(CDSTART_NONE),
  timeMode(CDTIME_TRACK),
  volLevel(-1),
  volBalance(-1.0)
{
  muteIcon=new CDBMPIcon(getApp(),mute_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  nomuteIcon=new CDBMPIcon(getApp(),nomute_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  introd=new FXBMPIcon(getApp(),introd_bmp,0,IMAGE_ALPHAGUESS);
  randd=new FXBMPIcon(getApp(),randd_bmp,0,IMAGE_ALPHAGUESS);

  bicolor[0]=new CDBMPIcon(getApp(),reverse_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  bicolor[1]=new CDBMPIcon(getApp(),forward_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  bicolor[2]=new CDBMPIcon(getApp(),stop_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  bicolor[3]=new CDBMPIcon(getApp(),play_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  bicolor[4]=new CDBMPIcon(getApp(),pause_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  bicolor[5]=new CDBMPIcon(getApp(),prev_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  bicolor[6]=new CDBMPIcon(getApp(),next_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  bicolor[7]=new CDBMPIcon(getApp(),eject_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);

  dcifore[0]=new CDBMPIcon(getApp(),zero_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[1]=new CDBMPIcon(getApp(),one_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[2]=new CDBMPIcon(getApp(),two_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[3]=new CDBMPIcon(getApp(),three_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[4]=new CDBMPIcon(getApp(),four_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[5]=new CDBMPIcon(getApp(),five_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[6]=new CDBMPIcon(getApp(),six_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[7]=new CDBMPIcon(getApp(),seven_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[8]=new CDBMPIcon(getApp(),eight_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[9]=new CDBMPIcon(getApp(),nine_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[10]=new CDBMPIcon(getApp(),dash_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[11]=new CDBMPIcon(getApp(),colon_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[12]=new CDBMPIcon(getApp(),intro_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[13]=new CDBMPIcon(getApp(),rand_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[14]=new CDBMPIcon(getApp(),repeatn_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[15]=new CDBMPIcon(getApp(),repeatt_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[16]=new CDBMPIcon(getApp(),repeatd_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);
  dcifore[17]=new CDBMPIcon(getApp(),prefs_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP);

  timeTarget=new FXDataTarget(timeMode);
  repeatTarget=new FXDataTarget(cdplayer.repeatMode);
  startTarget=new FXDataTarget(startMode);
  proxyAddrTarget=new FXDataTarget(cdinfo.proxyaddr);
  proxyPortTarget=new FXDataTarget(cdinfo.proxyport);
  cddbProtoTarget=new FXDataTarget(cdinfo.cddbproto);
  cddbAddrTarget=new FXDataTarget(cdinfo.cddbaddr);

  menubar=new FXMenubar(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X);
  filemenu=new FXMenuPane(this);
    new FXMenuTitle(menubar,"&File",NULL,filemenu);
    new FXMenuCommand(filemenu,"&Quit\tCtrl+Q",NULL,this,ID_QUIT);
  optionsmenu=new FXMenuPane(this);
    new FXMenuTitle(menubar,"&Options",NULL,optionsmenu);
    new FXMenuCommand(optionsmenu,"&Intro",NULL,this,ID_INTRO);
    new FXMenuCommand(optionsmenu,"&Shuffle",NULL,this,ID_RANDOM);
    new FXMenuSeparator(optionsmenu);
    timemenu=new FXMenuPane(this);
      new FXMenuCascade(optionsmenu,"&Time Mode",NULL,timemenu);
      new FXMenuCommand(timemenu,"&Album",NULL,timeTarget,FXDataTarget::ID_OPTION+CDTIME_DISC);
      new FXMenuCommand(timemenu,"&Track",NULL,timeTarget,FXDataTarget::ID_OPTION+CDTIME_TRACK);
      new FXMenuCommand(timemenu,"Albu&m Remaining",NULL,timeTarget,FXDataTarget::ID_OPTION+CDTIME_DISCREM);
      new FXMenuCommand(timemenu,"Trac&k Remaining",NULL,timeTarget,FXDataTarget::ID_OPTION+CDTIME_TRACKREM);
    repeatmenu=new FXMenuPane(this);
      new FXMenuCascade(optionsmenu,"&Repeat Mode",NULL,repeatmenu);
      new FXMenuCommand(repeatmenu,"&None",NULL,repeatTarget,FXDataTarget::ID_OPTION+CDREPEAT_NONE);
      new FXMenuCommand(repeatmenu,"&Track",NULL,repeatTarget,FXDataTarget::ID_OPTION+CDREPEAT_TRACK);
      new FXMenuCommand(repeatmenu,"&Album",NULL,repeatTarget,FXDataTarget::ID_OPTION+CDREPEAT_DISC);
    new FXMenuSeparator(optionsmenu);
    new FXMenuCommand(optionsmenu,"Stop Play on &Exit",NULL,this,ID_STOPONEXIT);
    new FXMenuSeparator(optionsmenu);
    new FXMenuCommand(optionsmenu,"&Preferences...\tCtrl+P",NULL,this,ID_PREFS);
  infomenu=new FXMenuPane(this);
    new FXMenuTitle(menubar,"&Internet",NULL,infomenu);
    new FXMenuCommand(infomenu,"Use &Remote Data Source",NULL,this,ID_REMOTEINFO);
    new FXMenuCommand(infomenu,"Check &Local Source First",NULL,this,ID_LOCALFIRST);
    new FXMenuSeparator(infomenu);
    new FXMenuCommand(infomenu,"Use &CDDB",NULL,this,ID_CDDB);
    new FXMenuSeparator(infomenu);
    new FXMenuCommand(infomenu,"&Update Now",NULL,this,ID_GETINFO);

  statusbar=new FXStatusbar(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);
  statusbar->getStatusline()->setNormalText("");
  FXLabel* label=new FXLabel(statusbar,"00:00",NULL,FRAME_SUNKEN|LAYOUT_RIGHT);
  label->setTipText("Album Length");
  label->setTarget(this);
  label->setSelector(ID_STATUSDISC);
  label=new FXLabel(statusbar,"00:00",NULL,FRAME_SUNKEN|LAYOUT_RIGHT);
  label->setTipText("Track Length");
  label->setTarget(this);
  label->setSelector(ID_STATUSTRACK);
  
  FXVerticalFrame* contents=new FXVerticalFrame(this,LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXHorizontalFrame* display=new FXHorizontalFrame(contents,FRAME_THICK|FRAME_SUNKEN|LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0, 0,0);
  FXVerticalFrame* canvasItems=new FXVerticalFrame(display,LAYOUT_FIX_WIDTH|LAYOUT_FILL_Y,0,0,114,0, 0,0,0,0, 0,0);
  FXVerticalFrame* canvasFrame=new FXVerticalFrame(canvasItems,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0, 0,0);
  canvas=new FXCanvas(canvasFrame,this,ID_CANVAS,LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  FXHorizontalFrame* displayButtons=new FXHorizontalFrame(canvasItems,LAYOUT_FILL_X);
  dcwback[0]=new FXToggleButton(displayButtons,"","",introd,dcifore[12],this,ID_INTRO,0, 0,0,0,0, 0,0,0,0);
  dcwback[1]=new FXToggleButton(displayButtons,"","",randd,dcifore[13],this,ID_RANDOM,0, 0,0,0,0, 0,0,0,0);
  FXSwitcher* switcher=new FXSwitcher(displayButtons,0, 0,0,0,0, 0,0,0,0);
  switcher->setTarget(this);
  switcher->setSelector(ID_REPEATSWITCH);
  dcwback[2]=new FXButton(switcher,"",dcifore[14],this,ID_REPEATNONESWITCH,0, 0,0,0,0, 0,0,0,0);
  dcwback[3]=new FXButton(switcher,"",dcifore[15],this,ID_REPEATTRACKSWITCH,0, 0,0,0,0, 0,0,0,0);
  dcwback[4]=new FXButton(switcher,"",dcifore[16],this,ID_REPEATDISCSWITCH,0, 0,0,0,0, 0,0,0,0);
  dcwback[5]=new FXButton(displayButtons,"",dcifore[17],this,ID_PREFS,0, 0,0,0,0, 0,0,0,0);
  dcwback[6]=displayButtons;

  FXVerticalFrame* textfields=new FXVerticalFrame(display,LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_RIGHT,0,0,0,0, 0,0,0,0, 0,0);
  textfields->setBackColor(DEFAULTBACK);
  bandTitle=new FXListBox(textfields,2,this,ID_BAND,LAYOUT_FILL_X);
  albumTitle=new FXLabel(textfields,NODISC_MSG,NULL,JUSTIFY_LEFT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  trackTitle=new FXListBox(textfields,8,this,ID_TRACK,LAYOUT_FILL_X);

  new FXHorizontalSeparator(contents,SEPARATOR_GROOVE|LAYOUT_FILL_X);
  FXHorizontalFrame* mixer=new FXHorizontalFrame(contents,PACK_UNIFORM_WIDTH|LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  FXHorizontalFrame* volume=new FXHorizontalFrame(mixer,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  new FXLabel(volume,"Volume: ",NULL,JUSTIFY_LEFT|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  FXSlider* volumeSlider=new FXSlider(volume,this,ID_VOLUME,SLIDER_HORIZONTAL|LAYOUT_FILL_X|LAYOUT_CENTER_Y);
  FXHorizontalFrame* balanceFrame=new FXHorizontalFrame(mixer,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  new FXToggleButton(balanceFrame,"\tMute audio","\tAllow audio",nomuteIcon,muteIcon,this,ID_MUTE,TOGGLEBUTTON_TOOLBAR|FRAME_RAISED,0,0,0,0, 2,2,1,1);
  new FXFrame(balanceFrame,LAYOUT_FIX_WIDTH,0,0,2);
  new FXLabel(balanceFrame,"Balance: ",NULL,JUSTIFY_LEFT|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  FXSlider* balanceSlider=new FXSlider(balanceFrame,this,ID_BALANCE,SLIDER_HORIZONTAL|LAYOUT_FILL_X|LAYOUT_CENTER_Y);
  volumeSlider->setRange(0,100);
  volumeSlider->setIncrement(1);
  balanceSlider->setRange(0,200);
  balanceSlider->setIncrement(2);

  new FXHorizontalSeparator(contents,SEPARATOR_GROOVE|LAYOUT_FILL_X);
  FXHorizontalFrame* buttons=new FXHorizontalFrame(contents,LAYOUT_FILL_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0, 0,0);
  new CDSeekButton(buttons,"\tSkip backword",bicolor[0],this,ID_SEEKREVERSE,FRAME_THICK|FRAME_RAISED,0,0,0,0, 2,2,1,1);
  new CDSeekButton(buttons,"\tSkip forward",bicolor[1],this,ID_SEEKFORWARD,FRAME_THICK|FRAME_RAISED,0,0,0,0, 2,2,1,1);
  new FXFrame(buttons,LAYOUT_FIX_WIDTH,0,0,4);
  new FXButton(buttons,"\tStop play",bicolor[2],this,ID_STOP,FRAME_THICK|FRAME_RAISED,0,0,0,0, 2,2,1,1);
  new FXToggleButton(buttons,"\tPlay track","\tPause track",bicolor[3],bicolor[4],this,ID_PLAY,FRAME_THICK|FRAME_RAISED,0,0,0,0, 2,2,1,1);
  new FXFrame(buttons,LAYOUT_FIX_WIDTH,0,0,4);
  new FXButton(buttons,"\tPrevious track",bicolor[5],this,ID_PREV,FRAME_THICK|FRAME_RAISED,0,0,0,0, 2,2,1,1);
  new FXButton(buttons,"\tNext track",bicolor[6],this,ID_NEXT,FRAME_THICK|FRAME_RAISED,0,0,0,0, 2,2,1,1);
  new FXButton(buttons,"\tEject",bicolor[7],this,ID_EJECT,FRAME_THICK|FRAME_RAISED|LAYOUT_RIGHT,0,0,0,0, 2,2,1,1);

  cdprefs=new CDPreferences(this);
}

CDWindow::~CDWindow()
{
  FXint i,n=bandTitle->getNumItems();
  FXString* devnam=NULL;

  for(i=0;i<n;i++)
  {
    devnam=(FXString*)bandTitle->getItemData(i);
    delete devnam;
  }

  delete timeTarget;
  delete repeatTarget;
  delete startTarget;
  delete proxyAddrTarget;
  delete proxyPortTarget;
  delete cddbProtoTarget;
  delete cddbAddrTarget;
  delete cdindexAddrTarget;
  delete cdindexPortTarget;
  delete font;

  delete timemenu;
  delete repeatmenu;
  delete optionsmenu;
  delete filemenu;

  for(i=17;i>=0;i--)
    delete dcifore[i];

  for(i=7;i>=0;i--)
    delete bicolor[i];

  delete randd;
  delete introd;
  delete nomuteIcon;
  delete muteIcon;
}

void CDWindow::create()
{
  FXint i;
  const FXchar* fontspec;
  FXFontDesc fontdesc;

  FXint x=getApp()->reg().readIntEntry("SETTINGS","x",10);
  FXint y=getApp()->reg().readIntEntry("SETTINGS","y",10);
  cdplayer.setIntro(getApp()->reg().readIntEntry("SETTINGS","intro",cdplayer.getIntro()));
  cdplayer.setIntroLength(getApp()->reg().readUnsignedEntry("SETTINGS","introTime",cdplayer.getIntroLength()));
  cdplayer.setRandom(getApp()->reg().readIntEntry("SETTINGS","random",cdplayer.getRandom()));
  cdplayer.setRepeatMode(getApp()->reg().readUnsignedEntry("SETTINGS","repeatMode",cdplayer.getRepeatMode()));
  timeMode=getApp()->reg().readUnsignedEntry("SETTINGS","timeMode",timeMode);
  stopOnExit=getApp()->reg().readIntEntry("SETTINGS","stopOnExit",stopOnExit);
  startMode=getApp()->reg().readIntEntry("SETTINGS","startAction",startMode);
  remoteInfo=getApp()->reg().readIntEntry("SETTINGS","remoteInfo",remoteInfo);
  localFirst=getApp()->reg().readIntEntry("SETTINGS","localFirst",localFirst);
  useCDDB=getApp()->reg().readIntEntry("SETTINGS","useCDDB",useCDDB);
  cdinfo.setProxy(getApp()->reg().readIntEntry("SETTINGS","proxy",cdinfo.getProxy()));
  cdinfo.setProxyAddress(getApp()->reg().readStringEntry("SETTINGS","proxyAddr",cdinfo.getProxyAddress().text()));
  cdinfo.setProxyPort(getApp()->reg().readUnsignedEntry("SETTINGS","proxyPort",cdinfo.getProxyPort()));
  cdinfo.setCDDBProtocol(getApp()->reg().readUnsignedEntry("SETTINGS","cddbProto",cdinfo.getCDDBProtocol()));
  cdinfo.setCDDBAddress(getApp()->reg().readStringEntry("SETTINGS","cddbAddr",cdinfo.getCDDBAddress().text()));
  cdinfo.cddbpport=getApp()->reg().readUnsignedEntry("SETTINGS","cddbpPort",cdinfo.cddbpport);
  cdinfo.cddbport=getApp()->reg().readUnsignedEntry("SETTINGS","cddbPort",cdinfo.cddbport);
  cdinfo.setCDDBScript(getApp()->reg().readStringEntry("SETTINGS","cddbScript",cdinfo.getCDDBScript().text()));
  if(!getApp()->reg().readIntEntry("SETTINGS","showMenu",TRUE)) menubar->hide();
  if(!getApp()->reg().readIntEntry("SETTINGS","showStatus",TRUE)) statusbar->hide();
  if(getApp()->reg().readIntEntry("SETTINGS","showTips",TRUE)) tooltip=new FXTooltip(getApp(),TOOLTIP_NORMAL);
  FXColor dfg=getApp()->reg().readColorEntry("SETTINGS","lcdforecolor",DEFAULTFORE);
  FXColor dbg=getApp()->reg().readColorEntry("SETTINGS","lcdbackcolor",DEFAULTBACK);
  FXColor ic=getApp()->reg().readColorEntry("SETTINGS","iconcolor",DEFAULTBACK);
  fontspec=getApp()->reg().readStringEntry("SETTINGS","font",NULL);
  if(fontspec&&fxparsefontdesc(fontdesc,fontspec))
  {
    font=new FXFont(getApp(),fontdesc);
    setDisplayFont(font);
  }

  FXMainWindow::create();
  for(i=0;i<12;i++)
    dcifore[i]->create();

  setDisplayForeground(dfg);
  setDisplayBackground(dbg);
  setIconColor(ic);

  move(x,y);
  resize(290,getDefaultHeight());

  checkDevices();
  handle(this,MKUINT(ID_BAND,SEL_COMMAND),(void*)bandTitle->getCurrentItem());

  timer=getApp()->addTimeout(TIMED_UPDATE,this,ID_TIMEOUT);

  //Start action
  if((startMode==CDSTART_STOP)&&(cdplayer.getStatus()==CDLYTE_PLAYING||cdplayer.getStatus()==CDLYTE_PAUSED))
    cdplayer.stop();
  else if((startMode==CDSTART_START)&&(cdplayer.getStatus()==CDLYTE_PAUSED))
    cdplayer.resume();
  else if((startMode==CDSTART_START)&&(cdplayer.getStatus()!=CDLYTE_PLAYING))
    cdplayer.play();
}

FXbool CDWindow::checkDevices()
{
  FXint i,player=0,total=0;
  FXbool stored=FALSE;
  FXString* devname=NULL;

  //Look in registry for CD-ROMs
  if(getApp()->reg().existingSection("DEVICES"))
  {
    FXint n;
    FXString entnam;

    stored=TRUE;
    n=getApp()->reg().readIntEntry("DEVICES","total",0);
    for(i=0;i<n;i++)
    {
      entnam.format("device%d",i);
      const FXchar* entstr=getApp()->reg().readStringEntry("DEVICES",entnam.text(),NULL);
      if(entstr!=NULL)
      {
	total++;
	devname=new FXString(entstr);
        bandTitle->appendItem(*devname,NULL,(void*)devname);
      }
    }    
  }

  if(!stored||total<=0)
  {
#ifndef WIN32
    //On Linux and *BSD we could use SDL code to check for devices.  
    //Once I have configure for this app I'll try to add that.  
    devname=new FXString("/dev/cdrom");
    bandTitle->appendItem(*devname,NULL,(void*)devname);
    total=1;
#else
    FXchar drive[4];
    for(i='A';i<='Z';i++)
    {
      sprintf(drive,"%c:\\",i);
      if(GetDriveType(drive)==DRIVE_CDROM)
      {
        total++;
        devname=new FXString;
        devname->format("%c:",i);
        bandTitle->appendItem(*devname,NULL,(void*)devname);
      }
    }
#endif
  }

  if(!stored)
  {
    //Commit them to registry
    FXint n=bandTitle->getNumItems();
    FXString* entstr;
    FXString entnam;

    getApp()->reg().writeIntEntry("DEVICES","total",n);
    for(i=0;i<total;i++)
    {
      entnam.format("device%d",i);
      entstr=(FXString*)bandTitle->getItemData(i);
      getApp()->reg().writeStringEntry("DEVICES",entnam.text(),entstr->text());
    }

    //Make sure it is committed now
    getApp()->reg().write();
  }

  //See if a device is currently active
  for(i=0;i<bandTitle->getNumItems();i++)
  {
    devname=(FXString*)bandTitle->getItemData(i);
    if(cdplayer.init(devname->text()))
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

  bandTitle->setCurrentItem(player);
  bandTitle->setNumVisible(total);

  return true;
}

FXbool CDWindow::loadDiscData()
{
  FXint band=bandTitle->getCurrentItem();
  FXString* deviceStr=(FXString*)bandTitle->getItemData(band);
  FXString title;

  if(!cdplayer.isValid()||!cdplayer.isDiscPresent()||!cdplayer.isAudioDisc())
  {
      //Relabel for empty media
      title.format("%s - <%s>",NODISC_MSG,deviceStr->text());
      bandTitle->setItemText(band,title);
      bandTitle->setTipText(title);
      albumTitle->setText(NODISC_MSG);
      albumTitle->setTipText(NODISC_MSG);
      trackTitle->clearItems();
      trackTitle->appendItem(NODISC_MSG);
      trackTitle->setTipText(NODISC_MSG);
      trackTitle->setCurrentItem(0);
      trackTitle->setNumVisible(1);
  }
  else
  {
    FXint i;
    FXString title;
    struct disc_data data;

    getData(&data);
    title.format("%s - <%s>",data.data_artist,deviceStr->text());
    bandTitle->setItemText(band,title);
    bandTitle->setTipText(title);
    albumTitle->setText(data.data_title);
    albumTitle->setTipText(data.data_title);

    //Load all tracks
    trackTitle->clearItems();
    for(i=cdplayer.getStartTrack();i<=cdplayer.getNumTracks();i++)
    {
      const struct track_info* track=cdplayer.getTrackInfo(i-1);
      if(track->track_type==CDLYTE_TRACK_AUDIO)
      {
        title.format("%d. %s (%d:%02d)",i,data.data_track[i-1].track_title,track->track_length.minutes,track->track_length.seconds);
        trackTitle->appendItem(title);
      }
    }

    //Size our drop down list - no empty list spaces
    trackTitle->setNumVisible((cdplayer.getNumTracks()>8)?8:cdplayer.getNumTracks());
  }

  return TRUE;
}

FXbool CDWindow::getData(struct disc_data* data)
{
  if(!remoteInfo) return cdinfo.getLocalCDDBInfo(cdplayer,data);
  if(localFirst&&cdinfo.getLocalCDDBInfo(cdplayer,data)) return TRUE;
  if(useCDDB&&cdinfo.getRemoteCDDBInfo(cdplayer,data,this)) return TRUE;
  if(!localFirst&&cdinfo.getLocalCDDBInfo(cdplayer,data)) return TRUE;
  cdinfo.defaultSettings(cdplayer,data);
  return FALSE;
}

void CDWindow::doDraw(FXint track,const struct disc_info* di)
{
  FXbool negative=FALSE;
  FXString text;
  FXImage back(getApp(),NULL,IMAGE_OPAQUE,canvas->getWidth(),canvas->getHeight());
  back.create();
  FXDCWindow dc(&back);

  dc.setForeground(lcdbackcolor);
  dc.fillRectangle(0,0,canvas->getWidth(),canvas->getHeight());

  struct disc_timeval drawTime;

  if(di->disc_mode==CDLYTE_PLAYING||di->disc_mode==CDLYTE_PAUSED)
  {
    if((di->disc_mode==CDLYTE_PAUSED)&&cdplayer.blink())
    {
      text.format("%02d--:--",track);
    }
    else
    {
      //Format time for proper output
      if(timeMode==CDTIME_DISC)
      {
        drawTime.minutes=di->disc_time.minutes;
        drawTime.seconds=di->disc_time.seconds;
      }
      else if(timeMode==CDTIME_TRACK)
      {
        drawTime.minutes=di->disc_track_time.minutes;
        drawTime.seconds=di->disc_track_time.seconds;
      }
      else if(timeMode==CDTIME_DISCREM)
      {
        FXlong pos=(di->disc_time.minutes*60)+di->disc_time.seconds;
        FXlong len=(di->disc_length.minutes*60)+di->disc_length.seconds;
        pos=len-pos;
        drawTime.minutes=pos/60;
        drawTime.seconds=pos%60;
        negative=TRUE;
      }
      else if(timeMode==CDTIME_TRACKREM)
      {
        FXlong pos=(di->disc_track_time.minutes*60)+di->disc_track_time.seconds;
        FXlong len=(di->disc_track[track-1].track_length.minutes*60)+di->disc_track[track-1].track_length.seconds;
        pos=len-pos;
        drawTime.minutes=pos/60;
        drawTime.seconds=pos%60;
        negative=TRUE;
      }
      text.format("%02d%02d:%02d",track,drawTime.minutes,drawTime.seconds);
    }
  }
  else
  {
    text.format("%02d00:00",track);
  }

  FXint x=DEFAULT_SPACING,y=DEFAULT_SPACING*3,p=0;
  FXIcon* drawMe=NULL;
  dc.setForeground(lcdforecolor);

  while(text[p]!='\0')
  {
    if(p==2)
    {
      x+=DEFAULT_PAD;
      if(negative) dc.drawIcon(dcifore[10],x,y);
      x+=dcifore[10]->getWidth();
    }

    drawMe=(text[p]=='-')?dcifore[10]:dcifore[text[p]-'0'];
    dc.drawIcon(drawMe,x,y);
    x+=dcifore[0]->getWidth();
    p++;
    if(text[p]==':')
    {
      dc.drawIcon(dcifore[11],x,y);
      x+=dcifore[11]->getWidth();
      p++;
    }
  }

  FXDCWindow sdc(canvas);
  sdc.drawImage(&back,0,0);
}

void CDWindow::setDisplayFont(FXFont* font)
{
  bandTitle->setFont(font);
  trackTitle->setFont(font);
  albumTitle->setFont(font);
  resize(290,getDefaultHeight());
}

FXFont* CDWindow::getDisplayFont() const
{
  return albumTitle->getFont();
}

void CDWindow::setDisplayForeground(FXColor color)
{
  FXint i;

  //Needed for dual color icons.  If they are equal, modify slightly.  
  if(color==lcdbackcolor)
  {
    FXuchar r=FXREDVAL(color),g=FXGREENVAL(color),b=FXBLUEVAL(color);
    color=FXRGB((r<255)?r+1:r-1,(g<255)?g+1:g-1,(b<255)?b+1:b-1);
  }

  //Fonts
  bandTitle->setTextColor(color);
  albumTitle->setTextColor(color);
  trackTitle->setTextColor(color);

  //Icons
  for(i=0;i<18;i++)
    dcifore[i]->swapColor(lcdforecolor,color);

  muteIcon->swapColor(lcdforecolor,color);
  nomuteIcon->swapColor(lcdforecolor,color);

  lcdforecolor=color;
}

FXColor CDWindow::getDisplayForeground() const
{
  return lcdforecolor;
}

void CDWindow::setDisplayBackground(FXColor color)
{
  FXint i;

  //Needed for dual color icons.  If they are equal, modify slightly.  
  if(color==lcdforecolor)
  {
    FXuchar r=FXREDVAL(color),g=FXGREENVAL(color),b=FXBLUEVAL(color);
    color=FXRGB((r<255)?r+1:r-1,(g<255)?g+1:g-1,(b<255)?b+1:b-1);
  }

  //Fonts
  bandTitle->setBackColor(color);
  albumTitle->setBackColor(color);
  trackTitle->setBackColor(color);

  //Widgets
  for(i=0;i<7;i++)
    dcwback[i]->setBackColor(color);

  muteIcon->swapColor(lcdbackcolor,color);
  nomuteIcon->swapColor(lcdbackcolor,color);

  lcdbackcolor=color;
}

FXColor CDWindow::getDisplayBackground() const
{
  return lcdbackcolor;
}

void CDWindow::setIconColor(FXColor color)
{
  FXint i;
  for(i=0;i<8;i++)
    bicolor[i]->swapColor(iconcolor,color);

  iconcolor=color;
}

FXColor CDWindow::getIconColor() const
{
  return iconcolor;
}

long CDWindow::onPaint(FXObject*,FXSelector,void*)
{
  doDraw(cdplayer.getCurrentTrack(),cdplayer.getDiscInfo());
  return 1;
}

long CDWindow::onMouseDown(FXObject*,FXSelector,void*)
{
  if(++timeMode>CDTIME_TRACKREM) timeMode=CDTIME_DISC;
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
      //Might want to rethink this type of check, performance wise
      if(albumTitle->getText()==NODISC_MSG)
        loadDiscData();
    }
    else
    {
      //No disc is present
      if(albumTitle->getText()!=NODISC_MSG)
        loadDiscData();
    }
  }  

  doDraw(cdplayer.getCurrentTrack(),cdplayer.getDiscInfo());

  //Reset timer
  timer=getApp()->addTimeout(TIMED_UPDATE,this,ID_TIMEOUT);

  return 1;
}

long CDWindow::onCmdQuit(FXObject*,FXSelector,void*)
{
  FXchar fontspec[200];
  FXFontDesc fontdesc;

  if(stopOnExit==TRUE)
  {
    if(cdplayer.getStatus()==CDLYTE_PLAYING||cdplayer.getStatus()==CDLYTE_PAUSED)
      cdplayer.stop();
    if(timer) timer=getApp()->removeTimeout(timer);
    cdplayer.finish();
  }

  getApp()->reg().writeIntEntry("SETTINGS","x",getX());
  getApp()->reg().writeIntEntry("SETTINGS","y",getY());
  getApp()->reg().writeIntEntry("SETTINGS","intro",cdplayer.getIntro());
  getApp()->reg().writeIntEntry("SETTINGS","introTime",cdplayer.getIntroLength());
  getApp()->reg().writeIntEntry("SETTINGS","random",cdplayer.getRandom());
  getApp()->reg().writeUnsignedEntry("SETTINGS","repeatMode",cdplayer.getRepeatMode());
  getApp()->reg().writeUnsignedEntry("SETTINGS","timeMode",timeMode);
  getApp()->reg().writeIntEntry("SETTINGS","stopOnExit",stopOnExit);
  getApp()->reg().writeIntEntry("SETTINGS","startAction",startMode);
  getApp()->reg().writeIntEntry("SETTINGS","remoteInfo",remoteInfo);
  getApp()->reg().writeIntEntry("SETTINGS","localFirst",localFirst);
  getApp()->reg().writeIntEntry("SETTINGS","useCDDB",useCDDB);
  getApp()->reg().writeIntEntry("SETTINGS","proxy",cdinfo.getProxy());
  getApp()->reg().writeStringEntry("SETTINGS","proxyAddr",cdinfo.getProxyAddress().text());
  getApp()->reg().writeUnsignedEntry("SETTINGS","proxyPort",cdinfo.getProxyPort());
  getApp()->reg().writeUnsignedEntry("SETTINGS","cddbProto",cdinfo.getCDDBProtocol());
  getApp()->reg().writeStringEntry("SETTINGS","cddbAddr",cdinfo.getCDDBAddress().text());
  getApp()->reg().writeUnsignedEntry("SETTINGS","cddbpPort",cdinfo.cddbpport);
  getApp()->reg().writeUnsignedEntry("SETTINGS","cddbPort",cdinfo.cddbport);
  getApp()->reg().writeStringEntry("SETTINGS","cddbScript",cdinfo.getCDDBScript().text());
  getApp()->reg().writeIntEntry("SETTINGS","showMenu",menubar->shown());
  getApp()->reg().writeIntEntry("SETTINGS","showStatus",statusbar->shown());
  getApp()->reg().writeIntEntry("SETTINGS","showTips",tooltip!=NULL);
  getApp()->reg().writeColorEntry("SETTINGS","lcdforecolor",lcdforecolor);
  getApp()->reg().writeColorEntry("SETTINGS","lcdbackcolor",lcdbackcolor);
  getApp()->reg().writeColorEntry("SETTINGS","iconcolor",iconcolor);

  getDisplayFont()->getFontDesc(fontdesc);
  fxunparsefontdesc(fontspec,fontdesc);
  getApp()->reg().writeStringEntry("SETTINGS","font",fontspec);

  getApp()->reg().write();

  getApp()->stop();

  return 1;
}

long CDWindow::onUpdStatusDisc(FXObject* sender,FXSelector,void*)
{
  FXString str("00:00");
  if(cdplayer.isDiscPresent())
  {
    const struct disc_timeval* length=cdplayer.getDiscLength();
    str.format("%02d:%02d",length->minutes,length->seconds);
  }

  sender->handle(this,MKUINT(ID_SETSTRINGVALUE,SEL_COMMAND),(void*)&str);
  return 1;
}

long CDWindow::onUpdStatusTrack(FXObject* sender,FXSelector,void*)
{
  FXString str("00:00");
  //if(cdplayer.isDiscPresent()&&(cdplayer.getStatus()==CDLYTE_PLAYING||cdplayer.getStatus()==CDLYTE_PAUSED))
  if(cdplayer.isDiscPresent())
  {
    const struct track_info* track=cdplayer.getTrackInfo(cdplayer.getCurrentTrack()-1);
    str.format("%02d:%02d",track->track_length.minutes,track->track_length.seconds);
  }
  sender->handle(this,MKUINT(ID_SETSTRINGVALUE,SEL_COMMAND),(void*)&str);
  return 1;
}

long CDWindow::onCmdIntro(FXObject*,FXSelector,void*)
{
  cdplayer.setIntro(!cdplayer.getIntro());
  return 1;
}

long CDWindow::onUpdIntro(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(cdplayer.getIntro())?ID_CHECK:ID_UNCHECK;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdIntroTime(FXObject* sender,FXSelector,void*)
{
  FXint value;
  sender->handle(this,MKUINT(ID_GETINTVALUE,SEL_COMMAND),(void*)&value);
  cdplayer.setIntroLength(value);
  return 1;
}

long CDWindow::onUpdIntroTime(FXObject* sender,FXSelector,void*)
{
  FXint value=cdplayer.getIntroLength();
  sender->handle(this,MKUINT(ID_SETINTVALUE,SEL_COMMAND),(void*)&value);
  return 1;
}

long CDWindow::onCmdRandom(FXObject*,FXSelector,void*)
{
  cdplayer.setRandom(!cdplayer.getRandom());
  return 1;
}

long CDWindow::onUpdRandom(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(cdplayer.getRandom())?ID_CHECK:ID_UNCHECK;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

//Set repeat mode from switcher
long CDWindow::onCmdRepeatSwitch(FXObject*,FXSelector sel,void*)
{
  cdplayer.setRepeatMode((cdplayer.getRepeatMode()==CDREPEAT_DISC)?CDREPEAT_NONE:cdplayer.getRepeatMode()+1);
  return 1;
}

long CDWindow::onUpdRepeatSwitch(FXObject* sender,FXSelector,void*)
{
  FXuint msg=FXSwitcher::ID_OPEN_FIRST+cdplayer.getRepeatMode();
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdPrefs(FXObject*,FXSelector,void*)
{
  cdprefs->show(PLACEMENT_OWNER);
  return 1;
}

long CDWindow::onCmdStopOnExit(FXObject*,FXSelector,void*)
{
  stopOnExit=!stopOnExit;
  return 1;
}

long CDWindow::onUpdStopOnExit(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(stopOnExit)?ID_CHECK:ID_UNCHECK;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdRemoteInfo(FXObject*,FXSelector,void*)
{
  remoteInfo=!remoteInfo;
  return 1;
}

long CDWindow::onUpdRemoteInfo(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(remoteInfo)?ID_CHECK:ID_UNCHECK;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdLocalFirst(FXObject*,FXSelector,void*)
{
  localFirst=!localFirst;
  return 1;
}

long CDWindow::onUpdLocalFirst(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(localFirst)?ID_CHECK:ID_UNCHECK;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdProxyServer(FXObject*,FXSelector,void*)
{
  cdinfo.setProxy(!cdinfo.getProxy());
  return 1;
}

long CDWindow::onUpdProxyServer(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(cdinfo.getProxy())?ID_CHECK:ID_UNCHECK;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdCDDBPort(FXObject* sender,FXSelector,void*)
{
  FXint value;
  sender->handle(this,MKUINT(ID_GETINTVALUE,SEL_COMMAND),(void*)&value);
  cdinfo.setCDDBPort(value);
  return 1;
}

long CDWindow::onUpdCDDBPort(FXObject* sender,FXSelector,void*)
{
  FXint value=cdinfo.getCDDBPort();
  sender->handle(this,MKUINT(ID_SETINTVALUE,SEL_COMMAND),(void*)&value);
  return 1;
}

long CDWindow::onCmdGetInfo(FXObject*,FXSelector,void*)
{
  loadDiscData();
  return 1;
}

long CDWindow::onCmdColor(FXObject*,FXSelector sel,void* ptr)
{
  FXColor clr=(FXColor)(long)ptr;
  switch(SELID(sel))
  {
    case ID_COLORFORE: setDisplayForeground(clr); getApp()->forceRefresh(); break;
    case ID_COLORBACK: setDisplayBackground(clr); getApp()->forceRefresh(); break;
    case ID_COLORICONS: setIconColor(clr); getApp()->forceRefresh(); break;
  }
  return 1;
}

long CDWindow::onUpdColor(FXObject* sender,FXSelector sel,void*)
{
  FXColor clr;
  switch(SELID(sel))
  {
    case ID_COLORFORE: clr=getDisplayForeground(); break;
    case ID_COLORBACK: clr=getDisplayBackground(); break;
    case ID_COLORICONS: clr=getIconColor(); break;
  }
  sender->handle(this,MKUINT(ID_SETINTVALUE,SEL_COMMAND),(void*)&clr);
  return 1;
}

long CDWindow::onCmdFont(FXObject*,FXSelector,void*)
{
  FXFontDialog fontdlg(this,"Change Display Font",DECOR_BORDER|DECOR_TITLE);
  FXFontDesc fontdesc;
  getDisplayFont()->getFontDesc(fontdesc);
  fontdlg.setFontSelection(fontdesc);
  if(fontdlg.execute())
  {
    FXFont *oldfont=font;
    fontdlg.getFontSelection(fontdesc);
    font=new FXFont(getApp(),fontdesc);
    font->create();
    setDisplayFont(font);
    delete oldfont;
  }
  return 1;
}

long CDWindow::onCmdCDROMAdd(FXObject*,FXSelector,void* data)
{
  FXint n=getApp()->reg().readIntEntry("DEVICES","total",0);
  FXString* devnam=new FXString(*((FXString*)data));
  FXString entnam;

  bandTitle->appendItem(*devnam,NULL,(void*)devnam);
  bandTitle->setNumVisible(n+1);

  entnam.format("device%d",n);
  getApp()->reg().writeIntEntry("DEVICES","total",n+1);
  getApp()->reg().writeStringEntry("DEVICES",entnam.text(),devnam->text());

  getApp()->reg().write();

  return 1;
}

long CDWindow::onCmdCDROMRemove(FXObject*,FXSelector,void* data)
{
  FXint i,n=getApp()->reg().readIntEntry("DEVICES","total",0);
  FXint item=(FXint)data;
  FXString entnam;
  FXbool stopngo=(item==bandTitle->getCurrentItem())?TRUE:FALSE;

  if(item<bandTitle->getNumItems())
  {
    FXString* devnam=(FXString*)bandTitle->getItemData(item);
    bandTitle->removeItem(item);
    bandTitle->setNumVisible(n-1);
    delete devnam;

    getApp()->reg().writeIntEntry("DEVICES","total",n-1);
    for(i=item;i<(n-1);i++)
    {
      //Bump each item up
      entnam.format("device%d",i+1);
      const FXchar* entstr=getApp()->reg().readStringEntry("DEVICES",entnam.text(),NULL);
    
      entnam.format("device%d",i);
      getApp()->reg().writeStringEntry("DEVICES",entnam.text(),entstr);
    }
    entnam.format("device%d",n-1);
    getApp()->reg().deleteEntry("DEVICES",entnam.text());
    getApp()->reg().write();

    //We deleted the current device, so we must restart it
    if(stopngo)
      handle(this,MKUINT(ID_BAND,SEL_COMMAND),(void*)bandTitle->getCurrentItem());
  }

  return 1;
}

long CDWindow::onCmdToggleMenu(FXObject*,FXSelector,void*)
{
  FXuint msg=(menubar->shown())?ID_HIDE:ID_SHOW;
  menubar->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  resize(290,getDefaultHeight());
  return 1;
}

long CDWindow::onUpdToggleMenu(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(menubar->shown())?ID_CHECK:ID_UNCHECK;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdToggleStatus(FXObject*,FXSelector,void*)
{
  FXuint msg=(statusbar->shown())?ID_HIDE:ID_SHOW;
  statusbar->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  resize(290,getDefaultHeight());
  return 1;
}

long CDWindow::onUpdToggleStatus(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(statusbar->shown())?ID_CHECK:ID_UNCHECK;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdToggleTips(FXObject*,FXSelector,void*)
{
  if(tooltip)
  {
    delete tooltip;
    tooltip=NULL;
  }
  else
  {
    tooltip=new FXTooltip(getApp(),TOOLTIP_NORMAL);
    tooltip->create();
  }
  return 1;
}

long CDWindow::onUpdToggleTips(FXObject* sender,FXSelector,void*)
{
  FXuint msg=(tooltip!=NULL)?ID_CHECK:ID_UNCHECK;
  sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}

long CDWindow::onCmdDefaultOptions(FXObject*,FXSelector,void*)
{
  cdplayer.setIntro(FALSE);
  cdplayer.setIntroLength(10);
  cdplayer.setRandom(FALSE);
  cdplayer.setRepeatMode(CDREPEAT_NONE);
  timeMode=CDTIME_TRACK;
  startMode=CDSTART_NONE;
  stopOnExit=TRUE;

  return 1;
}

long CDWindow::onCmdDefaultAppearance(FXObject*,FXSelector,void*)
{
  setDisplayForeground(DEFAULTFORE);
  setDisplayBackground(DEFAULTBACK);
  setIconColor(DEFAULTBACK);
  setDisplayFont(getApp()->getNormalFont());
  menubar->handle(this,MKUINT(ID_SHOW,SEL_COMMAND),NULL);
  statusbar->handle(this,MKUINT(ID_SHOW,SEL_COMMAND),NULL);
  if(tooltip==NULL)
  {
    tooltip=new FXTooltip(getApp(),TOOLTIP_NORMAL);
    tooltip->create();
  }
  resize(290,getDefaultHeight());

  return 1;
}

long CDWindow::onCmdDefaultInternet(FXObject*,FXSelector,void*)
{
  remoteInfo=FALSE;
  localFirst=TRUE;
  useCDDB=TRUE;
  cdinfo.setProxy(FALSE);
  cdinfo.setProxyAddress("0.0.0.0");
  cdinfo.setProxyPort(0);
  cdinfo.setCDDBAddress("www.freedb.org");
  cdinfo.setCDDBProtocol(PROTO_CDDBP);
  cdinfo.setCDDBPort(CDDBP_DEFAULT_PORT);
  cdinfo.setCDDBProtocol(PROTO_HTTP);
  cdinfo.setCDDBPort(HTTP_DEFAULT_PORT);
  cdinfo.setCDDBScript("cgi-bin/cddb.cgi");

  return 1;
}

long CDWindow::onCmdDefaultHardware(FXObject*,FXSelector,void*)
{
  //clear and reload devices
  FXint i,n=bandTitle->getNumItems();
  FXString* devnam=NULL;

  for(i=0;i<n;i++)
  {
    devnam=(FXString*)bandTitle->getItemData(i);
    delete devnam;
  }
  bandTitle->clearItems();

  //Clear devices from registry
  getApp()->reg().deleteSection("DEVICES");
  getApp()->reg().write();

  //Regenerate
  if(cdplayer.isValid())
  {
    cdplayer.stop();
    cdplayer.finish();
  }

  checkDevices();
  handle(this,MKUINT(ID_BAND,SEL_COMMAND),(void*)bandTitle->getCurrentItem());

  return 1;
}

long CDWindow::onCmdBand(FXObject*,FXSelector,void* data)
{
  FXint band=(FXint)data;
  if(band>=0&&band<bandTitle->getNumItems())
  {
    FXString* devname=(FXString*)bandTitle->getItemData(band);

    if(cdplayer.isValid())
    {
      cdplayer.stop();
      cdplayer.finish();
    }

    cdplayer.init(devname->text());
    loadDiscData();
  }
  return 1;
}

long CDWindow::onCmdTrack(FXObject*,FXSelector,void* ptr)
{
  cdplayer.setCurrentTrack(((FXint)ptr)+1);
  if(cdplayer.getStatus()!=CDLYTE_PLAYING&&cdplayer.getStatus()!=CDLYTE_PAUSED)
    cdplayer.play();
  return 1;
}

long CDWindow::onUpdTrack(FXObject*,FXSelector,void* data)
{
  FXint track=(cdplayer.isValid()&&cdplayer.isDiscPresent()&&cdplayer.isAudioDisc())?cdplayer.getCurrentTrack()-1:0;
  if(track>-1&&track<trackTitle->getNumItems()&&track<cdplayer.getNumTracks())
  {
    trackTitle->setCurrentItem(track);
    trackTitle->setTipText(trackTitle->getItemText(track));
  }
  return 1;
}

long CDWindow::onCmdVolume(FXObject*,FXSelector,void* data)
{
  FXString str;
  volLevel=(FXint)data;
  str.format("Volume: %d%",volLevel);
  statusbar->getStatusline()->setText(str);
  cdplayer.setVolume(volLevel);

  return 1;
}

long CDWindow::onUpdVolume(FXObject* sender,FXSelector,void*)
{
  if(volLevel!=cdplayer.getVolume())
  {
    volLevel=cdplayer.getVolume();
    sender->handle(this,MKUINT(ID_SETVALUE,SEL_COMMAND),(void*)volLevel);
  }

  return 1;
}

long CDWindow::onCmdMute(FXObject*,FXSelector,void*)
{
  cdplayer.setMute(!cdplayer.getMute());
  return 1;
}

long CDWindow::onCmdBalance(FXObject*,FXSelector,void* data)
{
  FXfloat value=(FXfloat)(FXint)data;

  //determine percentege of balance to what side
  if(value>100)  //Balance goes right
    volBalance=(value-100)/100.0;
  else if(value<100)          //Balance goes left
    volBalance=(value/100.0)-1.0;
  else
    volBalance=0.0;

  cdplayer.setBalance(volBalance);

  return 1;
}

long CDWindow::onUpdBalance(FXObject* sender,FXSelector,void*)
{
  if(volBalance!=cdplayer.getBalance())
  {
    volBalance=cdplayer.getBalance();

    FXint value=100+(FXint)(100*volBalance);
    sender->handle(this,MKUINT(ID_SETVALUE,SEL_COMMAND),(void*)value);
  }
  return 1;
}

long CDWindow::onActivateSeeker(FXObject*,FXSelector,void*)
{
  seekTrack=cdplayer.getCurrentTrack();
  seekTime.minutes=cdplayer.getTrackTime()->minutes;
  seekTime.seconds=cdplayer.getTrackTime()->seconds;
  return 1;
}

long CDWindow::onDeactivateSeeker(FXObject*,FXSelector,void*)
{
  return 1;
}

long CDWindow::onCmdSeekReverse(FXObject*,FXSelector,void*)
{
  seekTime.seconds--;
  if(seekTime.seconds<0)
  {
    seekTime.minutes--;
    seekTime.seconds=59;
  }

  //Check to see if we skipped over track boundary
  if(seekTime.minutes<0)
  {
    //What should it do for first track?  Stay there I think.  
    FXbool random=cdplayer.getRandom();
    if((seekTrack==cdplayer.getStartTrack())&&!random)
    {
      seekTime.minutes=0;
      seekTime.seconds=0;
    }
    else
    {
      if(random)
      {
        seekTrack=cdplayer.randomTrack();
	if(seekTrack==0)
	{
	  cdplayer.stop();
	  return 1;
	}
      }
      else
        seekTrack--;

      if(cdplayer.getIntro())
      {
	FXuint seconds=cdplayer.getIntroLength();
        seekTime.minutes=seconds/60;
        seekTime.seconds=seconds%60;
      }
      else
      {
        const struct track_info* track=cdplayer.getTrackInfo(seekTrack-1);
        seekTime.minutes=track->track_length.minutes;
        seekTime.seconds=track->track_length.seconds;
      }
    }
  }

  cdplayer.playTrackPos(seekTrack,&seekTime);

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
    const struct track_info* track=cdplayer.getTrackInfo(seekTrack-1);
    boundary.minutes=track->track_length.minutes;
    boundary.seconds=track->track_length.seconds;
  }

  seekTime.seconds++;
  if(seekTime.seconds>59)
  {
    seekTime.minutes++;
    seekTime.seconds=00;
  }

  //Check to see if we skipped over track boundary
  if((seekTime.minutes>=boundary.minutes)&&(seekTime.seconds>boundary.seconds))
  {
    //It may be necessary to test if track is audio - if data and audio tracks are intermixed
    FXint mode=cdplayer.getRepeatMode();
    if(cdplayer.getRandom())
    {
      if(mode!=CDREPEAT_TRACK)
      {
        seekTrack=cdplayer.randomTrack();
        if(seekTrack==0)
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
        if(seekTrack<cdplayer.getNumTracks())
          seekTrack++;
        else if(mode==CDREPEAT_DISC)
	  seekTrack=cdplayer.getStartTrack();
        else
	{
	  cdplayer.stop();
	  return 1;
	}
      }
    }
    seekTime.minutes=0;
    seekTime.seconds=0;
  }

  cdplayer.playTrackPos(seekTrack,&seekTime);

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
  if(cdplayer.isDoorOpen())
    cdplayer.closeTray();
  else
    cdplayer.openTray();
  return 1;
}

long CDWindow::onUpdEject(FXObject* sender,FXSelector,void*)
{
  //FXuint msg=(cdplayer.isValid()&&cdplayer.isDiscPresent()&&cdplayer.getStatus()==CDLYTE_PLAYING||cdplayer.getStatus()==CDLYTE_PAUSED)?ID_DISABLE:ID_ENABLE;
  //sender->handle(this,MKUINT(msg,SEL_COMMAND),NULL);
  return 1;
}
