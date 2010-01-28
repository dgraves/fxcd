/* CDDBInfo.h
 * Copyright (C) 2001,2009-2010 Dustin Graves <dgraves@computer.org>
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

#include <string.h>
#include <cdlyte.h>
#include <fox-1.6/fx.h>
#include <fox-1.6/FXArray.h>
#include <fox-1.6/FXElement.h>
#include "CDDefs.h"
#include "CDPlayer.h"
#include "CDChoiceDialog.h"
#include "CDData.h"
#include "CDInfo.h"
#include "CDDBInfo.h"

CDDBInfo::CDDBSettings::CDDBSettings()
  : proxy(FALSE),
    cddbproto(CDDB_PROTOCOL_HTTP),
    cddbpport(CDDBP_DEFAULT_PORT),
    cddbport(HTTP_DEFAULT_PORT),
    proxyport(0),
    cddbaddr("freedb.freedb.org"),
    proxyaddr("0.0.0.0"),
    cddbexec(CDDB_HTTP_QUERY_CGI),
    localcopy(FALSE)
{
}

CDDBInfo::CDDBSettings::CDDBSettings(const CDDBSettings& settings)
  : proxy(settings.proxy),
    cddbproto(settings.cddbproto),
    cddbpport(settings.cddbpport),
    cddbport(settings.cddbport),
    proxyport(settings.proxyport),
    cddbaddr(settings.cddbaddr),
    proxyaddr(settings.proxyaddr),
    cddbexec(settings.cddbexec),
    localcopy(settings.localcopy)
{
}

CDDBInfo::CDDBInfo(FXGUISignal* guisignal)
  : initialized(FALSE),
    choice(0),
    discid(0),
    status(CDINFO_IDLE),
    errorstring("No error"),
    signal(guisignal)
{
}

CDDBInfo::CDDBInfo(const CDDBInfo::CDDBSettings& s,FXGUISignal* guisignal)
  : initialized(FALSE),
    choice(0),
    discid(0),
    status(CDINFO_IDLE),
    errorstring("No error"),
    settings(s),
    signal(guisignal)
{
}

void CDDBInfo::init(const CDPlayer& cdplayer)
{
  // Initialize current data structure
  disc_data data;
  cddb_init_disc_data(&data);
  genDefaultInfo(cdplayer,&data);
  fillCurrentData(data);
  cddb_free_disc_data(&data);

  // Initialize query string for remote info request if local copy disabled or not found
  FXint len=BUFSIZ;
  char buffer[BUFSIZ];
  querystring=cddb_query_string(cdplayer.getDescriptor(),buffer,&len);

  // Initialize the discid and disc_info values when working with local copies
  if(settings.localcopy)
  {
    discid=cddb_discid(cdplayer.getDescriptor());
    cd_init_disc_info(&discinfo);
    cd_stat(cdplayer.getDescriptor(),&discinfo);
  }
 
  initialized=TRUE;
}

void CDDBInfo::setStatus(FXuint current)
{
  status = current;
}

// Get status
FXuint CDDBInfo::getStatus() const
{
  return status;
}

// Get error string
FXString CDDBInfo::getErrorString() const
{
  return errorstring;
}

void CDDBInfo::fillCurrentData(const disc_data& info)
{
  currentdata.artist = info.data_artist;
  currentdata.genre = info.data_genre;
  currentdata.title = info.data_title;
  currentdata.year = info.data_year;

  currentdata.trackArtist.clear();
  currentdata.trackTitle.clear();
  for(int i = 0; i < info.data_total_tracks; i++)
  {
    currentdata.trackArtist.append(info.data_track[i].track_artist);
    currentdata.trackTitle.append(info.data_track[i].track_title);
  }
}

// Request disc info; returns true if request will be handled and false if request can not be processed
FXbool CDDBInfo::requestData()
{
  FXbool success=TRUE;

  if(!initialized)
  {
    setStatus(CDINFO_ERROR);
    success=FALSE;
    errorstring="Not initialized";
  }
  else
  {
    FXbool foundlocal=FALSE;
    disc_data data;

    setStatus(CDINFO_PENDING);

    cddb_init_disc_data(&data);
    if(settings.localcopy)
    {
      if(getLocalInfo(&data))
        foundlocal=TRUE;
    }

    if(!foundlocal&&!getRemoteInfo(&data))
      success=FALSE;

    fillCurrentData(data);
    cddb_free_disc_data(&data);

    setStatus(success ? CDINFO_DONE : CDINFO_ERROR);
  }

  // Signal completion to main thread
  if(signal!=NULL)
    signal->signal();

  return success;
}

// Fill existing CDData object with disc info; returns true if data is available and false if data is unavailable 
// due to retrieval error, etc
FXbool CDDBInfo::getData(CDData* data)
{
  if(getStatus()!=CDINFO_DONE)
  {
    return FALSE;
  }

  // Fill CDData object
  *data = currentdata;
  return TRUE;
}

// Reset the object's state to idle
FXbool CDDBInfo::reset()
{
  setStatus(CDINFO_IDLE);
  return TRUE;
}

// Request user input through custom process defined for the object (for objects which may require user interaction)
FXbool CDDBInfo::getUserInput(FXWindow* owner)
{
  if(getStatus()!=CDINFO_INTERACT)
  {
    return FALSE;
  }

  // Show dialog for user input
  CDChoiceDialog dialog(owner,&query);
  if(dialog.execute(PLACEMENT_OWNER))
    choice=dialog.getSelection();

  // Signal thread to continue
  condition.signal();

  return TRUE;
}

void CDDBInfo::genDefaultInfo(const CDPlayer& cddesc,disc_data* info)
{
  cddb_gen_unknown_entry(cddesc.getDescriptor(),info);
}

FXbool CDDBInfo::getLocalInfo(disc_data* info)
{
  FXString localpath=FXStringFormat("%s%c%s",FXSystem::getHomeDirectory(),PATHSEP,".cddb");
  if(cddb_read_local(localpath.text(),discid,info)<0)
  {
    return FALSE;
  }
  return TRUE;
}

FXbool CDDBInfo::getRemoteInfo(disc_data* info)
{
  cdsock_t sock;
  char http_string[512];
  int http_string_len=sizeof(http_string);
  struct cddb_host host;
  struct cddb_hello hello;
  struct cddb_server server;
  struct cddb_server *pserver=NULL;
  FXbool success=FALSE;

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

  sock=cddb_connect(&host,pserver,&hello,http_string,&http_string_len);

  if(sock!=-1)
  {
    if(!querystring.empty())
    {
      cddb_init_cddb_query(&query);
      FXint result=cddb_query(querystring.text(),sock,(settings.cddbproto==CDDB_PROTOCOL_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP,&query,http_string);
      if(result!=-1&&query.query_match!=QUERY_NOMATCH)
      {
        choice=0;
        if(query.query_matches>1&&signal!=NULL)
        {
          setStatus(CDINFO_INTERACT);

          // Signal to main thread that input is requested
          signal->signal();

          // Wait for GUI operation to complete
          FXMutex mtx;
          mtx.lock();
          condition.wait(mtx);
        }

        if(settings.cddbproto==CDDB_PROTOCOL_HTTP)
        {
          cddb_quit(sock,CDDB_MODE_HTTP);
          sock=cddb_connect(&host,pserver,&hello,http_string,&http_string_len);
        }

        if(sock!=-1) result=cddb_read(query.query_list[choice].list_category,query.query_list[choice].list_id,sock,(settings.cddbproto==CDDB_PROTOCOL_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP,info,http_string);
        else result=-1;

        cddb_quit(sock,(settings.cddbproto==CDDB_PROTOCOL_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP);

        if(result!=-1)
        {
          success = TRUE;

          if(settings.localcopy)
          {
            // Save entry in home directory
            FXString localpath=FXStringFormat("%s%c%s",FXSystem::getHomeDirectory(),PATHSEP,".cddb");

            // Create CDDB direcotry if it does not exist
            if(!FXStat::exists(localpath))
              FXDir::create(localpath);

            //Write to the local database
            cddb_write_local(localpath.text(),&hello,&discinfo,info,"Generated by fxcd");
          }
        }
      }
      cddb_free_cddb_query(&query);
    }
  }

  if(pserver) cddb_free_cddb_server(&server);
  cddb_free_cddb_host(&host);
  cddb_free_cddb_hello(&hello);

  return success;
}

FXbool CDDBInfo::getUseProxy() const
{
  return settings.proxy;
}

void CDDBInfo::setUseProxy(FXbool use)
{
  settings.proxy=use;
}

FXuint CDDBInfo::getCDDBProtocol() const
{
  return settings.cddbproto;
}

void CDDBInfo::setCDDBProtocol(FXuint proto)
{
  settings.cddbproto=proto;
}

FXushort CDDBInfo::getCDDBPort() const
{
  return (settings.cddbproto==CDDB_PROTOCOL_CDDBP)?settings.cddbpport:settings.cddbport;
}

void CDDBInfo::setCDDBPort(FXushort port)
{
  if(settings.cddbproto==CDDB_PROTOCOL_CDDBP) settings.cddbpport=port;
  else settings.cddbport=port;
}

FXushort CDDBInfo::getProxyPort() const
{
  return settings.proxyport;
}

void CDDBInfo::setProxyPort(FXushort port)
{
  settings.proxyport=port;
}

FXString CDDBInfo::getCDDBAddress() const
{
  return settings.cddbaddr;
}

void CDDBInfo::setCDDBAddress(const FXString& addr)
{
  settings.cddbaddr=addr;
}

FXString CDDBInfo::getProxyAddress() const
{
  return settings.proxyaddr;
}

void CDDBInfo::setProxyAddress(const FXString& addr)
{
  settings.proxyaddr=addr;
}

FXString CDDBInfo::getCDDBScript() const
{
  return settings.cddbexec;
}

void CDDBInfo::setCDDBScript(const FXString& script)
{
  settings.cddbexec=script;
}

FXbool CDDBInfo::getSaveLocalCopy() const
{
  return settings.localcopy;
}

void CDDBInfo::setSaveLocalCopy(FXbool copy)
{
  settings.localcopy = copy;
}

FXbool CDDBInfo::getCDDBServerList(struct cddb_serverlist* list) const
{
  int sock;
  char http_string[512];
  int http_string_len=sizeof(http_string);
  struct cddb_host host;
  struct cddb_hello hello;
  struct cddb_server server;
  struct cddb_server *pserver=NULL;

  printf("Checking %s for disc info: ",settings.cddbaddr.text());

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

  sock=cddb_connect(&host,pserver,&hello,http_string,&http_string_len);

  if(pserver) cddb_free_cddb_server(&server);
  cddb_free_cddb_host(&host);
  cddb_free_cddb_hello(&hello);

  if(sock!=-1)
  {
    FXint result=cddb_sites(sock,(settings.cddbproto==CDDB_PROTOCOL_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP,list,http_string);
    if(result!=-1)
      return TRUE;
  }

  return FALSE;
}


// Get FXGUISignal object used to synchronize threaded info retrieval
FXGUISignal* CDDBInfo::getGUISignal() const
{
  return signal;
}

CDDBInfo::~CDDBInfo()
{
  if(initialized&&settings.localcopy)
    cd_free_disc_info(&discinfo);
}
