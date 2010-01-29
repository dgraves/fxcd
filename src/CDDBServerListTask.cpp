/* CDDBServerListTask.cpp
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
#include <fox-1.6/FXArray.h>
#include <fox-1.6/FXElement.h>
#include "CDdefs.h"
#include "CDDBSettings.h"
#include "CDDBServerListTask.h"

CDDBServerListTask::CDDBServerListTask(const CDDBSettings& cddbsettings,struct cddb_serverlist* serverlist,FXGUISignal* guisignal)
: pserver(NULL),
  signal(guisignal),
  list(serverlist),
  settings(cddbsettings)
{
  init();
}

void CDDBServerListTask::init()
{
  // Allocate CDDB resources here, to be deallocated in destructor, so that 
  // thread can be canceled at any time without leaking memory
  cddb_init_cddb_hello(&hello);
  hello.hello_program=cddb_strdup(PROG_PACKAGE);
  hello.hello_version=cddb_strdup(PROG_VERSION);
  hello.hello_user=cddb_strdup("anonymous");
  hello.hello_hostname=cddb_strdup("anonymous");

  cddb_init_cddb_host(&host);
  host.host_server.server_name=cddb_strdup(settings.cddbaddr.text());
  host.host_server.server_port=(settings.cddbproto==CDDB_PROTOCOL_CDDBP)?settings.cddbpport:settings.cddbport;
  host.host_protocol=(settings.cddbproto==CDDB_PROTOCOL_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP;
  if(settings.cddbproto==CDDB_PROTOCOL_HTTP) host.host_addressing=cddb_strdup(settings.cddbexec.text());

  if(settings.proxy)
  {
    cddb_init_cddb_server(&server);
    server.server_name=cddb_strdup(settings.proxyaddr.text());
    server.server_port=settings.proxyport;
    pserver=&server;
  }
}

FXint CDDBServerListTask::run()
{
  int sock;
  char http_string[512];
  int http_string_len=sizeof(http_string);

  sock=cddb_connect(&host,pserver,&hello,http_string,&http_string_len);

  if(sock!=-1)
  {
    FXint result=cddb_sites(sock,(settings.cddbproto==CDDB_PROTOCOL_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP,list,http_string);
    if(result!=-1)
      signal->setData(list);
    else
      signal->setData(NULL);
  }

  if(signal!=NULL)
    signal->signal();

  return 1;
}

CDDBServerListTask::~CDDBServerListTask()
{
  if(pserver) cddb_free_cddb_server(&server);
  cddb_free_cddb_host(&host);
  cddb_free_cddb_hello(&hello);
}
