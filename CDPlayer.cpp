/* CDPlayer.cpp
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

#include <errno.h>
#include <time.h>
#include <unistd.h>
extern "C" {
#include "cdaudio.h"
}
#include "fox/fx.h"
#include "fox/FXArray.h"
#include "fox/FXElement.h"
#include "CDPlayer.h"

CDPlayer::CDPlayer()
: media(-1),
  open(FALSE),
  nodisc(TRUE),
  audiodisc(FALSE),
  stopped(TRUE),
  currentTrack(0),
  blinkMode(0),
  repeatMode(CDREPEAT_NONE),
  intro(FALSE),
  random(FALSE),
  volLevel(0),
  volBalance(0.0)
{
  memset(&discInfo,0,sizeof(discInfo));
  memset(&volCurrent,0,sizeof(volCurrent));

  //Default intro time  
  introTime.minutes=0;
  introTime.seconds=10;
}

CDPlayer::~CDPlayer()
{
  //Not stopping it here.  Just closing handle.  
  //stop();
  finish();
}

//Check disc properties
void CDPlayer::load()
{
  FXint i;

  cd_stat(media,&discInfo);
  if(!discInfo.disc_present)
  {
    cd_close(media);  //Drive door may be open
    cd_stat(media,&discInfo);
    nodisc=(discInfo.disc_present)?FALSE:TRUE;
  }
  else
    nodisc=FALSE;

  open=FALSE;  //In any case, the drive has been closed
  audiodisc=FALSE;  //Will be set to true if it passes the following test

  if(nodisc)
  {
    currentTrack=0;
  }
  else
  {
    //Is audio disc?
    for(i=discInfo.disc_first_track;i<=discInfo.disc_total_tracks;i++)
    {
      if(discInfo.disc_track[i-1].track_type==CDAUDIO_TRACK_AUDIO)
      {
        audiodisc=TRUE;
        break;
      }
    }

    //Make the current track be the first track - or track being played
    if(!audiodisc)
      currentTrack=0;
    else if(discInfo.disc_mode==CDAUDIO_PLAYING||discInfo.disc_mode==CDAUDIO_PAUSED)
      currentTrack=discInfo.disc_current_track;
    else
      currentTrack=discInfo.disc_first_track;
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
  cd_update(&discInfo,status);
}

//Set the volume
void CDPlayer::setvol()
{
  if(volBalance>0.0)
  {
    volCurrent.vol_front.left=(FXint)(volLevel*(1.0-volBalance));
    volCurrent.vol_back.left=(FXint)(volLevel*(1.0-volBalance));
    volCurrent.vol_front.right=volLevel;
    volCurrent.vol_back.right=volLevel;
  }
  else if(volBalance<0.0)
  {
    volCurrent.vol_front.right=(FXint)(volLevel*(1.0+volBalance));
    volCurrent.vol_back.right=(FXint)(volLevel*(1.0+volBalance));
    volCurrent.vol_front.left=volLevel;
    volCurrent.vol_back.left=volLevel;
  }
  else
  {
    volCurrent.vol_front.right=volCurrent.vol_front.left=volLevel;
    volCurrent.vol_back.right=volCurrent.vol_back.left=volLevel;
  }

  if(cd_set_volume(media,volCurrent)<0)
    perror("cd_set_volume");
}

//Check for volume change from other application
void CDPlayer::checkvol()
{
  disc_volume volume;
  if(cd_get_volume(media,&volume)<0)
  {
    perror("cd_get_volume");
    return;
  }

  //Someone has been modifying our audio settings.  
  //Only update volume and balance if level changed by third party 
  if(memcmp(&volume,&volCurrent,sizeof(volume))!=0)
  {
    memcpy(&volCurrent,&volume,sizeof(volume));
    volLevel=(volume.vol_front.left>volume.vol_front.right)?volume.vol_front.left:volume.vol_front.right;

    if(volume.vol_front.left<volume.vol_front.right)
      volBalance=1.0-(((FXfloat)volume.vol_front.left)/((FXfloat)volume.vol_front.right));
    else if(volume.vol_front.right<volume.vol_front.left)
      volBalance=(((FXfloat)volume.vol_front.right)/((FXfloat)volume.vol_front.left))-1.0;
    else
      volBalance=0.0;

    if(volBalance!=0.0)
    {
      //Truncate to 2 digits
      volBalance=(FXint)(100.0*volBalance);
      volBalance/=100.0;
    }
  }
}

//A list for random selections to be taken from
//Guarantees no tracks are repeated
void CDPlayer::makeRandomList()
{
  FXint i;
  randomArray.clear();
  for(i=discInfo.disc_first_track;i<=discInfo.disc_total_tracks;i++)
    randomArray.append(i);
}

//Returns track num between 1 and total.  
//If finished, returns 0 currentTrack=getRandomTrack();
FXint CDPlayer::getRandomTrack()
{
  FXint selection=0,no=randomArray.no();
  if(no>0)
  {
    //We need a number between 0 and no-1
    FXint index=(no>1)?rand()%no:0;
    selection=randomArray[index];
    randomArray.remove(index);
  }
  return selection;
}

//Open the device
FXbool CDPlayer::init(const FXString& device)
{
  FXint i;

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

  if((discInfo.disc_mode!=CDAUDIO_PLAYING)||
     (discInfo.disc_mode!=CDAUDIO_PAUSED))
  {
    if(random)
    {
      makeRandomList();
      currentTrack=getRandomTrack();
    }
    cd_play(media,currentTrack);
    stopped=FALSE;
  }

  return TRUE;
}

FXbool CDPlayer::pause()
{
  if(media<0)
    return FALSE;

  if(discInfo.disc_mode==CDAUDIO_PLAYING)
  {
    cd_pause(media);
    blinkMode=0;
  }

  return TRUE;
}

FXbool CDPlayer::resume()
{
  if(media<0)
    return FALSE;

  if(discInfo.disc_mode==CDAUDIO_PAUSED)
    cd_resume(media);

  return TRUE;
}

FXbool CDPlayer::stop()
{
  if(media<0)
    return FALSE;

  if((discInfo.disc_mode==CDAUDIO_PLAYING)||
     (discInfo.disc_mode==CDAUDIO_PAUSED))
  {
    cd_stop(media);
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
	//cd_stop(media);
        //currentTrack=discInfo.disc_first_track;
	stop();
        return TRUE;
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
      //cd_stop(media);
      //currentTrack=discInfo.disc_first_track;
      stop();
      return TRUE;
    }
  }

  if(discInfo.disc_mode==CDAUDIO_PLAYING)
    cd_play(media,currentTrack);
  else if(discInfo.disc_mode==CDAUDIO_PAUSED)
  {
    cd_play(media,currentTrack);
    cd_pause(media);
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
          //cd_stop(media);
          //currentTrack=discInfo.disc_first_track;
	  stop();
          return TRUE;
	}
      }
    }
  }
  else if(currentTrack>discInfo.disc_first_track)
  {
    if(discInfo.disc_track_time.minutes==0&&discInfo.disc_track_time.seconds<2)
      currentTrack--;
  }

  if(discInfo.disc_mode==CDAUDIO_PLAYING)
    cd_play(media,currentTrack);
  else if(discInfo.disc_mode==CDAUDIO_PAUSED)
  {
    cd_play(media,currentTrack);
    cd_pause(media);
  }  

  return TRUE;
}

FXbool CDPlayer::seekForward(FXuint seconds)
{
  if(media<0)
    return FALSE;

  struct disc_timeval seekTime={seconds/60,seconds%60,0};
  cd_playctl(media,PLAY_START_POSITION,currentTrack,&seekTime);

  return TRUE;
}

FXbool CDPlayer::seekReverse(FXuint seconds)
{
  if(media<0)
    return FALSE;

  struct disc_timeval seekTime;
  seekTime.minutes=discInfo.disc_track_time.minutes-(seconds/60);
  seekTime.seconds=discInfo.disc_track_time.seconds-(seconds%60);
  cd_playctl(media,PLAY_START_POSITION,currentTrack,&seekTime);

  return TRUE;
}

FXbool CDPlayer::playTrackPos(FXint track,const struct disc_timeval* time)
{
  if(media<0)
    return FALSE;

  currentTrack=track;
  cd_playctl(media,PLAY_START_POSITION,track,time);

  return true;
}

FXbool CDPlayer::openTray()
{
  if(media<0)
    return FALSE;

  if(discInfo.disc_mode==CDAUDIO_PLAYING||discInfo.disc_mode==CDAUDIO_PAUSED)
    stop();
    //cd_stop(media);

  //Don't eject until disk is stopped - best solution?
  do
  {
    polldisc();
  }while(discInfo.disc_mode==CDAUDIO_PLAYING||discInfo.disc_mode==CDAUDIO_PAUSED);

  cd_eject(media);
  open=TRUE;

  return TRUE;
}

FXbool CDPlayer::closeTray()
{
  if(media<0)
    return FALSE;

  cd_close(media);
  open=FALSE;

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
      //What's happening
      switch(discInfo.disc_mode)
      {
      case CDAUDIO_PLAYING:
        //Check for track change
        if((currentTrack!=discInfo.disc_current_track)||
           (intro&&((discInfo.disc_track_time.minutes>introTime.minutes)||
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

      case CDAUDIO_PAUSED:
        //Make time blink
        if(++blinkMode>=10) blinkMode=0;
	break;

      case CDAUDIO_COMPLETED:
        //Continuous play - some cheaper CD-ROMS constantly report this when
        //they are idle.  So we have the stopped flag.
        if(!stopped&&(repeatMode==CDREPEAT_DISC))
	{
          //Start from begining
          if(random)
          {
	    makeRandomList();
	    currentTrack=getRandomTrack();
	  }
          else
 	    currentTrack=discInfo.disc_first_track;
          play();
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

FXbool CDPlayer::isDoorOpen() const
{
  return open;
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

const struct disc_timeval* CDPlayer::getDiscLength() const
{
  return &discInfo.disc_length;
}

const struct disc_timeval* CDPlayer::getDiscTime() const
{
  return &discInfo.disc_time;  
}

const struct disc_timeval* CDPlayer::getTrackTime() const
{
  return &discInfo.disc_track_time;
}

const struct disc_info* CDPlayer::getDiscInfo() const
{
  return &discInfo;
}

const struct track_info* CDPlayer::getTrackInfo(FXint track) const
{
  return &discInfo.disc_track[track];
}

FXint CDPlayer::getCurrentTrack() const
{
  return currentTrack;
}

void CDPlayer::setCurrentTrack(FXint track)
{
  currentTrack=track;
  if(discInfo.disc_mode==CDAUDIO_PLAYING)
    cd_play(media,currentTrack);
  else if(discInfo.disc_mode==CDAUDIO_PAUSED)
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

//Volume is checked in update
FXint CDPlayer::getVolume() const
{
  return volLevel;
}

void CDPlayer::setVolume(FXint volume)
{
  volLevel=volume;
  setvol();
}

//Volume is checked in update
FXfloat CDPlayer::getBalance() const
{
  return volBalance;
}

void CDPlayer::setBalance(FXfloat balance)
{
  volBalance=balance;
  setvol();
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

FXbool CDPlayer::blink() const
{
  return (blinkMode<5);
}
