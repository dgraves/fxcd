/* CDDBSettings.cpp
 * Copyright (C) 2009-2010 Dustin Graves <dgraves@computer.org>
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

CDDBSettings::CDDBSettings()
  : proxy(FALSE),
    cddbproto(CDDB_PROTOCOL_HTTP),
    cddbpport(CDDBP_DEFAULT_PORT),
    cddbport(HTTP_DEFAULT_PORT),
    proxyport(0),
    cddbaddr("freedb.freedb.org"),
    proxyaddr("0.0.0.0"),
    cddbexec(CDDB_HTTP_QUERY_CGI),
    promptmultiple(FALSE),
    localcopy(FALSE)
{
}

CDDBSettings::CDDBSettings(const CDDBSettings& settings)
  : proxy(settings.proxy),
    cddbproto(settings.cddbproto),
    cddbpport(settings.cddbpport),
    cddbport(settings.cddbport),
    proxyport(settings.proxyport),
    cddbaddr(settings.cddbaddr),
    proxyaddr(settings.proxyaddr),
    cddbexec(settings.cddbexec),
    promptmultiple(settings.promptmultiple),
    localcopy(settings.localcopy)
{
}

CDDBSettings& CDDBSettings::operator=(const CDDBSettings& rhs)
{
  proxy=rhs.proxy;
  cddbproto=rhs.cddbproto;
  cddbpport=rhs.cddbpport;
  cddbport=rhs.cddbport;
  proxyport=rhs.proxyport;
  cddbaddr=rhs.cddbaddr;
  proxyaddr=rhs.proxyaddr;
  cddbexec=rhs.cddbexec;
  promptmultiple=rhs.promptmultiple;
  localcopy=rhs.localcopy;

  return *this;
}

bool CDDBSettings::operator==(const CDDBSettings& rhs)
{
  return (proxy==rhs.proxy&&
          cddbproto==rhs.cddbproto&&
          cddbpport==rhs.cddbpport&&
          cddbport==rhs.cddbport&&
          proxyport==rhs.proxyport&&
          cddbaddr==rhs.cddbaddr&&
          proxyaddr==rhs.proxyaddr&&
          cddbexec==rhs.cddbexec&&
          promptmultiple==rhs.promptmultiple&&
          localcopy==rhs.localcopy);
}
