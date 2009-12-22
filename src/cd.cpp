/* cd.cpp
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

#include <signal.h>
#include "cdlyte.h"
#include "fox/fx.h"
#include "fox/FXArray.h"
#include "fox/FXElement.h"
#include "CDdefs.h"
#include "CDPlayer.h"
#include "CDWindow.h"

int main(int argc, char** argv)
{
  FXApp app(PROG_PACKAGE,FXString::null);
  app.init(argc,argv);

  CDWindow* win=new CDWindow(&app);
  app.addSignal(SIGINT,win,CDWindow::ID_QUIT);
  app.create();
  win->show();
  return app.run();
}
