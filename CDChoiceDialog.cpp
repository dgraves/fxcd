/* CDChoiceDialog.cpp
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

extern "C" {
#include "cdaudio.h"
}
#include "fox/fx.h"
#include "CDChoiceDialog.h"

FXDEFMAP(CDChoiceDialog) CDChoiceDialogMap[]={
  FXMAPFUNC(SEL_DOUBLECLICKED,CDChoiceDialog::ID_LIST,CDChoiceDialog::onCmdList)
};

FXIMPLEMENT(CDChoiceDialog,FXDialogBox,CDChoiceDialogMap,ARRAYNUMBER(CDChoiceDialogMap))

CDChoiceDialog::CDChoiceDialog(FXWindow* owner,const struct cddb_query* entries)
: FXDialogBox(owner,"CD Info Selection",DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE,0,0,360,250, 10,10,10,10, 4,4)
{
  FXint i;
  FXString s;

  FXHorizontalFrame* buttons=new FXHorizontalFrame(this,PACK_UNIFORM_WIDTH|LAYOUT_SIDE_BOTTOM|LAYOUT_RIGHT);
  new FXButton(buttons,"&OK",NULL,this,FXDialogBox::ID_ACCEPT,FRAME_RAISED|FRAME_THICK|LAYOUT_RIGHT,0,0,0,0, 20,20);
  new FXButton(buttons,"&Cancel",NULL,this,FXDialogBox::ID_CANCEL,FRAME_RAISED|FRAME_THICK|LAYOUT_RIGHT,0,0,0,0, 20,20);

  new FXHorizontalSeparator(this,SEPARATOR_RIDGE|LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

  FXVerticalFrame* labelframe=new FXVerticalFrame(this,LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FXLabel(labelframe,"There are multiple data entries to choose from:",NULL,LAYOUT_CENTER_Y);
  FXHorizontalFrame* listframe=new FXHorizontalFrame(this,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
  list=new FXIconList(listframe,this,ID_LIST,ICONLIST_BROWSESELECT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
  list->appendHeader("Genre",NULL,80);
  list->appendHeader("Artist",NULL,100);
  list->appendHeader("Title",NULL,155);

  for(i=0;i<entries->query_matches;i++)
  {
    FXString genre;
    switch(entries->query_list[i].list_genre)
    {
    case CDDB_BLUES:
      genre="Blues";
      break;
    case CDDB_CLASSICAL:
      genre="Classical";
      break;
    case CDDB_COUNTRY:
      genre="Country";
      break;
    case CDDB_DATA:
      genre="Data";
      break;
    case CDDB_FOLK:
      genre="Folk";
      break;
    case CDDB_JAZZ:
      genre="Jazz";
      break;
    case CDDB_MISC:
      genre="Misc";
      break;
    case CDDB_NEWAGE:
      genre="New Age";
      break;
    case CDDB_REGGAE:
      genre="Reggae";
      break;
    case CDDB_ROCK:
      genre="Rock";
      break;
    case CDDB_SOUNDTRACK:
      genre="Soundtrack";
      break;
    case CDDB_UNKNOWN:
    default:
      genre="Unknown";
    }

    s.format("%s\t%s\t%s",genre.text(),entries->query_list[i].list_artist,entries->query_list[i].list_title);

    list->appendItem(s);
  }
}

FXint CDChoiceDialog::getSelection() const
{
  return list->getCurrentItem();
}

void CDChoiceDialog::setSelection(FXint index)
{
  list->setCurrentItem(index);
}

long CDChoiceDialog::onCmdList(FXObject*,FXSelector,void*)
{
  handle(this,MKUINT(FXDialogBox::ID_ACCEPT,SEL_COMMAND),NULL);
  return 1;
}
