/* CDBMPIcon.cpp
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
#include "CDBMPIcon.h"

FXIMPLEMENT(CDBMPIcon,FXBMPIcon,NULL,0)

CDBMPIcon::CDBMPIcon(FXApp* a,const void *pix,FXColor clr,FXuint opts,FXint w,FXint h)
: FXBMPIcon(a,pix,clr,opts,w,h)
{
}

void CDBMPIcon::swapColor(FXColor before,FXColor after)
{
  if(before!=after)
  {
    FXuint i=0,size=width*height;
    for(i=0;i<size;i++)
    {
      if(data[i]==before)
        data[i]=after;
      else if(after==transp&&data[i]==transp)
        data[i]=before;
    }
    if(after==transp) transp=before;
    render();
  }
}
