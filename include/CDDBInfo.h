/* CDInfo.h
 * Copyright (C) 2001,2004,2009 Dustin Graves <dgraves@computer.org>
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

#ifndef _CDDBINFO_H_
#define _CDDBINFO_H_

enum
{
  CDDB_PROTOCOL_CDDBP=0,
  CDDB_PROTOCOL_HTTP=1
};

class CDDBInfo : public CDInfo
{
public:
  class CDDBSettings
  {
  public:
    FXbool proxy;
    FXuint cddbproto;
    FXushort cddbpport;
    FXushort cddbport;
    FXushort proxyport;
    FXString cddbaddr;
    FXString proxyaddr;
    FXString cddbexec;
    FXbool localcopy;

  public:
    CDDBSettings();
    CDDBSettings(const CDDBSettings& settings);
  };

protected:
  FXuint status;
  FXString errorstring;
  CDDBSettings settings;
  CDData currentdata;

protected:
  void setStatus(FXuint current);

  void genDefaultInfo(const CDPlayer& cddesc,disc_data* info);

  FXbool getLocalInfo(const CDPlayer& cddesc,disc_data* info);

  FXbool getRemoteInfo(const CDPlayer& cddesc,disc_data* info,FXWindow* owner=NULL);

public:
  CDDBInfo();

  CDDBInfo(const CDDBInfo::CDDBSettings& s);

  FXbool getUseProxy() const;

  void setUseProxy(FXbool use);

  FXuint getCDDBProtocol() const;

  void setCDDBProtocol(FXuint proto);

  FXushort getCDDBPort() const;

  void setCDDBPort(FXushort port);

  FXushort getProxyPort() const;

  void setProxyPort(FXushort port);

  FXString getCDDBAddress() const;

  void setCDDBAddress(const FXString& addr);

  FXString getProxyAddress() const;

  void setProxyAddress(const FXString& addr);

  FXString getCDDBScript() const;

  void setCDDBScript(const FXString& script);

  FXbool getSaveLocalCopy() const;

  void setSaveLocalCopy(FXbool copy);

  FXbool getCDDBServerList(struct cddb_serverlist* list) const;

  // Get status
  virtual FXuint getStatus() const;

  // Get error string
  virtual FXString getErrorString() const;

  // Request disc info; returns true if request will be handled and false if request can not be processed
  virtual FXbool requestData(const CDPlayer& cdplayer);

  // Fill existing CDData object with disc info; returns true if data is available and false if data is unavailable 
  // due to retrieval error, etc
  virtual FXbool getData(CDData* data);

  // Reset the object's state to idle
  virtual FXbool reset();

  // Request user input through custom process defined for the object (for objects which may require user interaction)
  virtual FXbool getUserInput(FXWindow* owner = 0);
};

#endif
