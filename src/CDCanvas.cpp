/* CDCanvas.cpp
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

#include <cdlyte.h>
#include "fox-1.6/fx.h"
#include "CDdefs.h"
#include "CDBMPIcon.h"
#include "CDCanvas.h"
#include "icons.h"

FXDEFMAP(CDCanvas) CDCanvasMap[]={
  FXMAPFUNC(SEL_LEFTBUTTONPRESS,0,CDCanvas::onMouseDown),
  FXMAPFUNC(SEL_TIMEOUT,CDCanvas::ID_BLINK,CDCanvas::onBlink)
};

FXIMPLEMENT(CDCanvas,FXCanvas,CDCanvasMap,ARRAYNUMBER(CDCanvasMap))

CDCanvas::CDCanvas(FXComposite* p,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h)
: FXCanvas(p,tgt,sel,opts,x,y,w,h),
  dw(w),
  dh(h),
  timemode(CDTIME_TRACK),
  blink(FALSE),
  blinkmode(FALSE),
  blinkrate(1000),
  blinktimer(NULL),
  lcdforeclr(FXRGB(255,255,255)),
  lcdbackclr(FXRGB(0,0,0))
{
  lcdbmp.push_back(new CDBMPIcon(getApp(),zero_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  lcdbmp.push_back(new CDBMPIcon(getApp(),one_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  lcdbmp.push_back(new CDBMPIcon(getApp(),two_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  lcdbmp.push_back(new CDBMPIcon(getApp(),three_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  lcdbmp.push_back(new CDBMPIcon(getApp(),four_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  lcdbmp.push_back(new CDBMPIcon(getApp(),five_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  lcdbmp.push_back(new CDBMPIcon(getApp(),six_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  lcdbmp.push_back(new CDBMPIcon(getApp(),seven_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  lcdbmp.push_back(new CDBMPIcon(getApp(),eight_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  lcdbmp.push_back(new CDBMPIcon(getApp(),nine_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  lcdbmp.push_back(new CDBMPIcon(getApp(),dash_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
  lcdbmp.push_back(new CDBMPIcon(getApp(),colon_bmp,0,IMAGE_ALPHAGUESS|IMAGE_KEEP));
}

void CDCanvas::create()
{
  cdbmp_array::iterator iter;

  FXCanvas::create();

  // Create icons
  for(iter=lcdbmp.begin();iter!=lcdbmp.end();++iter)
    (*iter)->create();
}

FXint CDCanvas::getDefaultWidth()
{
  return (dw==0)?FXCanvas::getDefaultWidth():dw;
}

FXint CDCanvas::getDefaultHeight()
{
  return (dh==0)?FXCanvas::getDefaultHeight():dh;
}

void CDCanvas::setTimeMode(FXuint mode)
{
  timemode=mode;
}

FXuint CDCanvas::getTimeMode() const
{
  return timemode;
}

void CDCanvas::setDisplayForeground(FXColor color)
{
  cdbmp_array::iterator iter;

  //Needed for dual color icons.  If they are equal, modify slightly.
  if(color==lcdbackclr)
  {
    FXuchar r=FXREDVAL(color),g=FXGREENVAL(color),b=FXBLUEVAL(color);
    color=FXRGB((r<255)?r+1:r-1,(g<255)?g+1:g-1,(b<255)?b+1:b-1);
  }

  //Icons
  for(iter=lcdbmp.begin();iter!=lcdbmp.end();++iter)
    (*iter)->swapColor(lcdforeclr,color);

  lcdforeclr=color;
}

FXColor CDCanvas::getDisplayForeground() const
{
  return lcdforeclr;
}

void CDCanvas::setDisplayBackground(FXColor color)
{
  lcdbackclr=color;
}

FXColor CDCanvas::getDisplayBackground() const
{
  return lcdbackclr;
}

void CDCanvas::setBlinkMode(FXbool mode)
{
  if(blinkmode!=mode)
  {
    blink=TRUE;
    blinkmode=mode;
    if(blinkmode) 
	  getApp()->addTimeout(this,ID_BLINK,blinkrate);
  }
}

FXbool CDCanvas::getBlinkMode() const
{
  return blinkmode;
}

void CDCanvas::setBlinkRate(FXbool rate)
{
  blinkrate=rate;
  if(blinktimer)
  {
    getApp()->addTimeout(this,ID_BLINK,blinkrate);
  }
}

FXbool CDCanvas::getBlinkRate() const
{
  return blinkrate;
}

void CDCanvas::doDraw(FXint track,const struct disc_info* di)
{
  struct disc_timeval drawTime;
  FXbool negative=FALSE;
  FXString text;

  //Create string to draw
  if(di->disc_mode==CDLYTE_PLAYING||di->disc_mode==CDLYTE_PAUSED)
  {
    if(blinkmode&&blink)
    {
      //Draw blink text for blink frame
      text.format("%02d--:--",track);
    }
    else
    {
      //Format time for proper output
      if(timemode==CDTIME_DISC)
      {
        // Subtract leadin from current disc position
        cd_frames_to_msf(&drawTime,cd_msf_to_frames(&di->disc_time)-cd_msf_to_frames(&di->disc_track[0].track_pos));
      }
      else if(timemode==CDTIME_TRACK)
      {
        drawTime.minutes=di->disc_track_time.minutes;
        drawTime.seconds=di->disc_track_time.seconds;
      }
      else if(timemode==CDTIME_DISCREM)
      {
        FXint pos=(di->disc_time.minutes*60)+di->disc_time.seconds;
        FXint len=(di->disc_length.minutes*60)+di->disc_length.seconds;
        pos=len-pos;
        drawTime.minutes=pos/60;
        drawTime.seconds=pos%60;
        negative=TRUE;
      }
      else if(timemode==CDTIME_TRACKREM)
      {
        FXint pos=(di->disc_track_time.minutes*60)+di->disc_track_time.seconds;
        FXint len=(di->disc_track[track-1].track_length.minutes*60)+di->disc_track[track-1].track_length.seconds;
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
    //If CD is not playing draw 0 for time
    text.format("%02d00:00",track);
  }

  //Create image for double buffering
  FXImage back(getApp(),NULL,IMAGE_OPAQUE,getWidth(),getHeight());
  back.create();

  //Create device context
  FXDCWindow dc(&back);

  //Draw background
  dc.setForeground(lcdbackclr);
  dc.fillRectangle(0,0,getWidth(),getHeight());

  FXint x=DEFAULT_SPACING,y=DEFAULT_SPACING*3,p=0;
  FXIcon* drawMe=NULL;

  while(text[p]!='\0')
  {
    if(p==2)
    {
      x+=DEFAULT_PAD;
      if(negative) dc.drawIcon(lcdbmp[10],x,y);
      x+=lcdbmp[10]->getWidth();
    }

    drawMe=(text[p]=='-')?lcdbmp[10]:lcdbmp[text[p]-'0'];
    dc.drawIcon(drawMe,x,y);
    x+=lcdbmp[0]->getWidth();
    p++;
    if(text[p]==':')
    {
      dc.drawIcon(lcdbmp[11],x,y);
      x+=lcdbmp[11]->getWidth();
      p++;
    }
  }

  dc.end();
  dc.begin(this);
  dc.drawImage(&back,0,0);
}

long CDCanvas::onMouseDown(FXObject*,FXSelector,void*)
{
  if(++timemode>CDTIME_TRACKREM) timemode=CDTIME_DISC;
  return 1;
}

long CDCanvas::onBlink(FXObject*,FXSelector,void*)
{
  blink=!blink;
  getApp()->addTimeout(this,ID_BLINK,blinkrate);
  return 1;
}

CDCanvas::~CDCanvas()
{
  cdbmp_array::iterator iter;

  getApp()->removeTimeout(this,ID_BLINK);

  for(iter=lcdbmp.begin();iter!=lcdbmp.end();++iter)
    delete (*iter);
}
