/* CDPlayer.h
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

#ifndef _CDPLAYER_H_
#define _CDPLAYER_H_

enum
{
  CDREPEAT_NONE=0,
  CDREPEAT_TRACK=1,
  CDREPEAT_DISC=2
};

class CDPlayer
{
  friend class CDWindow;      //For data targets
protected:
  FXint media;                //cd-rom handle
  FXbool open;                //drive door status (open/closed)
  FXbool nodisc;              //Indicates known presence of disc in cdrom
  FXbool audiodisc;           //Is disc audio or data
  FXbool stopped;             //Was a stop requested (or did the disc finish)
  FXbool mute;                //Is muted

  FXint currentTrack;         //track currently playing (or to play)
  FXint blinkMode;            //should blink if paused?
  FXuint repeatMode;          //No repeat, repeat track, repeat disc
  FXbool intro;               //Intro mode is active
  FXbool random;              //Shuffle play is active
  FXArray<FXint> randomArray;

  FXint volLevel;
  FXfloat volBalance;
  struct disc_volume volCurrent;        //Use to test for volume change by other party
  struct disc_info discInfo;            //info for current cd
  struct disc_timeval introTime;        //Length of time for intro play

  void load();
  void polldisc();
  void setvol();
  void checkvol();
  void makeRandomList();
  FXint getRandomTrack();

public:
  CDPlayer();
  ~CDPlayer();

  FXbool init(const FXString& device);
  FXbool finish();
  FXbool play();
  FXbool pause();
  FXbool resume();
  FXbool stop();
  FXbool skipNext();
  FXbool skipPrev();
  FXbool seekForward(FXuint seconds);
  FXbool seekReverse(FXuint seconds);  
  FXbool playTrackPos(FXint track,const struct disc_timeval* time);
  FXbool openTray();
  FXbool closeTray();

  FXbool update();

  //Current cd-rom status (play,pause,finished,etc)
  FXint getStatus() const;
  FXint getDescriptor() const;
  FXbool isValid() const;
  FXbool isDoorOpen() const;
  FXbool isDiscPresent() const;
  FXbool isAudioDisc() const;

  FXint getStartTrack() const;
  FXint getNumTracks() const;

  const struct disc_timeval* getDiscLength() const;
  const struct disc_timeval* getDiscTime() const;
  const struct disc_timeval* getTrackTime() const;
  const struct disc_info* getDiscInfo() const;
  const struct track_info* getTrackInfo(FXint track) const;

  FXint getCurrentTrack() const;
  void setCurrentTrack(FXint track);

  FXuint getRepeatMode() const;
  void setRepeatMode(FXuint mode);

  FXbool getIntro() const;
  void setIntro(FXbool mode);

  FXuint getIntroLength() const;
  void setIntroLength(FXuint seconds);

  FXbool getRandom() const;
  void setRandom(FXbool mode);

  FXbool getMute() const;
  void setMute(FXbool mode);
  
  FXint getVolume() const;
  void setVolume(FXint volume);

  FXfloat getBalance() const;
  void setBalance(FXfloat balance);

  FXint randomTrack();

  FXbool blink() const;
};
 
#endif
