
#include "ibxm.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define SAMPLING_FREQ 48000 /* 48khz. */
#define REVERB_BUF_LEN 4800 /* 50ms. */
#define BUFFER_SAMPLES 2048 /* 8k buffer. */

static short reverb_buffer[ REVERB_BUF_LEN ];
static int mix_buffer[ SAMPLING_FREQ / 3 ];
static int mix_len, mix_idx, reverb_len, reverb_idx, loop, display;
static int samples_remaining, redraw_event, song_end_event, mute;

/* Simple stereo cross delay with feedback. */
static void reverb( short *buffer, int count ) {
	int buffer_idx, buffer_end;
	if( reverb_len > 2 ) {
		buffer_idx = 0;
		buffer_end = buffer_idx + ( count << 1 );
		while( buffer_idx < buffer_end ) {
			buffer[ buffer_idx ] = ( buffer[ buffer_idx ] * 3 + reverb_buffer[ reverb_idx + 1 ] ) >> 2;
			buffer[ buffer_idx + 1 ] = ( buffer[ buffer_idx + 1 ] * 3 + reverb_buffer[ reverb_idx ] ) >> 2;
			reverb_buffer[ reverb_idx ] = buffer[ buffer_idx ];
			reverb_buffer[ reverb_idx + 1 ] = buffer[ buffer_idx + 1 ];
			reverb_idx += 2;
			if( reverb_idx >= reverb_len ) {
				reverb_idx = 0;
			}
			buffer_idx += 2;
		}
	}
}

static void clip_audio( int *input, short *output, int count ) {
	int idx, end, sam;
	for( idx = 0, end = count << 1; idx < end; idx++ ) {
		sam = input[ idx ];
		if( sam < -32768 ) {
			sam = -32768;
		}
		if( sam > 32767 ) {
			sam = 32767;
		}
		output[ idx ] = sam;
	}
}

static void get_audio( struct replay *replay, short *buffer, int count ) {
	int buf_idx = 0, remain;
	while( buf_idx < count ) {
		if( mix_idx >= mix_len ) {
			mix_len = replay_get_audio( replay, mix_buffer, mute );
			mix_idx = 0;
		}
		remain = mix_len - mix_idx;
		if( ( buf_idx + remain ) > count ) {
			remain = count - buf_idx;
		}
		clip_audio( &mix_buffer[ mix_idx << 1 ], &buffer[ buf_idx << 1 ], remain );
		mix_idx += remain;
		buf_idx += remain;
	}
}

static void audio_callback( struct replay *replay, short *stream, int len ) {
	// printf("audio_callback %d\n", len);
	int count = len / 4;
	if( samples_remaining < count ) {
		/* Clear output.*/
		memset( stream, 0, len );
		count = samples_remaining;
	}
	if( count > 0 ) {
		/* Get audio from replay.*/
		get_audio( replay, stream, count );
		reverb( stream, count );
		if( !loop ) {
			samples_remaining -= count;
		}
	}
}
