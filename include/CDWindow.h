/* CDWindow.h
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

#ifndef _CDWINDOW_H_
#define _CDWINDOW_H_

enum
{
  CDSTART_NONE =0,         // Take no action when program starts
  CDSTART_START=1,         // Start playing a disc when program starts
  CDSTART_STOP =2          // Stop playing a disc (if a disc is playing) when program starts
};

class CDBMPIcon;
class CDCanvas;
class CDListBox;
class CDPreferences;
class CDInfo;

class CDWindow : public FXMainWindow
{
  FXDECLARE(CDWindow)
  friend class CDPreferences;
protected:
  FXbool              stoponexit;        // Stop playing when program exits
  FXuint              startmode;         // Action to take when program starts: nont, start play, stop play
  FXColor             lcdforeclr;        // Color of LCD foreground
  FXColor             lcdbackclr;        // Color of LCD background
  FXColor             iconclr;           // Color of icons on CD player control buttons
  FXFont*             font;              // Font for LCD
  FXint               seektrack;         // Currently seeking in this track
  struct disc_timeval seektime;          // Current seek time in track
  CDPlayer            cdplayer;          // The cd player
protected:
  FXDataTarget        stoponexittgt;
  FXDataTarget        startmodetgt;
  FXDataTarget        timemodetgt;
  FXDataTarget        repeatmodetgt;
  FXDataTarget        introtgt;
protected:
  CDBMPArray         mutebmp;           // Mute icons
  CDBMPArray         btnbmp;            // Icons for cd player controls
  CDBMPArray         lcdbmp;            // Icons for the display
  CDBMPArray         disbmp;            // "Greyed-out" icons for the display
  FXWinArray         lcdwin;            // Buttons for the display
  CDCanvas*           canvas;            // Time display
  CDListBox*          bandtitle;         // List for LCD artist name display
  CDListBox*          tracktitle;        // List for LCD track name display
  FXLabel*            albumtitle;        // Label for LCD album display
  FXMenuBar*          menubar;
  FXMenuPane*         filemenu;
  FXMenuPane*         viewmenu;
  FXMenuPane*         optionsmenu;
  FXMenuPane*         timemenu;
  FXMenuPane*         repeatmenu;
  FXMenuPane*         helpmenu;
  FXMenuPane*         popupmenu;
  FXStatusBar*        statusbar;
  FXToolTip*          tooltip;
  FXHorizontalFrame*  lcdframe;
  CDPreferences*      prefsbox;
protected:
  CDWindow() { }
  void readRegistry();
  void writeRegistry();
  FXbool checkDevices();                      // Check for available cdrom devices
  FXbool loadDiscData();                      // Load data for currently open device
  void genDefaultInfo(CDData* data);          // Fill CDData object with default disc description
  void displayDiscData(const CDData& data);   // Populate the GUI with the data loaded for currently open device
public:
  long onPaint(FXObject*,FXSelector,void*);
  long onMouseUp(FXObject*,FXSelector,void*);
  long onTimeout(FXObject*,FXSelector,void*);
  long onCmdToggleMenuBar(FXObject*,FXSelector,void*);
  long onUpdToggleMenuBar(FXObject*,FXSelector,void*);
  long onCmdToggleStatusBar(FXObject*,FXSelector,void*);
  long onUpdToggleStatusBar(FXObject*,FXSelector,void*);
  long onCmdToggleTooltips(FXObject*,FXSelector,void*);
  long onUpdToggleTooltips(FXObject*,FXSelector,void*);
  long onCmdAbout(FXObject*,FXSelector,void*);
  long onCmdQuit(FXObject*,FXSelector,void*);
  long onUpdStatusDisc(FXObject*,FXSelector,void*);
  long onUpdStatusTrack(FXObject*,FXSelector,void*);
  long onCmdRandom(FXObject*,FXSelector,void*);
  long onUpdRandom(FXObject*,FXSelector,void*);
  long onCmdPrefs(FXObject*,FXSelector,void*);
  long onCmdBand(FXObject*,FXSelector,void*);
  long onCmdTrack(FXObject*,FXSelector,void*);
  long onUpdTrack(FXObject*,FXSelector,void*);
  long onCmdVolume(FXObject*,FXSelector,void*);
  long onUpdVolume(FXObject*,FXSelector,void*);
  long onCmdMute(FXObject*,FXSelector,void*);
  long onUpdMute(FXObject*,FXSelector,void*);
  long onCmdBalance(FXObject*,FXSelector,void*);
  long onUpdBalance(FXObject*,FXSelector,void*);
  long onActivateSeeker(FXObject*,FXSelector,void*);
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
public:
  enum
  {
    ID_QUIT=FXMainWindow::ID_LAST,

    ID_CANVAS,
    ID_TIMEOUT,

    ID_TOGGLEMENUBAR,
    ID_TOGGLESTATUSBAR,
    ID_TOGGLETOOLTIPS,
    ID_ABOUT,

    ID_STATUSDISC,
    ID_STATUSTRACK,

    ID_RANDOM,

    ID_PREFS,

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
public:
  // Constructor
  CDWindow(FXApp* app);

  // Widget creation
  virtual void create();

  // Set font for LCD
  void setDisplayFont(FXFont* font);

  // Get font for LCD
  FXFont* getDisplayFont() const;

  // Set LCD foreground color
  void setDisplayForeground(FXColor color);

  // Get LCD foreground color
  FXColor getDisplayForeground() const;

  // Set LCD background color
  void setDisplayBackground(FXColor color);

  // Get LCD background color
  FXColor getDisplayBackground() const;

  // Set button icon color
  void setIconColor(FXColor color);

  // Get button icon color
  FXColor getIconColor() const;

  // Add a new CD audio device
  FXbool addDevice(const FXString& devnam);

  // Remove an existing CD audio device
  FXbool removeDevice(const FXString& devnam);

  // Destructor
  ~CDWindow();
};

#endif

/*

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

class CDBMPIcon;
class CDPreferences;

class CDWindow : public FXMainWindow
{
  FXDECLARE(CDWindow)
  friend class CDPreferences;
protected:
  CDBMPIcon* muteIcon;
  CDBMPIcon* nomuteIcon;
  FXIcon* introd;
  FXIcon* randd;
  CDBMPIcon* bicolor[8];
  CDBMPIcon* dcifore[18];

  FXWindow* dcwback[7];

  FXMenuBar* menubar;
  FXStatusBar* statusbar;
  FXToolTip* tooltip;
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

  FXTimer* timer;
  FXColor lcdforecolor;
  FXColor lcdbackcolor;
  FXColor iconcolor;
  FXbool remoteInfo;
  FXbool localFirst;
  FXbool useCDDB;
  FXbool stopOnExit;
  FXuint startMode;
  FXuint timeMode;            //disc, track, disc remain, track remain
  FXint volLevel;	      //store volume to check against cdplayer for changes
  FXfloat volBalance;	      //store balance to check against cdplayer for changes
  FXint seekTrack;
  struct disc_timeval seekTime;
  CDPlayer cdplayer;
  CDInfo cdinfo;
  CDPreferences* cdprefs;

  CDWindow(){}
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
  long onCmdPrefs(FXObject*,FXSelector,void*);
  long onCmdStopOnExit(FXObject*,FXSelector,void*);
  long onUpdStopOnExit(FXObject*,FXSelector,void*);
  long onCmdRemoteInfo(FXObject*,FXSelector,void*);
  long onUpdRemoteInfo(FXObject*,FXSelector,void*);
  long onCmdLocalFirst(FXObject*,FXSelector,void*);
  long onUpdLocalFirst(FXObject*,FXSelector,void*);
  long onCmdProxyServer(FXObject*,FXSelector,void*);
  long onUpdProxyServer(FXObject*,FXSelector,void*);
  long onCmdCDDBPort(FXObject*,FXSelector,void*);
  long onUpdCDDBPort(FXObject*,FXSelector,void*);
  long onCmdGetInfo(FXObject*,FXSelector,void*);
  long onCmdColor(FXObject*,FXSelector,void*);
  long onUpdColor(FXObject*,FXSelector,void*);
  long onCmdFont(FXObject*,FXSelector,void*);
  long onCmdCDROMAdd(FXObject*,FXSelector,void*);
  long onCmdCDROMRemove(FXObject*,FXSelector,void*);
  long onCmdToggleMenu(FXObject*,FXSelector,void*);
  long onUpdToggleMenu(FXObject*,FXSelector,void*);
  long onCmdToggleStatus(FXObject*,FXSelector,void*);
  long onUpdToggleStatus(FXObject*,FXSelector,void*);
  long onCmdToggleTips(FXObject*,FXSelector,void*);
  long onUpdToggleTips(FXObject*,FXSelector,void*);
  long onCmdDefaultOptions(FXObject*,FXSelector,void*);
  long onCmdDefaultAppearance(FXObject*,FXSelector,void*);
  long onCmdDefaultInternet(FXObject*,FXSelector,void*);
  long onCmdDefaultHardware(FXObject*,FXSelector,void*);
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
    ID_GETINFO,

    ID_COLORFORE,
    ID_COLORBACK,
    ID_COLORICONS,
    ID_FONT,

    ID_CDROMADD,
    ID_CDROMREM,

    ID_TOGGLEMENU,
    ID_TOGGLESTATUS,
    ID_TOGGLETIPS,

    ID_DEFAULTOPTIONS,
    ID_DEFAULTAPPEARANCE,
    ID_DEFAULTHARDWARE,
    ID_DEFAULTINTERNET,

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
*/
