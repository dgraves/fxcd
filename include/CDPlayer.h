/* CDPlayer.h
 * Copyright (C) 2001,2004 Dustin Graves <dgraves@computer.org>
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

#ifndef _CDPLAYER_H_
#define _CDPLAYER_H_

#include <vector>

enum
{
  CDREPEAT_NONE=0,
  CDREPEAT_TRACK=1,
  CDREPEAT_DISC=2
};

class CDPlayer
{
  friend class CDWindow;            // For data targets
  friend class CDPrefsBox;          // For data targets
protected:
  cddesc_t media;                   // CD-ROM handle
  FXbool open;                      // Drive door status (open/closed)
  FXbool nodisc;                    // Indicates known presence of disc in cdrom
  FXbool audiodisc;                 // Is disc audio or data
  FXbool stopped;                   // Was a stop requested (or did the disc finish)
  FXbool mute;                      // Is muted
  FXuint repeatMode;                // No repeat, repeat track, repeat disc
  FXbool intro;                     // Intro mode is active
  FXbool random;                    // Shuffle play is active
  FXint currentTrack;               // Track currently playing (or to play)
  FXint volLevel;                   // Volume level (0-100%)
  FXfloat volBalance;               // Left-right volume balance
  struct disc_timeval introTime;    // Length of time for intro play
  struct disc_volume volCurrent;    // Use to test for volume change by other party
  struct disc_info discInfo;        // Info for current disc
  std::vector<FXint> randomArray;   // List of random tracks
protected:
  void load();
  void polldisc();
  FXbool setvol();
  FXbool checkvol();
  void makeRandomList();
  FXint getRandomTrack();
public:
  /// Constructor
  CDPlayer();

  /// Initialize CD audio device
  FXbool init(const FXString& device);

  /// Close CD audio device
  FXbool finish();

  /// Issue play command to CD audio device
  FXbool play();

  /// Issue pause command to CD audio device
  FXbool pause();

  /// Issue resume command to CD audio device
  FXbool resume();

   /// Issue stop command to CD audio device
  FXbool stop();

  /// Issue skip to next track command to CD audio device
  FXbool skipNext();

  /// Issue skip to previous track command to CD audio device
  FXbool skipPrev();

  /// Issue seek forward command to CD audio device
  FXbool seekForward(FXuint seconds);

  /// Issue seek reverse command to CD audio device
  FXbool seekReverse(FXuint seconds);

   /// Issue play command, to start at specific time, to CD audio device
  FXbool playTrackPos(FXint track,const struct disc_timeval* time);

  /// Issue tray open command to CD audio device
  FXbool openTray();

  /// Issue tray close command to CD audio device
  FXbool closeTray();

  /// Poll CD audio device for current state
  FXbool update();

  // Get current cd-rom status (play,pause,finished,etc)
  FXint getStatus() const;

  /// Get CD audio descriptor
  FXint getDescriptor() const;

  /// Check for valid CD audio device
  FXbool isValid() const;

  /// Check for open CD audio device tray
  FXbool isTrayOpen() const;

  /// Check for disc in tray
  FXbool isDiscPresent() const;

  /// Check if disc in CD audio device is audio CD or data CD
  FXbool isAudioDisc() const;

  /// Get index for start track of disc
  FXint getStartTrack() const;

  /// Get total number of tracks present on disc
  FXint getNumTracks() const;

  /// Retrieve length of disc
  void getDiscLength(struct disc_timeval& dtv) const;

  /// Retrieve length of specified track
  void getTrackLength(FXint track, struct disc_timeval& dtv) const;

  /// Retrieve current time position for the currently playing disc
  void getDiscTime(struct disc_timeval& dtv) const;

  /// Retrieve current time position for the currently playing track
  void getTrackTime(struct disc_timeval& dtv) const;

  /// Retrieve info for current disc
  void getDiscInfo(struct disc_info& info) const;

  /// Retrieve info for specified track
  void getTrackInfo(FXint track, struct track_info& info) const;

  /// Get current track
  FXint getCurrentTrack() const;

  /// Set current track
  void setCurrentTrack(FXint track);

  /// Get repeat mode status
  FXuint getRepeatMode() const;

  /// Set repeat mode status
  void setRepeatMode(FXuint mode);

  /// Get intro mode status
  FXbool getIntro() const;

  /// Set intro mode status
  void setIntro(FXbool mode);

  /// Get intro time period
  FXuint getIntroLength() const;

  /// Set intro mode time period
  void setIntroLength(FXuint seconds);

   /// Get random track selection status
  FXbool getRandom() const;

  /// Set random track selection status
  void setRandom(FXbool mode);

  /// Check for muted audio
  FXbool getMute() const;

  /// Mute audio
  FXbool setMute(FXbool mode);

  /// Get volume level
  FXint getVolume() const;

  /// Set volume level
  FXbool setVolume(FXint volume);

  /// Get left-right audio balance
  FXfloat getBalance() const;

  /// Set left-right audio balance
  FXbool setBalance(FXfloat balance);

  /// Get random track
  FXint randomTrack();

  /// Destructor
  ~CDPlayer();
};

#endif
