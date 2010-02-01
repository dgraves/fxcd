/* CDDBServerListExecutor.cpp
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

#include <cdlyte.h>
#include <fox-1.6/fx.h>
#include "CDDBSettings.h"
#include "CDDBServerListTask.h"
#include "CDDBServerListExecutor.h"

namespace {
  FXchar progstates[][4] = {".  ",".. ","..."," ..","  .","   ","  ."," ..","...",".. ",".  ","   "};
  FXuint progtotal=ARRAYNUMBER(progstates);
}

FXDEFMAP(CDDBServerListExecutor) CDDBServerListExecutorMap[]={
  FXMAPFUNC(SEL_IO_READ,CDDBServerListExecutor::ID_GUISIGNAL,CDDBServerListExecutor::onGUISignal),
  FXMAPFUNC(SEL_TIMEOUT,CDDBServerListExecutor::ID_TIMEOUT,CDDBServerListExecutor::onTimeout)
};

FXIMPLEMENT(CDDBServerListExecutor,FXDialogBox,CDDBServerListExecutorMap,ARRAYNUMBER(CDDBServerListExecutorMap))

CDDBServerListExecutor::CDDBServerListExecutor(FXWindow* owner,const CDDBSettings& cddbsettings,struct cddb_serverlist* serverlist)
: FXDialogBox(owner,"Retrieving List",DECOR_TITLE|DECOR_BORDER,0,0,0,0),
  list(serverlist),
  settings(cddbsettings),
  signal(NULL),
  task(NULL),
  success(FALSE),
  progcount(0),
  progress("   ")
{
  progresstgt.connect(progress);

  FXVerticalFrame* contents=new FXVerticalFrame(this,LAYOUT_FILL);
  FXHorizontalFrame* labelframe=new FXHorizontalFrame(contents,LAYOUT_CENTER_X|LAYOUT_CENTER_Y,0,0,0,0, 0,0,0,0);
  new FXLabel(labelframe,"Retrieving CDDB server list ",NULL,LAYOUT_CENTER_Y);
  FXLabel* progresslabel=new FXLabel(labelframe,"   ",NULL,LAYOUT_CENTER_Y);
  progresslabel->setTarget(&progresstgt);
  progresslabel->setSelector(FXDataTarget::ID_VALUE);
  new FXHorizontalSeparator(contents,SEPARATOR_RIDGE|LAYOUT_FILL_X);
  new FXButton(contents,"&Cancel",NULL,this,FXDialogBox::ID_CANCEL,FRAME_RAISED|FRAME_THICK|LAYOUT_CENTER_X,0,0,0,0, 20,20);

  getApp()->addTimeout(this,ID_TIMEOUT,1);
}

long CDDBServerListExecutor::onGUISignal(FXObject*,FXSelector,void*)
{
  getApp()->stopModal(this,TRUE);
  return 1;
}

long CDDBServerListExecutor::onTimeout(FXObject*,FXSelector,void*)
{
  progress=progstates[progcount];
  progcount=(progcount+1)%progtotal;
  getApp()->addTimeout(this,ID_TIMEOUT,250);
  return 1;
}

FXbool CDDBServerListExecutor::getSuccess() const
{
  return success;
}

FXuint CDDBServerListExecutor::execute(FXuint placement)
{
  FXuint result;

  signal=new FXGUISignal(getApp(),this,CDDBServerListExecutor::ID_GUISIGNAL);
  task=new CDDBServerListTask(settings,list,signal);

  task->start();

  result=FXDialogBox::execute(placement);
  if(result)
  {
    // The process failed if the signal's data object is set to NULL
    success=(signal->getData()==NULL)?FALSE:TRUE;
    task->join();
  }
  else
  {
    success=FALSE;
    task->cancel();
  }

  getApp()->removeTimeout(this,ID_TIMEOUT);

  delete task;
  delete signal;

  task=NULL;
  signal=NULL;

  return result;
}

CDDBServerListExecutor::~CDDBServerListExecutor()
{
  getApp()->removeTimeout(this,ID_TIMEOUT);

  if(task!=NULL)
  {
    task->cancel();
    delete task;
  }

  if(signal!=NULL)
    delete signal;
}
