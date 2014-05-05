 //entry point in tracker thread.


#include "peerwire.h"
#include "swarm.h"

/* todo:
 * pseudo
 * for every piece in swarm, tell every peer at handshake with have message.
 * for piece download, for pieces not existent, trylock and download
 * for piece upload, for pieces have, trylock copy unlock and upload.
 */

/*
	PIECE INDEXES ARE ZERO-BASED
	BLOCK SPECIFIES BYTE OFFSET IN PIECE
	DROP PACKETS WITH INVALID LENGTH

	MESSAGE HEADER
		4 BYTES LENGHT SPECIFIER
		1 BYTE MESSAGE TYPE
		4 BYTE INDEX | VARIABLE SIZE PAYLOAD

	HEADER MESSAGES
		choke: 			<len=0001><id=0> 						
		unchoke: 		<len=0001><id=1>						
		interested: 	<len=0001><id=2>					
		not interested: <len=0001><id=3>				
		have: 			<len=0005><id=4><piece index>
		request:		<len=0013><id=6><piece index><begin offset><requested length, piece len?>

	PAYLOAD MESSAGES
		piece: 	<len=0009+X><id=7><index><begin><block> 
		cancel: <len=0013>  <id=8><index><begin><length>		
		port: 	<len=0003>  <id=9><listen-port>
*/

#define PROTOCOL    "BitTorrent protocol"
#define CHOKE 		0
#define UNCHOKE 	1
#define INTERESTED 	2
#define NOT_INTERESTED 3
#define HAVE 		4
#define PIECE       7
#define CANCEL      8
#define PORT        9

	#include <openssl/sha.h>


//finds a peer in the swarm with piece_id. (todo: ignore peers who are choking client), in swarm or peerwire?
/*
peer_t* piecefinder(swarm_t* swarm, int piece_id)
{
	int i, j, peer_index;

	//for all peers in swarm
	for (i = 0; i < swarm->peercount; i++)
	{
		//for all pieces in peer
		for (j = 0; j < swarm->peer[i].piece_count; j++)
			if (swarm->peer[i].piece[j] == piece_id)
			{
				return swarm->peer[i].sockfd;
			}
	}

	return NULL;
}
*/

//locks X amount of peers from unconnected in swarm, contact them and find their pieces
//set cooldown on their
void ping()
{

}

void handshake(peer_t* peer, char info_hash[20], char peer_id[20])
{
	//handshake
	int payload = 0;
    struct addrinfo hints, *res;
    unsigned char protocol_len = strlen(PROTOCOL);
    unsigned char reserved[8];
    char* request = malloc(1 + protocol_len + 8 + 20 + 20);

    memset(reserved, 0, 8);
    reserved[5] = 16;
    reserved[7]  = 5;

    sprintf(request, "%c%s", protocol_len, PROTOCOL);	payload += 1 + protocol_len;
    memcpy(request + payload, reserved, 8);				payload += 8;
    memcpy(request + payload, info_hash, 20);			payload += 20;
    memcpy(request + payload, peer_id, 20);				payload += 20;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(peer->ip, peer->port, &hints, &res);

    //if sock open, close first.
    if (peer->sockfd != 0)
    	close(peer->sockfd);

    if ((peer->sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) > -1)
    {
        if (connect(peer->sockfd, res->ai_addr, res->ai_addrlen) > -1)
        {
            send(peer->sockfd, request, payload, 0);	//strlen will find the reserved byte.
            //receive?
        } 
    }
    free(request);
}


//<len=0013><id=6><piece index><begin offset><requested length, piece len?>
void request(peer_t* peer, int piece_index, int offset_begin, int offset_length)
{
	int payload = 0, len = htonl(13);
	unsigned char id = 6;
    char* request = malloc(4 + 1 + 4 + 4 + 4);

    memcpy(request, &len, 4);						payload += 4;
    memcpy(request + payload, &id, 1);				payload += 1;
    memcpy(request + payload, &piece_index,  4);	payload += 4;
    memcpy(request + payload, &offset_begin, 4);	payload += 4;
    memcpy(request + payload, &offset_length,4);	payload += 4;

    send(peer->sockfd, request, payload, 0);
    free(request);
}



//message [choke, unchoke, interested, not interested]
void message(peer_t* peer, unsigned char message)
{
	int payload = 0, len = htonl(1);
    char* request = malloc(1 + 1);

    memcpy(request, &len, 4);						payload += 4;
    memcpy(request + payload, &message, 1);			payload += 1;

    send(peer->sockfd, request, payload, 0);		
    free(request);
}

void have(peer_t* peer)
{
	int payload, len = htonl(5);
	unsigned char id = 4;
	char* request = malloc(4 + 1 + 4);

	//for every piece have in swarm
	//{
	payload = 0;
	memcpy(request, &len, 4); 								payload += 4;
	memcpy(request + payload, &id, 1);						payload += 1;
	//memcpy(request + payload, &swarm->piece[x].index, 4); payload += 4;
	send(peer->sockfd, request, payload, 0);	
	//}


	free(request);
}
                                                                         

/*
void main(void)
{
	peer_t peer;
	char info_hash[20];
	char peer_id[20];

	printf("-----"); fflush(stdout);


	// gcc sha1Openssl.c -o sha1Openssl -lssl -lcrypto

	strcpy(peer_id, "NSA-PirateBust-05Ac7");
	strcpy(peer.port, "6881");
	strcpy(peer.ip,   "192.168.0.10");

	//sprintf(info_hash, "%x", "d15b9f7471d78dd64f1419d630a8c48d708924dd");

	info_hash[0] = 0xf4;
	info_hash[1] = 0x3e;
	info_hash[2] = 0x6d;
	info_hash[3] = 0x2b;
	info_hash[4] = 0x91;
	info_hash[5] = 0x3f;
	info_hash[6] = 0x22;
	info_hash[7] = 0xc3;
	info_hash[8] = 0xb0;
	info_hash[9] = 0x61;
	info_hash[10] = 0x25;
	info_hash[11] = 0x95;
	info_hash[12] = 0xf0;
	info_hash[13] = 0x25;
	info_hash[14] = 0xb1;
	info_hash[15] = 0x25;
	info_hash[16] = 0x2a;
	info_hash[17] = 0x99;
	info_hash[18] = 0x85;
	info_hash[19] = 0xdf;


	//strcpy(info_hash, hash);
	//strcpy(peer_id,   hash);

	//OOO
	//handshake -> extended/bitfield/have -> interested/not interested -> unchoke/choke -> request/piece(reply)


	//piece transfer over TCP.
	handshake(&peer, info_hash, peer_id);
	sleep(1);
	message(&peer, INTERESTED);
	sleep(6);
	request(&peer, htonl(0), htonl(0), htonl(16384));
	//message(&peer, CHOKE);
	//message(&peer, NOT_INTERESTED);
	//message(&peer, UNCHOKE);
	//message(&peer, INTERESTED);
	sleep(30);
}
*/

S