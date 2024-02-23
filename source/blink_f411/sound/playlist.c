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
#include "lcd_12864_dfrobot.h"

PlayListItem gPlayList[PLAYLIST_SIZE];
PlayListItem *gPlayListHead;            //head node - start of the playlist
PlayListItem *gPlayListCurrentSong;     //pointer to the current song - init at the head
PlayListItem *gPlayListNextSong;        //pointer the next song
PlayListItem *gPlayListPreviousSong;    //pointer to the previous song

volatile static PlayMode gPlayListMode = PLAY_MODE_SINGLE;        //set to 1 if continuous, 0 for single play

///////////////////////////////////
//initializes a blank playlist
//with the head pointing at the
//first index
void playList_Init(void)
{
    gPlayListMode = PLAY_MODE_SINGLE;

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
    gPlayListCurrentSong = gPlayListHead;
    gPlayListNextSong = gPlayListHead;
    gPlayListPreviousSong = gPlayListHead;

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
    uint8_t index;
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

        gPlayListNextSong = ptr;

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


//////////////////////////////////////////////////
//Remove a PlayListItem from the list.
uint8_t playList_RemoveSong(const char* name)
{
    PlayListItem *ptr = gPlayListHead;
    PlayListItem *next = gPlayListHead;
    PlayListItem *prev = gPlayListHead;

    uint8_t counter = 2;


    //name is at the head - remove the head
    if (!strcmp(ptr->songName, name))
    {
        if (ptr->next == NULL)
        {
            //clear the head node, etc. - it's a single item list
            ptr->alive = 0;
            memset(ptr->songName, 0x00, SONG_MAX_LENGTH);
            return 0;
        }

        //there is at least 1 other node, so remove the
        //head and update
        prev = gPlayListHead;
        gPlayListHead = gPlayListHead->next;

        //remove the prev - free it up
        prev->alive = 0;
        prev->next = NULL;
        memset(prev->songName, 0x00, SONG_MAX_LENGTH);

        return 1;
    }

    //target node is somewhere in the middle or end of the list
    //follow the until you find the name that matches
    //assuming at least 3 elements
    prev = gPlayListHead;
    ptr = gPlayListHead->next;
    next = ptr->next;

    while (ptr->next != NULL)
    {
        if (!strcmp(ptr->songName, name))
        {
            //names match, remove the item.
            //set prev->next = next
            //clear the contents of ptr
            prev->next = next;

            ptr->alive = 0;
            ptr->next = NULL;
            memset(ptr->songName, 0x00, SONG_MAX_LENGTH);

            return counter;

        }

        prev = ptr;
        ptr = ptr->next;
        next = ptr->next;

        counter++;
    }

    //test the last item on the list
    if (ptr->next == NULL)
    {
        if (!strcmp(ptr->songName, name))
        {
            prev->next = NULL;      //set the new tail of the list

            ptr->alive = 0;
            ptr->next = NULL;
            memset(ptr->songName, 0x00, SONG_MAX_LENGTH);
        }
    }

    return 0;
}

/////////////////////////////////////////////////////////
//Prints the song list to a buffer of maxLength
//returns the length of the string
uint16_t playList_printSongList(uint8_t *buffer, uint16_t maxLength)
{
    PlayListItem *ptr = gPlayListHead;
    uint16_t offset = 0;
    memset(buffer, 0x00, maxLength);

    while (ptr->next != NULL)
    {
        strncpy((char*)(buffer + offset), ptr->songName, maxLength - offset);
        offset += strlen(ptr->songName);
        strncpy((char*)(buffer + offset), "\r\n", maxLength - offset);
        offset += strlen("\r\n");
        ptr = ptr->next;
    }

    //finally, add the last song
    strncpy((char*)(buffer + offset), ptr->songName, maxLength - offset);
    offset += strlen(ptr->songName);
    strncpy((char*)(buffer + offset), "\r\n", maxLength - offset);
    offset += strlen("\r\n");

    return offset;
}


PlayListItem* playList_getCurrentSong(void)
{
    return gPlayListCurrentSong;
}

PlayListItem* playList_getNextSong(void)
{
    return gPlayListNextSong;
}

PlayListItem* playList_getPreviousSong(void)
{
    return gPlayListPreviousSong;
}




////////////////////////////////////////////
//skips to the next song in the playlist
//returns a pointer to the new current song
//advances the current, next, and previous
//playlistitem pointers along the playlist
PlayListItem* playList_skipSong(void)
{
    PlayListItem* ptr = gPlayListCurrentSong;

    //not at the end of the list
    if (ptr->next != NULL)
    {
        gPlayListPreviousSong = gPlayListCurrentSong;
        gPlayListCurrentSong = ptr->next;
        ptr = ptr->next;

        if (ptr->next !=NULL)
        {
            gPlayListNextSong = ptr->next;
        }
        else
        {
            gPlayListNextSong = gPlayListHead;
        }
    }

    //the current is at the end of the line
    else
    {
        gPlayListPreviousSong = gPlayListCurrentSong;
        gPlayListCurrentSong = gPlayListHead;
        gPlayListNextSong = gPlayListHead->next;
    }

    return gPlayListCurrentSong;
}


void playList_playCurrentSong(void)
{
    if (wav_getSoundPlayStatus())
    {
        wav_stopSound();
    }

    wav_playSound(gPlayListCurrentSong->songName);
}


void playList_playNextSong(void)
{
    if (wav_getSoundPlayStatus())
    {
        wav_stopSound();
    }

    //increment the playlist
    playList_skipSong();

    //play the current
    wav_playSound(gPlayListCurrentSong->songName);
}




void playList_stopCurrentSong(void)
{
    wav_stopSound();
}

void playList_pauseCurrentSong(void)
{
    wav_pauseSound();
}

void playList_resumeCurrentSong(void)
{
    wav_resumeSound();
}


///////////////////////////////////////
//Checks to see if the song "name" is
//on the playlist.  returns 0 if not
//on the list.
uint8_t playList_isValidSong(char* name)
{
    uint8_t valid = 0;

    PlayListItem *ptr = gPlayListHead;

    while (ptr->next != NULL)
    {
        //search for the instance where the name matches
        //the song name and set valid = 1.
        if (!strcmp(ptr->songName, name))
            valid = 1;

        ptr = ptr->next;
    }

    //final item - end of the list
    if (!strcmp(ptr->songName, name))
        valid = 1;

    return valid;
}

////////////////////////////////////////////////
void playList_setPlayMode(PlayMode MODE)
{
    gPlayListMode = MODE;
}

////////////////////////////////////////////////
PlayMode playList_getPlayMode(void)
{
    return gPlayListMode;
}

////////////////////////////////////////////////
PlayMode playList_TogglePlayMode(void)
{
    if (gPlayListMode == PLAY_MODE_SINGLE)
        gPlayListMode = PLAY_MODE_CONTINUOUS;
    else
        gPlayListMode = PLAY_MODE_SINGLE;

    return gPlayListMode;
}



///////////////////////////////////////////////
//LCD Display update in between songs.
//
void playList_displayUpdate(void)
{
    uint8_t size;
    uint8_t buffer[32];

    LCD_Clear(0x00);

    if (gPlayListMode == PLAY_MODE_SINGLE)
        LCD_DrawStringKern(0, 3, "Single");
    else
        LCD_DrawStringKern(0, 3, "Continuous");

    size = snprintf(buffer, 32, "  %s", gPlayListPreviousSong->songName);
    LCD_DrawStringKernLength(2, 3, (uint8_t*)buffer, size);

    size = snprintf(buffer, 32, ":>> %s", gPlayListCurrentSong->songName);
    LCD_DrawStringKernLength(4, 3, (uint8_t*)buffer, size);

    size = snprintf(buffer, 32, "  %s", gPlayListNextSong->songName);
    LCD_DrawStringKernLength(6, 3, (uint8_t*)buffer, size);
}




