#ifndef _swarm_h
#define _swarm_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "peerwire.h"
#include "protocol_meta.h"

/*
typedef struct
{
	char url [MAX_URL_LEN];
	int scrape_completed, scrape_incomplete, scrape_downloaded, announce_interval, announce_minterval;
} tracker_t;

//ing = local, ed = remote
typedef struct 
{
	int sockfd;
	int choked, choking; 
	int interested, interesting;
	char ip[21];
	char port[6];
	char* peer_id;		//pointers to swarm_t data. (required for threading.)
	char* info_hash;
	pthread_t thread;
} peer_t;

//swarms contain all swarm-connected peers, built from tracker queries.
//before every scrape, clear scrape data and repopulate.
 typedef struct
 {
 	int taken;
 	tracker_t tracker 	[MAX_TRACKERS];
 	peer_t peer 		[MAX_SWARM_SIZE];
 	char  peer_id   	[21];
 	char  info_hash 	[21];
 	int listenport, peercount, sockfd;
 	pthread_t thread;
 	pthread_mutex_t lock;
 } swarm_t;

 swarm_t swarm[MAX_SWARMS];
 */

 //return a free swarm
int swarm_select(char* info_hash, char* trackers[MAX_TRACKERS]);
//lock the mutex of swarm index
void swarm_lock(int index);
//unlock the mutex of swarm index
void swarm_unlock(int index);
//clear the swarm, before announcing to get rid of stale peers.
void swarm_reset(swarm_t* swarm);
//sets the swarm to listen mode
void swarm_listen(swarm_t* swarm);
//release the swarm, tracker stopped or failure.
void swarm_release(swarm_t* swarm);

#endif