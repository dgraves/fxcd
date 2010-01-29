/* CDDBServerListExecutor.h
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

#ifndef _CDDBSERVERLISTEXECUTOR_H_
#define _CDDBSERVERLISTEXECUTOR_H_

class CDDBServerListTask;

class CDDBServerListExecutor : public FXDialogBox
{
  FXDECLARE(CDDBServerListExecutor)
private:
  struct cddb_serverlist* list;
  CDDBSettings settings;
  FXGUISignal* signal;
  CDDBServerListTask* task;
protected:
  FXbool success;
  FXuint progcount;
  FXString progress;
protected:
  FXDataTarget progresstgt;
protected:
  CDDBServerListExecutor(){};
public:
  long onGUISignal(FXObject*,FXSelector,void*);
  long onTimeout(FXObject*,FXSelector,void*);
public:
  enum
  {
    ID_GUISIGNAL=FXDialogBox::ID_LAST,
    ID_TIMEOUT,
    ID_LAST
  };
public:
  CDDBServerListExecutor(FXWindow* owner,const CDDBSettings& cddbsettings,struct cddb_serverlist* serverlist);

  FXbool getSuccess() const;

  virtual FXuint execute(FXuint placement=PLACEMENT_CURSOR);

  ~CDDBServerListExecutor();
};

#endif
