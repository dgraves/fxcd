/* CDListBox.cpp
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

#include "fox-1.6/fx.h"
#include "CDListBox.h"

FXIMPLEMENT(CDListBox,FXListBox,NULL,0)

CDListBox::CDListBox(FXComposite *p,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h,FXint pl,FXint pr,FXint pt,FXint pb)
: FXListBox(p,tgt,sel,opts,x,y,w,h,pl,pr,pt,pb)
{
  button->setFrameStyle(FRAME_LINE|FRAME_THICK);
  button->setBaseColor(getTextColor());
  button->setBorderColor(getTextColor());
  button->setHiliteColor(getBackColor());
  button->setTextColor(getTextColor());
  button->setBackColor(getBackColor());
}

void CDListBox::setBackColor(FXColor clr)
{
  FXListBox::setBackColor(clr);
  button->setBackColor(clr);
}

void CDListBox::setForeColor(FXColor clr)
{
  setTextColor(clr);
  button->setBaseColor(clr);
  button->setBorderColor(clr);
  button->setShadowColor(clr);
  button->setHiliteColor(clr);
  button->setTextColor(clr);
}
