/* CDWindow.h
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

#ifndef _CDWINDOW_H_
#define _CDWINDOW_H_

enum
{
  CDTIME_DISC=0,
  CDTIME_TRACK=1,
  CDTIME_DISCREM=2,
  CDTIME_TRACKREM=3
};

enum
{
  CDSTART_NONE=0,
  CDSTART_START=1,
  CDSTART_STOP=2
};

class CDWindow : public FXMainWindow
{
  FXDECLARE(CDWindow)
  friend class CDPreferences;
protected:
  FXIcon* muteIcon;
  FXIcon* nomuteIcon;
  FXIcon* introd;
  FXIcon* randd;
  FXIcon* bicolor[8];
  FXIcon* dcifore[18];

  FXWindow* dcwback[7];

  FXMenubar* menubar; 
  FXStatusbar* statusbar;
  FXTooltip* tooltip;
  FXMenuPane* filemenu;
  FXMenuPane* optionsmenu;
  FXMenuPane* infomenu;
  FXMenuPane* repeatmenu;
  FXMenuPane* timemenu;
  FXFont* font;
  FXCanvas* canvas;
  FXListBox* bandTitle;
  FXListBox* trackTitle;
  FXLabel* albumTitle;

  FXDataTarget* timeTarget;
  FXDataTarget* repeatTarget;
  FXDataTarget* startTarget;
  FXDataTarget* proxyAddrTarget;
  FXDataTarget* proxyPortTarget;
  FXDataTarget* cddbProtoTarget;
  FXDataTarget* cddbAddrTarget;
  FXDataTarget* cdindexAddrTarget;
  FXDataTarget* cdindexPortTarget;

  FXTimer* timer;
  FXColor lcdforecolor;
  FXColor lcdbackcolor;
  FXColor iconcolor;
  FXbool remoteInfo;
  FXbool localFirst;
  FXbool useCDDB;
  FXbool useCDIndex;
  FXbool stopOnExit;
  FXuint startMode;
  FXuint timeMode;            //disc, track, disc remain, track remain
  FXint volLevel;	      //store volume to check against cdplayer for changes
  FXfloat volBalance;	      //store balance to check against cdplayer for changes
  FXint seekTrack;
  struct disc_timeval seekTime;
  CDPlayer cdplayer;
  CDInfo cdinfo;

  CDWindow(){}
  void swapIconColor(FXIcon*,FXColor,FXColor);
  void doDraw(FXint,const struct disc_info*);
  FXbool checkDevices();  //Check for available cdrom devices
  FXbool loadDiscData();  //Load data for currently open device
  FXbool getData(struct disc_data* data);

public:
  CDWindow(FXApp* app);
  ~CDWindow();
  virtual void create();

  void setDisplayFont(FXFont* font);
  FXFont* getDisplayFont() const;

  void setDisplayForeground(FXColor color);
  FXColor getDisplayForeground() const;

  void setDisplayBackground(FXColor color);
  FXColor getDisplayBackground() const;

  void setIconColor(FXColor color);
  FXColor getIconColor() const;

  long onPaint(FXObject*,FXSelector,void*);
  long onMouseDown(FXObject*,FXSelector,void*);
  long onTimeout(FXObject*,FXSelector,void*);
  long onCmdQuit(FXObject*,FXSelector,void*);
  long onUpdStatusDisc(FXObject*,FXSelector,void*);
  long onUpdStatusTrack(FXObject*,FXSelector,void*);
  long onCmdIntro(FXObject*,FXSelector,void*);
  long onUpdIntro(FXObject*,FXSelector,void*);
  long onCmdIntroTime(FXObject*,FXSelector,void*);
  long onUpdIntroTime(FXObject*,FXSelector,void*);
  long onCmdRandom(FXObject*,FXSelector,void*);
  long onUpdRandom(FXObject*,FXSelector,void*);
  long onCmdRepeatSwitch(FXObject*,FXSelector,void*);
  long onUpdRepeatSwitch(FXObject*,FXSelector,void*);
  long onCmdStopOnExit(FXObject*,FXSelector,void*);
  long onUpdStopOnExit(FXObject*,FXSelector,void*);
  long onCmdRemoteInfo(FXObject*,FXSelector,void*);
  long onUpdRemoteInfo(FXObject*,FXSelector,void*);
  long onCmdLocalFirst(FXObject*,FXSelector,void*);
  long onUpdLocalFirst(FXObject*,FXSelector,void*);
  long onCmdProxyServer(FXObject*,FXSelector,void*);
  long onUpdProxyServer(FXObject*,FXSelector,void*);
  long onCmdCDDB(FXObject*,FXSelector,void*);
  long onUpdCDDB(FXObject*,FXSelector,void*);
  long onCmdCDDBPort(FXObject*,FXSelector,void*);
  long onUpdCDDBPort(FXObject*,FXSelector,void*);
  long onCmdCDIndex(FXObject*,FXSelector,void*);
  long onUpdCDIndex(FXObject*,FXSelector,void*);
  long onCmdGetInfo(FXObject*,FXSelector,void*);
  long onCmdColor(FXObject*,FXSelector,void*);
  long onUpdColor(FXObject*,FXSelector,void*);
  long onCmdFont(FXObject*,FXSelector,void*);
  long onCmdPrefs(FXObject*,FXSelector,void*);
  long onCmdToggleMenu(FXObject*,FXSelector,void*);
  long onUpdToggleMenu(FXObject*,FXSelector,void*);
  long onCmdToggleStatus(FXObject*,FXSelector,void*);
  long onUpdToggleStatus(FXObject*,FXSelector,void*);
  long onCmdToggleTips(FXObject*,FXSelector,void*);
  long onUpdToggleTips(FXObject*,FXSelector,void*);
  long onCmdBand(FXObject*,FXSelector,void*);
  long onCmdTrack(FXObject*,FXSelector,void*);
  long onUpdTrack(FXObject*,FXSelector,void*);
  long onCmdVolume(FXObject*,FXSelector,void*);
  long onUpdVolume(FXObject*,FXSelector,void*);
  long onCmdMute(FXObject*,FXSelector,void*);
  long onCmdBalance(FXObject*,FXSelector,void*);
  long onUpdBalance(FXObject*,FXSelector,void*);
  long onActivateSeeker(FXObject*,FXSelector,void*);
  long onDeactivateSeeker(FXObject*,FXSelector,void*);
  long onCmdSeekReverse(FXObject*,FXSelector,void*);
  long onCmdSeekForward(FXObject*,FXSelector,void*);
  long onUpdSeekBtns(FXObject*,FXSelector,void*);
  long onCmdStop(FXObject*,FXSelector,void*);
  long onUpdStop(FXObject*,FXSelector,void*);
  long onCmdPrev(FXObject*,FXSelector,void*);
  long onCmdNext(FXObject*,FXSelector,void*);
  long onUpdSkipBtns(FXObject*,FXSelector,void*);
  long onCmdPlay(FXObject*,FXSelector,void*);
  long onUpdPlay(FXObject*,FXSelector,void*);
  long onCmdEject(FXObject*,FXSelector,void*);
  long onUpdEject(FXObject*,FXSelector,void*);

  enum
  {
    ID_QUIT=FXMainWindow::ID_LAST,

    ID_CANVAS,
    ID_TIMEOUT,

    ID_STATUSDISC,
    ID_STATUSTRACK,

    ID_INTRO,
    ID_INTROTIME,
    ID_RANDOM,

    ID_REPEATNONESWITCH,
    ID_REPEATTRACKSWITCH,
    ID_REPEATDISCSWITCH,
    ID_REPEATSWITCH,

    ID_PREFS,

    ID_STOPONEXIT,

    ID_REMOTEINFO,
    ID_LOCALFIRST,
    ID_PROXYSERVER,
    ID_CDDB,
    ID_CDDBPORT,
    ID_CDINDEX,
    ID_GETINFO,

    ID_COLORFORE,
    ID_COLORBACK,
    ID_COLORICONS,
    ID_FONT,

    ID_TOGGLEMENU,
    ID_TOGGLESTATUS,
    ID_TOGGLETIPS,

    ID_BAND,
    ID_TRACK,
    ID_VOLUME,
    ID_MUTE,
    ID_BALANCE,
    ID_SEEKREVERSE,
    ID_SEEKFORWARD,
    ID_STOP,
    ID_PREV,
    ID_NEXT,
    ID_PLAY,
    ID_EJECT,

    ID_LAST
  };
};

#endif
