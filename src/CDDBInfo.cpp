/* CDDBInfo.h
 * Copyright (C) 2001,2009 Dustin Graves <dgraves@computer.org>
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

CDDBInfo::CDDBInfo()
  : status(CDINFO_IDLE),
    errorstring("No error"),
    proxy(FALSE),
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

// Request disc info; returns true if request will be handled and false if request can not be processed
FXbool CDDBInfo::requestData(const CDPlayer& cdplayer)
{
  FXbool success=TRUE;
  disc_data info;

  setStatus(CDINFO_PENDING);

  cddb_init_disc_data(&info);
  if(!getLocalInfo(cdplayer,&info))
  {
    if(!getRemoteInfo(cdplayer,&info))
    {
      genDefaultInfo(cdplayer,&info);
      success=FALSE;
    }
  }

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

  setStatus(success ? CDINFO_DONE : CDINFO_ERROR);

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

  return TRUE;
}

void CDDBInfo::genDefaultInfo(const CDPlayer& cddesc,disc_data* info)
{
  cddb_gen_unknown_entry(cddesc.getDescriptor(),info);
}

FXbool CDDBInfo::getLocalInfo(const CDPlayer& cddesc,disc_data* info)
{
  FXString localpath=FXStringFormat("%s%c%s",FXSystem::getHomeDirectory(),PATHSEP,".cddb");
  if(cddb_read_local(localpath.text(),cddesc.getDescriptor(),info)<0)
  {
    return FALSE;
  }
  return TRUE;
}

FXbool CDDBInfo::getRemoteInfo(const CDPlayer& cddesc,disc_data* info,FXWindow* owner)
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
  host.host_server.server_name=cddb_strdup(cddbaddr.text());
  host.host_server.server_port=(cddbproto==CDDB_PROTOCOL_CDDBP)?cddbpport:cddbport;
  host.host_protocol=(cddbproto==CDDB_PROTOCOL_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP;
  if(cddbproto==CDDB_PROTOCOL_HTTP) host.host_addressing=cddb_strdup(cddbexec.text());

  if(proxy)
  {
    cddb_init_cddb_server(&server);
    server.server_name=cddb_strdup(proxyaddr.text());
    server.server_port=proxyport;
    pserver=&server;
  }

  sock=cddb_connect(&host,pserver,&hello,http_string,&http_string_len);

  if(sock!=-1)
  {
    FXint result,len=BUFSIZ;
    char query_string[BUFSIZ];
    struct cddb_query query;
    cddb_init_cddb_query(&query);
    result=cddb_query(cddb_query_string(cddesc.getDescriptor(),query_string,&len),sock,(cddbproto==CDDB_PROTOCOL_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP,&query,http_string);
    if(result!=-1&&query.query_match!=QUERY_NOMATCH)
    {
      FXint choice=0;
      if(query.query_matches>1&&owner!=NULL)
      {
        CDChoiceDialog dialog(owner,&query);
        if(dialog.execute())
	  choice=dialog.getSelection();
      }

      if(cddbproto==CDDB_PROTOCOL_HTTP)
      {
        cddb_quit(sock,CDDB_MODE_HTTP);
        sock=cddb_connect(&host,pserver,&hello,http_string,&http_string_len);
      }

      if(sock!=-1) result=cddb_read(query.query_list[choice].list_category,query.query_list[choice].list_id,sock,(cddbproto==CDDB_PROTOCOL_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP,info,http_string);
      else result=-1;

      cddb_quit(sock,(cddbproto==CDDB_PROTOCOL_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP);

      if(result!=-1)
      {
        success = TRUE;

        if(localcopy)
        {
          struct disc_info di;

          // Save entry in home directory
          FXString localpath=FXStringFormat("%s%c%s",FXSystem::getHomeDirectory(),PATHSEP,".cddb");

          cd_init_disc_info(&di);
          cd_stat(cddesc.getDescriptor(),&di);

          //Write to the local database
          cddb_write_local(localpath.text(),&hello,&di,info,"Generated by fxcd");
          cd_free_disc_info(&di);
        }
      }
    }
    cddb_free_cddb_query(&query);
  }

  if(pserver) cddb_free_cddb_server(&server);
  cddb_free_cddb_host(&host);
  cddb_free_cddb_hello(&hello);

  return success;
}

FXbool CDDBInfo::getUseProxy() const
{
  return proxy;
}

void CDDBInfo::setUseProxy(FXbool use)
{
  proxy=use;
}

FXuint CDDBInfo::getCDDBProtocol() const
{
  return cddbproto;
}

void CDDBInfo::setCDDBProtocol(FXuint proto)
{
  cddbproto=proto;
}

FXushort CDDBInfo::getCDDBPort() const
{
  return (cddbproto==CDDB_PROTOCOL_CDDBP)?cddbpport:cddbport;
}

void CDDBInfo::setCDDBPort(FXushort port)
{
  if(cddbproto==CDDB_PROTOCOL_CDDBP) cddbpport=port;
  else cddbport=port;
}

FXushort CDDBInfo::getProxyPort() const
{
  return proxyport;
}

void CDDBInfo::setProxyPort(FXushort port)
{
  proxyport=port;
}

FXString CDDBInfo::getCDDBAddress() const
{
  return cddbaddr;
}

void CDDBInfo::setCDDBAddress(const FXString& addr)
{
  cddbaddr=addr;
}

FXString CDDBInfo::getProxyAddress() const
{
  return proxyaddr;
}

void CDDBInfo::setProxyAddress(const FXString& addr)
{
  proxyaddr=addr;
}

FXString CDDBInfo::getCDDBScript() const
{
  return cddbexec;
}

void CDDBInfo::setCDDBScript(const FXString& script)
{
  cddbexec=script;
}

FXbool CDDBInfo::getSaveLocalCopy() const
{
  return localcopy;
}

void CDDBInfo::setSaveLocalCopy(FXbool copy)
{
  localcopy = copy;
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

  printf("Checking %s for disc info: ",cddbaddr.text());

  cddb_init_cddb_hello(&hello);
  hello.hello_program=cddb_strdup(PROG_PACKAGE);
  hello.hello_version=cddb_strdup(PROG_VERSION);

  cddb_init_cddb_host(&host);
  host.host_server.server_name=cddb_strdup(cddbaddr.text());
  host.host_server.server_port=(cddbproto==CDDB_PROTOCOL_CDDBP)?cddbpport:cddbport;
  host.host_protocol=(cddbproto==CDDB_PROTOCOL_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP;
  if(cddbproto==CDDB_PROTOCOL_HTTP) host.host_addressing=cddb_strdup(cddbexec.text());

  if(proxy)
  {
    cddb_init_cddb_server(&server);
    server.server_name=cddb_strdup(proxyaddr.text());
    server.server_port=proxyport;
    pserver=&server;
  }

  sock=cddb_connect(&host,pserver,&hello,http_string,&http_string_len);

  if(pserver) cddb_free_cddb_server(&server);
  cddb_free_cddb_host(&host);
  cddb_free_cddb_hello(&hello);

  if(sock!=-1)
  {
    FXint result=cddb_sites(sock,(cddbproto==CDDB_PROTOCOL_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP,list,http_string);
    if(result!=-1)
      return TRUE;
  }

  return FALSE;
}
