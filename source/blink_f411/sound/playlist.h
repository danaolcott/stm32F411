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



#define PLAYLIST_SIZE       20
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




void playList_Init(void);
uint8_t playList_getNumSongs(void);
uint8_t playList_getFreeItem(void);

uint8_t playList_AddSong(const char* name);
void playList_RemoveSong(char* name);
void playList_playNext(void);
uint16_t playList_printSongList(uint8_t *buffer, uint16_t maxLength);




#endif /* PLAYLIST_H_ */
