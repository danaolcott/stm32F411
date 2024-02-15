/*
 * playlist.c
 *
 *  Created on: Feb 13, 2024
 *      Author: henry
 *
 *  Playlist for playing songs off on an SD Card
 *  Uses the DIR.TXT for getting file names with the
 *  .WAV extension.  When adding a song, just checks
 *  the DIR.TXT file if the song name string is contained
 *  in the directly file.
 *
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "playlist.h"
#include "wav.h"

PlayListItem gPlayList[20];
PlayListItem *gPlayListHead;


///////////////////////////////////
//initializes a blank playlist
//with the head pointing at the
//first index
void playList_Init(void)
{
    uint8_t i = 0;
    for (i = 0 ; i < PLAYLIST_SIZE ; i++)
    {
        gPlayList[i].next = NULL;
        gPlayList[i].index = i;
        gPlayList[i].alive = 0;
        memset(gPlayList[i].songName, 0x00, SONG_MAX_LENGTH);
    }

    //set the head node pointer
    gPlayListHead = &gPlayList[0];

}


////////////////////////////////////////////
//Count the number of songs in the list
//with alive = 1
uint8_t playList_getNumSongs(void)
{
    PlayListItem *ptr = gPlayListHead;
    uint8_t length = 0;

    if (!(ptr->alive))
        return 0;
    if ((ptr->alive == 1) && (ptr->next == NULL))
        return 1;


    while (ptr->next != NULL)
    {
        if (ptr->alive == 1)
            length++;

        ptr = ptr->next;
    }

    //last item
    if (ptr->alive == 1)
        length++;

    return length;
}


////////////////////////////////////////////
//returns the index of the first item
//in the PlayList array with alive = 0
uint8_t playList_getFreeItem(void)
{
    uint8_t i;
    uint8_t index = 0;
    for (i = 0 ; i < PLAYLIST_SIZE ; i++)
    {
        if (!(gPlayList[i].alive))
        {
            index = i;
            i = PLAYLIST_SIZE;
        }
    }

    return index;
}

///////////////////////////////////////////
//Adds a song at the end of the list
//For now, it assumes the song name is
//valid and on the SDcard
//returns the length of the updated playlist
uint8_t playList_AddSong(const char* name)
{
    //check for an empty playlist
    PlayListItem *ptr = gPlayListHead;
    uint8_t index, i = 0;
    uint8_t length = 2;

    if (!(ptr->alive))
    {
        //empty playlist
        ptr->alive = 1;
        memset(ptr->songName, 0x00, SONG_MAX_LENGTH);
        strncpy(ptr->songName, name, SONG_MAX_LENGTH);

        return 1;
    }

    if (ptr->next == NULL)
    {
        //there is only 1 song, the head pointing to null
        //get a new node
        index = playList_getFreeItem();
        ptr->next = &gPlayList[index];
        ptr = ptr->next;
        ptr->alive = 1;
        memset(ptr->songName, 0x00, SONG_MAX_LENGTH);
        strncpy(ptr->songName, name, SONG_MAX_LENGTH);
        ptr->next = NULL;

        return 2;
    }

    //more than 2 items, jump to the end of the list
    else
    {
        //jump to the end of the list
        while (ptr->next != NULL)
        {
            ptr = ptr->next;
            length++;
        }

        //get a new node;
        index = playList_getFreeItem();
        ptr->next = &gPlayList[index];
        ptr = ptr->next;
        ptr->alive = 1;
        memset(ptr->songName, 0x00, SONG_MAX_LENGTH);
        strncpy(ptr->songName, name, SONG_MAX_LENGTH);
        ptr->next = NULL;
    }

    return length;
}


void playList_RemoveSong(char* name)
{

}

void playList_playNext(void)
{

}

uint16_t playList_printSongList(uint8_t *buffer, uint16_t maxLength)
{

}

