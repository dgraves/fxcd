/* CDServerDialog.cpp
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

#include "cdlyte.h"
#include "fox/fx.h"
#include "CDServerDialog.h"

FXDEFMAP(CDServerDialog) CDServerDialogMap[]={
  FXMAPFUNC(SEL_DOUBLECLICKED,CDServerDialog::ID_LIST,CDServerDialog::onCmdList)
};

FXIMPLEMENT(CDServerDialog,FXDialogBox,CDServerDialogMap,ARRAYNUMBER(CDServerDialogMap))

CDServerDialog::CDServerDialog(FXWindow* owner,const struct cddb_serverlist* servers)
: FXDialogBox(owner,"CDDB Servers",DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE,0,0,410,250, 10,10,10,10, 4,4)
{
  FXint i;
  FXString s;

  FXHorizontalFrame* buttons=new FXHorizontalFrame(this,PACK_UNIFORM_WIDTH|LAYOUT_SIDE_BOTTOM|LAYOUT_RIGHT);
  new FXButton(buttons,"&OK",NULL,this,FXDialogBox::ID_ACCEPT,FRAME_RAISED|FRAME_THICK|LAYOUT_RIGHT,0,0,0,0, 20,20);
  new FXButton(buttons,"&Cancel",NULL,this,FXDialogBox::ID_CANCEL,FRAME_RAISED|FRAME_THICK|LAYOUT_RIGHT,0,0,0,0, 20,20);

  new FXHorizontalSeparator(this,SEPARATOR_RIDGE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

  FXHorizontalFrame* listframe=new FXHorizontalFrame(this,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
  list=new FXIconList(listframe,this,ID_LIST,ICONLIST_BROWSESELECT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  list->appendHeader("Address",NULL,120);
  list->appendHeader("Port",NULL,40);
  list->appendHeader("Protocol",NULL,90);
  list->appendHeader("Lat",NULL,60);
  list->appendHeader("Lon",NULL,60);
  list->appendHeader("Description",NULL,150);
  list->appendHeader("Script",NULL,120);

  for(i=0;i<servers->list_len;i++)
  {
    //CDaudio is slightly broken.  Need to parse addressing string
    FXString parse(servers->list_host[i].host_addressing);
    FXString script;
    if(servers->list_host[i].host_protocol==CDDB_MODE_HTTP)
    {
      script=parse.before(' ');
      parse=parse.after(' ');
    }
    else
    {
      script="NONE";
      parse=parse.after(' ');
    }

    FXString lat=parse.before(' ');
    parse=parse.after(' ');
    FXString lon=parse.before(' ');
    parse=parse.after(' ');

    s.format("%s\t%d\t%s\t%s\t%s\t%s\t%s",
             servers->list_host[i].host_server.server_name,
	     servers->list_host[i].host_server.server_port,
	     (servers->list_host[i].host_protocol==CDDB_MODE_HTTP)?"CDDB_HTTP":"CDDB_CDDBP",
	     lat.text(),
	     lon.text(),
	     parse.text(),
	     script.text());

    list->appendItem(s);
  }
}

FXint CDServerDialog::getSelection() const
{
  return list->getCurrentItem();
}

void CDServerDialog::setSelection(FXint index)
{
  list->setCurrentItem(index);
}

long CDServerDialog::onCmdList(FXObject*,FXSelector,void*)
{
  handle(this,MKUINT(FXDialogBox::ID_ACCEPT,SEL_COMMAND),NULL);
  return 1;
}
