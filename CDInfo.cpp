/* CDInfo.h
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

#include <string.h>
#include <unistd.h>
extern "C" {
#include "cdaudio.h"
}
#include "fox/fx.h"
#include "fox/FXArray.h"
#include "fox/FXElement.h"
#include "CDPlayer.h"
#include "CDChoiceDialog.h"
#include "CDInfo.h"

CDInfo::CDInfo()
  : proxy(FALSE),
    cddbproto(PROTO_HTTP),
    //cddbpport(CDDBP_DEFAULT_PORT),
    cddbpport(8880),
    cddbport(HTTP_DEFAULT_PORT),
    cdindexport(HTTP_DEFAULT_PORT),
    proxyport(0),
    cddbaddr("www.freedb.org"),
    cdindexaddr("www.cdindex.org"),
    proxyaddr("0.0.0.0"),
    cddbexec("cgi-bin/cddb.cgi"),
    cdindexexec("cgi-bin/cdi/get.pl")
{
}

void CDInfo::defaultSettings(const CDPlayer& cddesc,disc_data* info)
{
  cddb_generate_unknown_entry(cddesc.getDescriptor(),info);

  FXint i,first=cddesc.getStartTrack(),total=cddesc.getNumTracks();
  strncpy(info->data_title,"Unknown Album",sizeof(info->data_title));
  strncpy(info->data_artist,"Unknown Artist",sizeof(info->data_artist));
  for(i=0;i<total;i++)
    sprintf(info->data_track[i].track_name,"Track %d",first++);
}

FXbool CDInfo::getLocalCDDBInfo(const CDPlayer& cddesc,disc_data* info)
{
  struct cddb_entry entry;
  printf("Checking ~/.cddb for disc info: ");
  cddb_stat_disc_data(cddesc.getDescriptor(),&entry);
  if(entry.entry_present)
  {
    printf("yes\n");
    cddb_read_disc_data(cddesc.getDescriptor(),info);
    return TRUE;
  }
  printf("no\n");

  defaultSettings(cddesc,info);
  return FALSE;
}

FXbool CDInfo::getRemoteCDDBInfo(const CDPlayer& cddesc,disc_data* info,FXWindow* owner)
{
  int sock;
  char http_string[512];
  struct cddb_entry entry;
  struct cddb_host host;
  struct cddb_hello hello;
  struct cddb_query query;
  struct cddb_server server;
  struct cddb_server *pserver=NULL;

  printf("Checking %s for disc info: ",cddbaddr.text());

  strncpy(hello.hello_program,"fxcd",sizeof(hello.hello_program));
  strncpy(hello.hello_version,"1.0.0",sizeof(hello.hello_version));

  strncpy(host.host_server.server_name,cddbaddr.text(),sizeof(host.host_server.server_name));
  host.host_server.server_port=(cddbproto==PROTO_CDDBP)?cddbpport:cddbport;
  host.host_protocol=(cddbproto==PROTO_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP;
  if(cddbproto==PROTO_HTTP) strncpy(host.host_addressing,cddbexec.text(),sizeof(host.host_addressing));

  if(proxy)
  {
    strncpy(server.server_name,proxyaddr.text(),sizeof(server.server_name));
    server.server_port=proxyport;
    pserver=&server;
  }

  sock=cddb_connect_server(host,pserver,hello,http_string,sizeof(http_string));

  if(sock!=-1)
  {
    FXint result=cddb_query(cddesc.getDescriptor(),sock,(cddbproto==PROTO_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP,&query,http_string);
    if(result!=-1&&query.query_match!=QUERY_NOMATCH)
    {
      FXint choice=0;
      if(query.query_matches>1&&owner!=NULL)
      {
        CDChoiceDialog dialog(owner,&query);
        if(dialog.execute())
	  choice=dialog.getSelection();
      }

      entry.entry_genre=query.query_list[choice].list_genre;
      entry.entry_id=query.query_list[choice].list_id;

      if(cddbproto==PROTO_HTTP)
      {
	close(sock);
        sock=cddb_connect_server(host,pserver,hello,http_string,sizeof(http_string));
      }

      if(sock!=-1) result=cddb_read(cddesc.getDescriptor(),sock,(cddbproto==PROTO_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP,entry,info,http_string);
      else result=-1;

      if(cddbproto==PROTO_CDDBP) cddb_quit(sock);
      else close(sock);

      if(result!=-1)
      {
        printf("yes.  Saving to ~/.cddb\n");

        //Write to the local database
        cddb_write_data(cddesc.getDescriptor(),info);

        return TRUE;
      }
    }
  }
  printf("no\n");

  defaultSettings(cddesc,info);
  return FALSE;
}

FXbool CDInfo::getCDIndexInfo(const CDPlayer& cddesc,disc_data* info)
{
  int sock;
  char http_string[512];
  struct cddb_host host;
  struct cddb_hello hello;
  struct cddb_server server;
  struct cddb_server *pserver=NULL;

  printf("Checking %s for disc info: ",cdindexaddr.text());

  strncpy(host.host_server.server_name,cdindexaddr.text(),256);
  strncpy(host.host_addressing,cdindexexec.text(),256);
  host.host_server.server_port=cdindexport;
  host.host_protocol = CDINDEX_MODE_HTTP;

  if(proxy)
  {
    strncpy(server.server_name,proxyaddr.text(),sizeof(server.server_name));
    server.server_port=proxyport;
    pserver=&server;
  }

  sock=cdindex_connect_server(host,pserver,http_string,sizeof(http_string));
  if(sock!=-1)
  {
    int result=cdindex_read(cddesc.getDescriptor(),sock,info,http_string);
    close(sock);

    if(result!=-1)
    {
      printf("yes.  Writing to ~/.cddb\n");

      //Write to the local database
      cdindex_write_data(cddesc.getDescriptor(),info);

      return TRUE;
    }
  }
  printf("no\n");

  defaultSettings(cddesc,info);
  return FALSE;
}

FXbool CDInfo::getProxy() const
{
  return proxy;
}

void CDInfo::setProxy(FXbool use)
{
  proxy=use;
}

FXuint CDInfo::getCDDBProtocol() const
{
  return cddbproto;
}

void CDInfo::setCDDBProtocol(FXuint proto)
{
  cddbproto=proto;
}

FXushort CDInfo::getCDDBPort() const
{
  return (cddbproto==PROTO_CDDBP)?cddbpport:cddbport;
}

void CDInfo::setCDDBPort(FXushort port)
{
  if(cddbproto==PROTO_CDDBP) cddbpport=port;
  else cddbport=port;
}

FXushort CDInfo::getCDIndexPort() const
{
  return cdindexport;
}

void CDInfo::setCDIndexPort(FXushort port)
{
  cdindexport=port;
}

FXushort CDInfo::getProxyPort() const
{
  return proxyport;
}

void CDInfo::setProxyPort(FXushort port)
{
  proxyport=port;
}

FXString CDInfo::getCDDBAddress() const
{
  return cddbaddr;
}

void CDInfo::setCDDBAddress(const FXString& addr)
{
  cddbaddr=addr;
}

FXString CDInfo::getCDIndexAddress() const
{
  return cdindexaddr;
}

void CDInfo::setCDIndexAddress(const FXString& addr)
{
  cdindexaddr=addr;
}

FXString CDInfo::getProxyAddress() const
{
  return proxyaddr;
}

void CDInfo::setProxyAddress(const FXString& addr)
{
  proxyaddr=addr;
}

FXString CDInfo::getCDDBScript() const
{
  return cddbexec;
}

void CDInfo::setCDDBScript(const FXString& script)
{
  cddbexec=script;
}

FXString CDInfo::getCDIndexScript() const
{
  return cdindexexec;
}

void CDInfo::setCDIndexScript(const FXString& script)
{
  cdindexexec=script;
}

FXbool CDInfo::getCDDBServerList(struct cddb_serverlist* list) const
{
  int sock;
  char http_string[512];
  struct cddb_host host;
  struct cddb_hello hello;
  struct cddb_server server;
  struct cddb_server *pserver=NULL;

  printf("Checking %s for disc info: ",cddbaddr.text());

  strncpy(hello.hello_program,"fxcd",sizeof(hello.hello_program));
  strncpy(hello.hello_version,"1.0.0",sizeof(hello.hello_version));

  strncpy(host.host_server.server_name,cddbaddr.text(),sizeof(host.host_server.server_name));
  host.host_server.server_port=(cddbproto==PROTO_CDDBP)?cddbpport:cddbport;
  host.host_protocol=(cddbproto==PROTO_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP;
  if(cddbproto==PROTO_HTTP) strncpy(host.host_addressing,cddbexec.text(),sizeof(host.host_addressing));

  if(proxy)
  {
    strncpy(server.server_name,proxyaddr.text(),sizeof(server.server_name));
    server.server_port=proxyport;
    pserver=&server;
  }

  sock=cddb_connect_server(host,pserver,hello,http_string,sizeof(http_string));

  if(sock!=-1)
  {
    FXint result=cddb_sites(sock,(cddbproto==PROTO_CDDBP)?CDDB_MODE_CDDBP:CDDB_MODE_HTTP,list,http_string);
    if(result!=-1)
      return TRUE;
  }
  
  return FALSE;
}
