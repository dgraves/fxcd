/* CDPipe.cpp
 * Copyright (C) 2004 Dustin Graves <dgraves@computer.org>
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
#else
#include <unistd.h>
#endif

//Construct an anonymous pipe
CDPipe::CDPipe()
{
#ifdef WIN32
  FXString pipename=FXStringFormat("\\\\.\\pipe\\%s%d",PROGRAM,GetCurrentProcessId());

  // Create pipe server for writing
  pipes[1]=CreateNamedPipe((LPCTSTR)pipename.text(),PIPE_ACCESS_OUTBOUND,PIPE_TYPE_BYTE|PIPE_WAIT,1,BUFSIZ,0,PIPE_TIMEOUT,NULL);

  // Create pipe client for reading
  overlap.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
  pipes[0]=CreateFile((LPCTSTR)pipename.text(),GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);

  ReadFile(pipes[0],&input,sizeof(FXint),&inputlen,&overlap);
#else
  pipe(pipes);
#endif
}

//Write data to pipe
FXbool CDPipe::write(FXint data)
{
#ifdef WIN32
#else
  return write(pipes[1],&data,sizeof(FXint))!=sizeof(FXint);
#endif
}

//Read data from pipe
FXbool CDPipe::read(FXint* data)
{
#ifdef WIN32
  if(inputlen!=4)
    return FALSE;
  *data=input;
  ReadFile(pipes[0],&input,sizeof(FXint),&inputlen,&overlap);
  return TRUE;
#else
  return read(pipes[0],data,sizeof(FXint))!=sizeof(FXint);
#endif
}

//Get waitable object for reading data
FXInputHandle CDPipe::getReadHandle()
{
#ifdef WIN32
  return (FXInputHandle)overlap.hEvent;
#else
  return (FXInputHandle)pipes[0];
#endif
}

//Destructor
CDPipe::~CDPipe()
{
#ifdef WIN32
  CancelIO(pipes[0]);
  CloseHandle(overlap.hEvent);
  CloseHandle(pipes[0]);
  CloseHandle(pipes[1]);
#else
  close(pipes[0]);
  close(pipes[1]);
#endif
}
