/* CDInfo.h
 * Copyright (C) 2001,2004,2009-2010 Dustin Graves <dgraves@computer.org>
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
    FXbool promptmultiple;  // Prompt for user selection when multiple matches exist
    FXbool localcopy;
  public:
    CDDBSettings();
    CDDBSettings(const CDDBSettings& settings);
  };
private:
  FXbool initialized;
  FXuint choice;                 // Choice made by getUsetInput for getRemoteInfo
  FXString querystring;          // String to be used for query
  unsigned long discid;          // Disc ID to be initialized by init for use by getLocalInfo
  struct cddb_query query;       // Query to be shared with getUserInput by getRemoteInfo
  struct disc_info discinfo;     // Disc info to be initialized by init for use by getRemoteInfo
private:
  // Thread synchronization mechanisms
  FXCondition condition;
  FXGUISignal* signal;
private:
  void genDefaultInfo(const CDPlayer& cddesc,disc_data* info);
  FXbool getLocalInfo(disc_data* info);
  FXbool getRemoteInfo(disc_data* info);
protected:
  FXuint status;
  FXString errorstring;
  CDDBSettings settings;
  CDData currentdata;
protected:
  void setStatus(FXuint current);
  void fillCurrentData(const disc_data& info);
public:
  CDDBInfo(FXGUISignal* guisignal=NULL);

  CDDBInfo(const CDDBInfo::CDDBSettings& s,FXGUISignal* guisignal=NULL);

  void init(const CDPlayer& cdplayer);

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
  virtual FXbool requestData();

  // Fill existing CDData object with disc info; returns true if data is available and false if data is unavailable 
  // due to retrieval error, etc
  virtual FXbool getData(CDData* data);

  // Reset the object's state to idle
  virtual FXbool reset();

  // Request user input through custom process defined for the object (for objects which may require user interaction)
  virtual FXbool getUserInput(FXWindow* owner = 0);

  // Get FXGUISignal object used to synchronize threaded info retrieval
  virtual FXGUISignal* getGUISignal() const;

  // Destructor
  ~CDDBInfo();
};

#endif
