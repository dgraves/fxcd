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

#ifndef _CDINFO_H_
#define _CDINFO_H_

enum
{
  PROTO_CDDBP=0,
  PROTO_HTTP=1
};

class CDInfo
{
  friend class CDWindow;      //For data targets
protected:
  FXbool proxy;
  FXuint cddbproto;
  FXushort cddbpport;
  FXushort cddbport;
  FXushort proxyport;
  FXString cddbaddr;
  FXString proxyaddr;
  FXString cddbexec;

public:
  CDInfo();

  void defaultInfo(const CDPlayer& cddesc,disc_data* info);
  FXbool getLocalInfo(const CDPlayer& cddesc,disc_data* info);
  FXbool getRemoteInfo(const CDPlayer& cddesc,disc_data* info,FXWindow* owner=NULL);

  FXbool getProxy() const;
  void setProxy(FXbool use);

  FXuint getCDDBProtocol() const;
  void setCDDBProtocol(FXuint proto);

  FXushort getCDDBPort() const;
  void setCDDBPort(FXushort port);

  FXushort getCDIndexPort() const;
  void setCDIndexPort(FXushort port);

  FXushort getProxyPort() const;
  void setProxyPort(FXushort port);

  FXString getCDDBAddress() const;
  void setCDDBAddress(const FXString& addr);

  FXString getProxyAddress() const;
  void setProxyAddress(const FXString& addr);

  FXString getCDDBScript() const;
  void setCDDBScript(const FXString& script);

  FXbool getCDDBServerList(struct cddb_serverlist* list) const;
};

#endif
