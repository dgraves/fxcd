/* CDBMPIcon.h
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

#ifndef _CDBMPICON_H_
#define _CDBMPICON_H_

class FXAPI CDBMPIcon : public FXBMPIcon
{
  FXDECLARE(CDBMPIcon)
protected:
  CDBMPIcon(){}
private:
  CDBMPIcon(const CDBMPIcon&);
  CDBMPIcon &operator=(const CDBMPIcon&);
public:
  //Constructor
  CDBMPIcon(FXApp* a,const void *pix=NULL,FXColor clr=FXRGB(192,192,192),FXuint opts=0,FXint w=1,FXint h=1);

  //Replace a color in the icon
  void swapColor(FXColor before,FXColor after);
};

#endif
