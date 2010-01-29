/* CDDBServerListTask.h
 * Copyright (C) 2010 Dustin Graves <dgraves@computer.org>
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

#ifndef _CDDBSERVERLISTTASK_H_
#define _CDDBSERVERLISTTASK_H_

class CDDBServerListTask : public FXThread
{
private:
  FXGUISignal* signal;
  struct cddb_host host;
  struct cddb_hello hello;
  struct cddb_server server;
  struct cddb_server *pserver;
protected:
  struct cddb_serverlist* list;
  CDDBSettings settings;
protected:
  void init();
public:
  CDDBServerListTask(const CDDBSettings& cddbsettings,struct cddb_serverlist* serverlist,FXGUISignal* guisignal=NULL);

  virtual FXint run();

  ~CDDBServerListTask();
};

#endif
