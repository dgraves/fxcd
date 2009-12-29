/* CDSeekButton.h
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

#ifndef _CDSEEKBUTTON_H_
#define _CDSEEKBUTTON_H_

class CDSeekButton : public FXButton
{
  FXDECLARE(CDSeekButton)
protected:
  FXint    rate;
protected:
  CDSeekButton();
private:
  CDSeekButton(const CDSeekButton&);
  CDSeekButton &operator=(const CDSeekButton&);
public:
  long onLeftBtnPress(FXObject*,FXSelector,void*);
  long onLeftBtnRelease(FXObject*,FXSelector,void*);
  long onRepeat(FXObject*,FXSelector,void*);
  long onClicked(FXObject*,FXSelector,void*);
public:
  enum
  {
    ID_REPEAT=FXFrame::ID_LAST,
    ID_LAST
  };
public:
  //Constructor
  CDSeekButton(FXComposite* p,const FXString& text,FXIcon* ic=NULL,FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=BUTTON_NORMAL,FXint x=0,FXint y=0,FXint w=0,FXint h=0,FXint pl=DEFAULT_PAD,FXint pr=DEFAULT_PAD,FXint pt=DEFAULT_PAD,FXint pb=DEFAULT_PAD);

  //Get timer value in milliseconds.
  //A negative value inidcates use of app scroll speed.
  FXint getSeekRate() const;

  //Set timer value in milliseconds.
  //A negative value inidcates use of app scroll speed.
  void setSeekRate(FXint seekrate);
  
  //Destructor
  ~CDSeekButton();
};

#endif
