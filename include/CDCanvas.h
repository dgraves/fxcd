/* CDCanvas.h
 * Copyright (C) 2004 Dustin Graves <dgraves@computer.org>
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

#ifndef _CDCANVAS_H_
#define _CDCANVAS_H_

enum
{
  CDTIME_DISC    =0,      // Show elapsed disc time in time display
  CDTIME_TRACK   =1,      // Show elapsed track time in time display
  CDTIME_DISCREM =2,      // Show remaining disc time in time display
  CDTIME_TRACKREM=3       // Show remaining track time in time display
};

class FXAPI CDCanvas : public FXCanvas
{
  FXDECLARE(CDCanvas)
  friend class CDWindow;
protected:
  FXuint      timemode;         // Time mode: disc, track, disc remain, track remain
  FXbool      blink;            // Is display currently in the blink state ("--:--")
  FXbool      blinkmode;        // Is blinkmode currently enabled
  FXuint      blinkrate;        // Rate to toggle blink state
  FXTimer*    blinktimer;       // Timer to toggle blink state
  FXColor     lcdforeclr;       // Foreground color for canvas
  FXColor     lcdbackclr;       // Background color for canvas
  cdbmp_array lcdbmp;           // Icons for the display
protected:
  CDCanvas(){}
private:
  CDCanvas(const CDCanvas&);
  CDCanvas &operator=(const CDCanvas&);
public:
  long onMouseDown(FXObject*,FXSelector,void*);
  long onBlink(FXObject*,FXSelector,void*);
public:
  enum
  {
    ID_BLINK=FXCanvas::ID_LAST,
    ID_LAST
  };
public:
  //Constructor
  CDCanvas(FXComposite* p,FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=FRAME_NORMAL,FXint x=0,FXint y=0,FXint w=0,FXint h=0);

  //Widget creation
  virtual void create();

  //Set time mode
  void setTimeMode(FXuint mode);

  //Get time mode
  FXuint getTimeMode() const;

  //Set canvas foreground color
  void setDisplayForeground(FXColor color);

  //Get canvas foreground color
  FXColor getDisplayForeground() const;

  //Set canvas background color
  void setDisplayBackground(FXColor color);

  //Get canvas background color
  FXColor getDisplayBackground() const;

  //Set blink mode for pause
  void setBlinkMode(FXbool mode);

  //Get blink mode for pause
  FXbool getBlinkMode() const;

  //Set blink rate
  void setBlinkRate(FXbool rate);

  //Get blink rate
  FXbool getBlinkRate() const;

  //Draw LCD time display
  void doDraw(FXint track,const struct disc_info* di);

  //Destructor
  ~CDCanvas();
};

#endif
