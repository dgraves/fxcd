/* CDPlayer.cpp
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

#include <errno.h>
#include <time.h>
#include <algorithm>
#include <cdlyte.h>
#include <fox-1.6/fx.h>
#include "CDPlayer.h"

#define VOLUME_TO_DOUBLE(v) (((FXdouble)(v))/100.0)
#define VOLUME_TO_INT(v)   ((FXint)floor(((v)*100.0)+0.5))    // Float to int conversion with rounding

// Function for random track generation
ptrdiff_t rand_func(ptrdiff_t max)
{
  double num=static_cast<double>(rand())/static_cast<double>(RAND_MAX);
  return static_cast<ptrdiff_t>(num*max);
}

CDPlayer::CDPlayer()
: media(-1),
  nodisc(TRUE),
  audiodisc(FALSE),
  stopped(TRUE),
  mute(FALSE),
  repeatMode(CDREPEAT_NONE),
  intro(FALSE),
  random(FALSE),
  currentTrack(0),
  volLevel(0),
  volBalance(0.0)
{
  cd_init_disc_info(&discInfo);
  memset(&volCurrent,0,sizeof(volCurrent));

  playlen.minutes=0;
  playlen.seconds=0;

  //Default intro time
  introTime.minutes=0;
  introTime.seconds=10;
}

CDPlayer::~CDPlayer()
{
  //Not stopping play.  Just closing handle.
  finish();
  cd_free_disc_info(&discInfo);
}

//Check disc properties
void CDPlayer::load()
{
  FXint i,sec;

  cd_stat(media,&discInfo);
  if(!discInfo.disc_present)
  {
    cd_close(media);  //Drive door may be open
    cd_stat(media,&discInfo);
    nodisc=(discInfo.disc_present)?FALSE:TRUE;
  }
  else
    nodisc=FALSE;

  audiodisc=FALSE;  //Will be set to true if it passes the following test

  if(nodisc)
  {
    currentTrack=0;
  }
  else
  {
    //If it's playing it can't be stopped
    if(discInfo.disc_mode==CDLYTE_PLAYING||discInfo.disc_mode==CDLYTE_PAUSED)
      stopped=FALSE;

    //Is audio disc?
    for(i=0;i<discInfo.disc_total_tracks;i++)
    {
      if(discInfo.disc_track[i].track_type==CDLYTE_TRACK_AUDIO)
      {
        audiodisc=TRUE;
        break;
      }
    }

    //Make the current track be the first track - or track being played
    if(audiodisc&&(discInfo.disc_mode==CDLYTE_PLAYING||discInfo.disc_mode==CDLYTE_PAUSED))
      currentTrack=discInfo.disc_current_track;
    else
      currentTrack=discInfo.disc_first_track;

    //Calculate actual play time for disc in seconds
    sec=0;
    for(i=0;i<discInfo.disc_total_tracks;i++)
    {
      sec+=discInfo.disc_track[i].track_length.minutes*60+discInfo.disc_track[i].track_length.seconds;
    }
    playlen.minutes=sec/60;
    playlen.seconds=sec%60;
    playlen.frames=0;
  }

  //Helpful when starting while disc is already playing
  srand(time(NULL));
  makeRandomList();
}

//Check disc status
void CDPlayer::polldisc()
{
  struct disc_status status;
  cd_poll(media,&status);
  cd_update(&discInfo,&status);
}

//Set the volume
FXbool CDPlayer::setvol()
{
  if(volBalance>0.0)
  {
    volCurrent.vol_front.left=VOLUME_TO_DOUBLE(volLevel)*(1.0-volBalance);
    volCurrent.vol_back.left=VOLUME_TO_DOUBLE(volLevel)*(1.0-volBalance);
    volCurrent.vol_front.right=VOLUME_TO_DOUBLE(volLevel);
    volCurrent.vol_back.right=VOLUME_TO_DOUBLE(volLevel);
  }
  else if(volBalance<0.0)
  {
    volCurrent.vol_front.right=VOLUME_TO_DOUBLE(volLevel)*(1.0+volBalance);
    volCurrent.vol_back.right=VOLUME_TO_DOUBLE(volLevel)*(1.0+volBalance);
    volCurrent.vol_front.left=VOLUME_TO_DOUBLE(volLevel);
    volCurrent.vol_back.left=VOLUME_TO_DOUBLE(volLevel);
  }
  else
  {
    volCurrent.vol_front.right=volCurrent.vol_front.left=VOLUME_TO_DOUBLE(volLevel);
    volCurrent.vol_back.right=volCurrent.vol_back.left=VOLUME_TO_DOUBLE(volLevel);
  }

  //Don't change anything if we are mute
  if(!mute)
  {
    if(cd_set_volume(media,&volCurrent)<0)
      return FALSE;
  }

  return TRUE;
}

//Check for volume change from other application
FXbool CDPlayer::checkvol()
{
  if(!mute)
  {
    disc_volume volume;
    if(cd_get_volume(media,&volume)<0)
    {
      return FALSE;
    }

    //Someone has been modifying our audio settings.
    //Only update volume and balance if level changed by third party
    if(memcmp(&volume,&volCurrent,sizeof(volume))!=0)
    {
      memcpy(&volCurrent,&volume,sizeof(volume));
      volLevel=VOLUME_TO_INT((volume.vol_front.left>volume.vol_front.right)?volume.vol_front.left:volume.vol_front.right);

      if(volume.vol_front.left<volume.vol_front.right)
        volBalance=1.0-(volume.vol_front.left/volume.vol_front.right);
      else if(volume.vol_front.right<volume.vol_front.left)
        volBalance=(volume.vol_front.right/volume.vol_front.left)-1.0;
      else
        volBalance=0.0;
    }
  }

  return TRUE;
}

//A list for random selections to be taken from
//Guarantees no tracks are repeated
void CDPlayer::makeRandomList()
{
  FXint i;
  randomArray.clear();
  for(i=0;i<discInfo.disc_total_tracks;i++)
    randomArray.push_back(i+discInfo.disc_first_track);
  std::random_shuffle(randomArray.begin(),randomArray.end(),rand_func);
}

//Returns track num between 1 and total.
//If finished, returns 0 currentTrack=getRandomTrack();
FXint CDPlayer::getRandomTrack()
{
  FXint selection=0;
  if(!randomArray.empty())
  {
    selection=randomArray.back();
    randomArray.pop_back();
  }
  return selection;
}

//Open the device
FXbool CDPlayer::init(const FXString& device)
{
  //Load selected cdrom
  if(media>=0)
  {
    cd_finish(media);
    media=-1;
  }

  media=cd_init_device((FXchar*)device.text());
  if(media<0)
    return FALSE;

  load();
  checkvol();
  return TRUE;
}

//Close media handle
FXbool CDPlayer::finish()
{
  if(media<0)
    return FALSE;

  cd_finish(media);
  media=-1;

  return TRUE;
}

FXbool CDPlayer::play()
{
  if(media<0)
    return FALSE;

  if((discInfo.disc_mode!=CDLYTE_PLAYING)||
     (discInfo.disc_mode!=CDLYTE_PAUSED))
  {
    if(random)
    {
      makeRandomList();
      currentTrack=getRandomTrack();
    }
    if(cd_play(media,currentTrack)<0)
    {
      return FALSE;
    }
    stopped=FALSE;
  }

  return TRUE;
}

FXbool CDPlayer::pause()
{
  if(media<0)
    return FALSE;

  if(discInfo.disc_mode==CDLYTE_PLAYING)
  {
    if(cd_pause(media)<0)
      return FALSE;
  }

  return TRUE;
}

FXbool CDPlayer::resume()
{
  if(media<0)
    return FALSE;

  if(discInfo.disc_mode==CDLYTE_PAUSED)
  {
    if(cd_resume(media)<0)
      return FALSE;
  }

  return TRUE;
}

FXbool CDPlayer::stop()
{
  if(media<0)
    return FALSE;

  if((discInfo.disc_mode==CDLYTE_PLAYING)||
     (discInfo.disc_mode==CDLYTE_PAUSED))
  {
    if(cd_stop(media)<0)
      return FALSE;
    stopped=TRUE;
    currentTrack=discInfo.disc_first_track;
  }

  return TRUE;
}

FXbool CDPlayer::skipNext()
{
  if(media<0)
    return FALSE;

  if(random)
  {
    currentTrack=getRandomTrack();
    if(currentTrack==0)
    {
      if(repeatMode==CDREPEAT_DISC)
      {
        makeRandomList();
        currentTrack=getRandomTrack();
      }
      else
      {
	return stop();
      }
    }
  }
  else
  {
    if(currentTrack<discInfo.disc_total_tracks)
      currentTrack++;
    else if(repeatMode==CDREPEAT_DISC)
      currentTrack=discInfo.disc_first_track;
    else
    {
      return stop();
    }
  }

  if(discInfo.disc_mode==CDLYTE_PLAYING)
  {
    if(cd_play(media,currentTrack)<0)
      return FALSE;
  }
  else if(discInfo.disc_mode==CDLYTE_PAUSED)
  {
    if(cd_play(media,currentTrack)<0)
      return FALSE;
    cd_pause(media);  // Attempt to resume paused state
  }

  return TRUE;
}

FXbool CDPlayer::skipPrev()
{
  if(media<0)
    return FALSE;

  if(random)
  {
    if(discInfo.disc_track_time.minutes==0&&discInfo.disc_track_time.seconds<2)
    {
      currentTrack=getRandomTrack();
      if(currentTrack==0)
      {
        if(repeatMode==CDREPEAT_DISC)
	{
          makeRandomList();
          currentTrack=getRandomTrack();
	}
        else
	{
	  return stop();
	}
      }
    }
  }
  else if(currentTrack>discInfo.disc_first_track)
  {
    if(discInfo.disc_track_time.minutes==0&&discInfo.disc_track_time.seconds<2)
      currentTrack--;
  }

  if(discInfo.disc_mode==CDLYTE_PLAYING)
  {
    if(cd_play(media,currentTrack)<0)
      return FALSE;
  }
  else if(discInfo.disc_mode==CDLYTE_PAUSED)
  {
    if(cd_play(media,currentTrack)<0)
      return FALSE;
    cd_pause(media);  //We'll let this one slide by if it fails
  }

  return TRUE;
}

FXbool CDPlayer::seekForward(FXuint seconds)
{
  if(media<0)
    return FALSE;

  struct disc_timeval seekTime={seconds/60,seconds%60,0};
  if(cd_playctl(media,PLAY_START_POSITION,currentTrack,&seekTime)<0)
    return FALSE;

  return TRUE;
}

FXbool CDPlayer::seekReverse(FXuint seconds)
{
  if(media<0)
    return FALSE;

  struct disc_timeval seekTime;
  seekTime.minutes=discInfo.disc_track_time.minutes-(seconds/60);
  seekTime.seconds=discInfo.disc_track_time.seconds-(seconds%60);
  if(cd_playctl(media,PLAY_START_POSITION,currentTrack,&seekTime)<0)
    return FALSE;

  return TRUE;
}

FXbool CDPlayer::playTrackPos(FXint track,const struct disc_timeval* time)
{
  if(media<0)
    return FALSE;

  currentTrack=track;
  if(cd_playctl(media,PLAY_START_POSITION,track,time)<0)
    return FALSE;

  return TRUE;
}

FXbool CDPlayer::openTray()
{
  if(media<0)
    return FALSE;

  //Some cd players continue to act as if playing if ejected while playing, and ignore stop requests while tray is open.
  //So check for present disc to avoid infinite loop.
  if(nodisc==FALSE&&(discInfo.disc_mode==CDLYTE_PLAYING||discInfo.disc_mode==CDLYTE_PAUSED))
  {
    if(!stop())
      return FALSE;

    //Don't eject until disk is stopped - best solution?
    do
    {
      polldisc();
    }while(discInfo.disc_mode==CDLYTE_PLAYING||discInfo.disc_mode==CDLYTE_PAUSED);

    //Give it a few milliseconds to settle
    struct timeval tv={0,10000};
    select(0,NULL,NULL,NULL,&tv);
  }

  if(cd_eject(media)<0)
    return FALSE;

  return TRUE;
}

FXbool CDPlayer::closeTray()
{
  if(media<0)
    return FALSE;

  if(cd_close(media)<0)
    return FALSE;

  return TRUE;
}

//Update state
FXbool CDPlayer::update()
{
  if(media<0)
    return FALSE;

  polldisc();
  checkvol();

  if(discInfo.disc_present)
  {
    //Was a disc just inserted?
    if(nodisc)
      load();

    if(audiodisc)
    {
      //Check for completion of play for devices that do not report completed status (Windows MCI)
      if(discInfo.disc_mode==CDLYTE_PLAYING)
      {
        if(discInfo.disc_length.minutes==discInfo.disc_time.minutes&&
           discInfo.disc_length.seconds==discInfo.disc_time.seconds&&
           discInfo.disc_length.frames==discInfo.disc_time.frames)
        {
          discInfo.disc_mode=CDLYTE_COMPLETED;
        }
      }

      //What's happening
      switch(discInfo.disc_mode)
      {
      case CDLYTE_PLAYING:
        //Check for track change
        if((currentTrack!=discInfo.disc_current_track)||
           (intro&&((discInfo.disc_track_time.minutes>=introTime.minutes)&&
           (discInfo.disc_track_time.seconds>introTime.seconds))))
        {
          //Finished a track
          if(repeatMode==CDREPEAT_TRACK)
	    cd_play(media,currentTrack);
          else if(random||intro)
            skipNext();
          else  //Regular play
            currentTrack=discInfo.disc_current_track;
	}
	break;
      case CDLYTE_PAUSED:
        //No action
        break;
      default:
      {
	//The cd-rom is no longer playing, but if 'stopped' is false it should repeat.
	if(stopped) break;

	//Some of them (the ones that require this hack)
	//need a few milliseconds to settle
        struct timeval tv={0,10000};
        select(0,NULL,NULL,NULL,&tv);
      }
      case CDLYTE_COMPLETED:
        //Continuous play - some cheaper CD-ROMS constantly report this when
        //they are idle.  So we have the stopped flag.
	if(!stopped&&(repeatMode==CDREPEAT_TRACK))
	{
	  cd_play(media,currentTrack);
	}
        else if(!stopped&&(repeatMode==CDREPEAT_DISC))
	{
          //Start from begining
          if(random)
          {
	    makeRandomList();
	    currentTrack=getRandomTrack();
	  }
          else
 	    currentTrack=discInfo.disc_first_track;
          cd_play(media,currentTrack);
        }
        else
	{
          currentTrack=discInfo.disc_first_track;
	}
        break;
      }
    }
  }
  else
  {
    nodisc=TRUE;
    currentTrack=0;
  }

  return TRUE;
}

FXint CDPlayer::getStatus() const
{
  return discInfo.disc_mode;
}

FXint CDPlayer::getDescriptor() const
{
  return media;
}

FXbool CDPlayer::isValid() const
{
  return (media!=-1);
}

FXbool CDPlayer::isDiscPresent() const
{
  return (nodisc==FALSE);
}

FXbool CDPlayer::isAudioDisc() const
{
  return audiodisc;
}

FXint CDPlayer::getStartTrack() const
{
  return discInfo.disc_first_track;
}

FXint CDPlayer::getNumTracks() const
{
  return discInfo.disc_total_tracks;
}

void CDPlayer::getPlayLength(struct disc_timeval& dtv) const
{
  memcpy(&dtv, &playlen, sizeof(struct disc_timeval));
}

void CDPlayer::getDiscLength(struct disc_timeval& dtv) const
{
  memcpy(&dtv, &discInfo.disc_length, sizeof(struct disc_timeval));
}

void CDPlayer::getTrackLength(FXint track, struct disc_timeval& dtv) const
{
  memcpy(&dtv, &discInfo.disc_track[track-1].track_length, sizeof(struct disc_timeval));
}

void CDPlayer::getDiscTime(struct disc_timeval& dtv) const
{
  memcpy(&dtv, &discInfo.disc_time, sizeof(struct disc_timeval));
}

void CDPlayer::getTrackTime(struct disc_timeval& dtv) const
{
  memcpy(&dtv, &discInfo.disc_track_time, sizeof(struct disc_timeval));
}

void CDPlayer::getDiscInfo(struct disc_info& info) const
{
  cd_free_disc_info(&info);
  memcpy(&info, &discInfo, sizeof(struct disc_info)-sizeof(struct track_info *));
  info.disc_track=(struct track_info*)malloc(info.disc_total_tracks*sizeof(struct track_info));
  memcpy(info.disc_track, discInfo.disc_track, info.disc_total_tracks*sizeof(struct track_info));
}

void CDPlayer::getTrackInfo(FXint track, struct track_info& info) const
{
  memcpy(&info, &discInfo.disc_track[track], sizeof(struct track_info));
}

FXint CDPlayer::getCurrentTrack() const
{
  return currentTrack;
}

void CDPlayer::setCurrentTrack(FXint track)
{
  currentTrack=track;
  if(discInfo.disc_mode==CDLYTE_PLAYING)
    cd_play(media,currentTrack);
  else if(discInfo.disc_mode==CDLYTE_PAUSED)
  {
    cd_play(media,currentTrack);
    cd_pause(media);
  }
}

FXuint CDPlayer::getRepeatMode() const
{
  return repeatMode;
}

void CDPlayer::setRepeatMode(FXuint mode)
{
  repeatMode=mode;
}

FXbool CDPlayer::getIntro() const
{
  return intro;
}

void CDPlayer::setIntro(FXbool mode)
{
  intro=mode;
}

FXuint CDPlayer::getIntroLength() const
{
  return (introTime.minutes*60)+introTime.seconds;
}

void CDPlayer::setIntroLength(FXuint seconds)
{
  introTime.minutes=seconds/60;
  introTime.seconds=seconds%60;
}

FXbool CDPlayer::getRandom() const
{
  return random;
}

void CDPlayer::setRandom(FXbool mode)
{
  random=mode;
  if(random)
  {
    srand(time(NULL));
    makeRandomList();
  }
}

FXbool CDPlayer::getMute() const
{
  return mute;
}

FXbool CDPlayer::setMute(FXbool mode)
{
  if(mode!=mute)
  {
    mute=mode;
    if(mode&&media!=-1)
    {
      //Volume level set to zero
      disc_volume volume;
      memset(&volume,0,sizeof(volume));
      if(cd_set_volume(media,&volume)<0)
        return FALSE;
    }
    else if(media!=-1)
    {
      if(setvol()==FALSE)      //Bring the volume back
        return FALSE;
    }
  }

  return TRUE;
}

//Volume is checked in update
FXint CDPlayer::getVolume() const
{
  return volLevel;
}

FXbool CDPlayer::setVolume(FXint volume)
{
  volLevel=volume;
  return setvol();
}

//Volume is checked in update
FXdouble CDPlayer::getBalance() const
{
  return volBalance;
}

FXbool CDPlayer::setBalance(FXdouble balance)
{
  volBalance=balance;
  return setvol();
}

FXint CDPlayer::randomTrack()
{
  FXint track=getRandomTrack();
  if(track==0&&repeatMode==CDREPEAT_DISC)
  {
    makeRandomList();
    track=getRandomTrack();
  }
  return track;
}
