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

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#endif

#include <signal.h>
#include <cdlyte.h>
#include <fox-1.6/fx.h>
#include <fox-1.6/FXArray.h>
#include <fox-1.6/FXElement.h>
#include "CDdefs.h"
#include "CDData.h"
#include "CDPlayer.h"
#include "CDDBSettings.h"
#include "CDInfo.h"
#include "CDDBInfo.h"
#include "CDWindow.h"

int main(int argc, char** argv)
{
  FXApp app(PROG_PACKAGE,FXString::null);
  app.init(argc,argv);

  // Initialize winsock for obtaining remote cd info
#ifdef WIN32
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

  CDWindow* win=new CDWindow(&app);
  app.addSignal(SIGINT,win,CDWindow::ID_QUIT);
  app.create();
  win->show();

  FXint result = app.run();

#ifdef WIN32
  WSACleanup();
#endif

  return result;
}
