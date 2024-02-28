/*
 * playlist.h
 *
 *  Created on: Feb 13, 2024
 *      Author: henry
 *
 *  Playlist for playing wav files from SDCard
 *
 */

#ifndef PLAYLIST_H_
#define PLAYLIST_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define PLAYLIST_SIZE       128
#define SONG_MAX_LENGTH     16


//////////////////////////////////////////////////
//PlayList - Contains a list of songs on the SDCard
struct playlistItem
{
    struct playlistItem *next;
    uint8_t index;
    uint8_t alive;
    char songName[SONG_MAX_LENGTH];
};

typedef struct playlistItem PlayListItem;


////////////////////////////////////////
typedef enum
{
    PLAY_MODE_SINGLE,
    PLAY_MODE_RANDOM,
    PLAY_MODE_CONTINUOUS
}PlayMode;



void playList_Init(void);
uint8_t playList_getNumSongs(void);
uint8_t playList_getFreeItem(void);

uint8_t playList_AddSong(const char* name);
uint8_t playList_RemoveSong(const char* name);

uint16_t playList_printSongList(uint8_t *buffer, uint16_t maxLength);

PlayListItem* playList_getCurrentSong(void);
PlayListItem* playList_getNextSong(void);
PlayListItem* playList_getPreviousSong(void);
PlayListItem* playList_skipSong(void);

void playList_playCurrentSong(void);
void playList_playNextSong(void);
void playList_stopCurrentSong(void);
void playList_pauseCurrentSong(void);
void playList_resumeCurrentSong(void);

PlayListItem* playList_setRandomSong(void);
PlayListItem* playList_getRandomSong(void);
void playList_playRandomSong(void);

uint8_t playList_isValidSong(char* name);

void playList_setPlayMode(PlayMode MODE);
PlayMode playList_getPlayMode(void);
PlayMode playList_TogglePlayMode(void);

void playList_displayUpdate(void);


#endif /* PLAYLIST_H_ */
