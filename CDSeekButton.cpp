/* CDSeekButton.h
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

#include "fox/fx.h"
#include "CDSeekButton.h"

FXDEFMAP(CDSeekButton) CDSeekButtonMap[]={
  FXMAPFUNC(SEL_LEFTBUTTONPRESS,0,CDSeekButton::onLeftBtnPress),
  FXMAPFUNC(SEL_LEFTBUTTONRELEASE,0,CDSeekButton::onLeftBtnRelease),
  FXMAPFUNC(SEL_TIMEOUT,CDSeekButton::ID_REPEAT,CDSeekButton::onRepeat)
};

FXIMPLEMENT(CDSeekButton,FXButton,CDSeekButtonMap,ARRAYNUMBER(CDSeekButtonMap))

CDSeekButton::CDSeekButton()
{
  repeater=NULL;
  fired=FALSE;
  rate=-1;
}

CDSeekButton::CDSeekButton(FXComposite* p,const FXString& text,FXIcon* ic,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h,FXint pl,FXint pr,FXint pt,FXint pb)
: FXButton(p,text,ic,tgt,sel,opts,x,y,w,h,pl,pr,pt,pb)
{
  repeater=NULL;
  fired=FALSE;
  rate=-1;
}

long CDSeekButton::onLeftBtnPress(FXObject* sender,FXSelector sel,void* data)
{
  flags|=FLAG_PRESSED;
  flags&=~FLAG_UPDATE;
  if(state!=STATE_ENGAGED) setState(STATE_DOWN);
  if(target) target->handle(this,MKUINT(message,SEL_ACTIVATE),NULL);
  repeater=getApp()->addTimeout((rate<0)?getApp()->getScrollSpeed():rate,this,ID_REPEAT);
  fired=FALSE; 
  return 1;
}

long CDSeekButton::onLeftBtnRelease(FXObject*,FXSelector,void* ptr)
{
  flags&=~FLAG_PRESSED;
  flags|=FLAG_UPDATE;
  if(state!=STATE_ENGAGED) setState(STATE_UP);
  if(repeater) getApp()->removeTimeout(repeater);
  repeater=NULL;
  if(!fired){ if(target) target->handle(this,MKUINT(message,SEL_COMMAND),(void*)1); }
  if(target) target->handle(this,MKUINT(message,SEL_DEACTIVATE),NULL);
  fired=FALSE;
  return 1;
}

long CDSeekButton::onRepeat(FXObject*,FXSelector,void*)
{
  FXuint click=(state==STATE_DOWN);
  if(target) target->handle(this,MKUINT(message,SEL_COMMAND),(void*)1);
  repeater=getApp()->addTimeout((rate<0)?getApp()->getScrollSpeed():rate,this,ID_REPEAT);
  fired=TRUE;
  return 1;
}


