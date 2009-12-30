/* CDSeekButton.cpp
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

#include <fox-1.6/fx.h>
#include "CDSeekButton.h"

FXDEFMAP(CDSeekButton) CDSeekButtonMap[]={
  FXMAPFUNC(SEL_LEFTBUTTONPRESS,0,CDSeekButton::onLeftBtnPress),
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,0,CDSeekButton::onLeftBtnRelease),
  FXMAPFUNC(SEL_TIMEOUT,CDSeekButton::ID_REPEAT,CDSeekButton::onRepeat)
};

FXIMPLEMENT(CDSeekButton,FXButton,CDSeekButtonMap,ARRAYNUMBER(CDSeekButtonMap))

CDSeekButton::CDSeekButton()
: rate(-1)
{
}

CDSeekButton::CDSeekButton(FXComposite* p,const FXString& text,FXIcon* ic,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h,FXint pl,FXint pr,FXint pt,FXint pb)
: FXButton(p,text,ic,tgt,sel,opts,x,y,w,h,pl,pr,pt,pb),
  rate(-1)
{
}

FXint CDSeekButton::getSeekRate() const
{
  return rate;
}

void CDSeekButton::setSeekRate(FXint seekrate)
{
  rate=seekrate;
  if(getApp()->hasTimeout(this, ID_REPEAT))
  {
    getApp()->addTimeout(this,ID_REPEAT,(rate<0)?getApp()->getScrollSpeed():rate);
  }
}

long CDSeekButton::onLeftBtnPress(FXObject* sender,FXSelector sel,void* ptr)
{
  if(FXButton::onLeftBtnPress(sender,sel,ptr))
  {
    getApp()->addTimeout(this,ID_REPEAT,(rate<0)?getApp()->getScrollSpeed():rate);
    return 1;
  }
  return 0;
}

long CDSeekButton::onLeftBtnRelease(FXObject* sender,FXSelector sel,void* ptr)
{
  if(FXButton::onLeftBtnRelease(sender,sel,ptr))
  {
    getApp()->removeTimeout(this,ID_REPEAT);
    return 1;
  }
  return 0;
}

long CDSeekButton::onRepeat(FXObject*,FXSelector,void* ptr)
{
  if(target) target->handle(this,FXSEL(SEL_COMMAND,message),ptr);
  getApp()->addTimeout(this,ID_REPEAT,(rate<0)?getApp()->getScrollSpeed():rate);
  return 1;
}

CDSeekButton::~CDSeekButton()
{
  getApp()->removeTimeout(this,ID_REPEAT);
}
