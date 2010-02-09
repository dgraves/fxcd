/* CDWindow.h
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

class CDWindow : public FXMainWindow
{
  FXDECLARE(CDWindow)
  friend class CDPreferences;
protected:
  FXbool                 stoponexit;        // Stop playing when program exits
  FXuint                 startmode;         // Action to take when program starts: nont, start play, stop play
  FXColor                lcdforeclr;        // Color of LCD foreground
  FXColor                lcdbackclr;        // Color of LCD background
  FXColor                iconclr;           // Color of icons on CD player control buttons
  FXFont*                font;              // Font for LCD
  FXint                  seektrack;         // Currently seeking in this track
  struct disc_timeval    seektime;          // Current seek time in track
  FXuint                 seekrate;          // Current seek rate (seconds)
  FXuint                 initseekrate;      // Inital seek rate (seconds)
  FXuint                 fastseekrate;      // Faster seek rate to be activated after seeking for a specific interval (seconds)
  FXuint                 fastseekstart;     // Interval that must elapse before the faster seek rate is used (seconds)
  CDPlayer               cdplayer;          // The cd player
  FXbool                 usecddb;
  CDDBSettings cddbsettings;
  CDInfoTaskList         tasklist;          // List to hold info tasks that retrieve CD info
protected:
  FXDataTarget           stoponexittgt;
  FXDataTarget           startmodetgt;
  FXDataTarget           timemodetgt;
  FXDataTarget           repeatmodetgt;
  FXDataTarget           introtgt;
protected:
  FXIcon*             bigcd;
  FXIcon*             smallcd;
  CDBMPArray          mutebmp;           // Mute icons
  CDBMPArray          btnbmp;            // Icons for cd player controls
  CDBMPArray          lcdbmp;            // Icons for the display
  CDBMPArray          disbmp;            // "Greyed-out" icons for the display
  FXWinArray          lcdwin;            // Buttons for the display
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
  long onGUISignal(FXObject*,FXSelector,void*);
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
  long onDeactivateSeeker(FXObject*,FXSelector,void*);
  long onSeekRateTimeout(FXObject*,FXSelector,void*);
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
    ID_GUISIGNAL,

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
    ID_SEEKRATETIMEOUT,
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
