#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>

unsigned int count = 0;
static struct lws *web_socket = NULL;

#define EXAMPLE_RX_BUFFER_LENGTH (512)

struct per_session_example{
    unsigned char data[EXAMPLE_RX_BUFFER_LENGTH];
};

static int callback_example( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
	switch( reason )
	{
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			lws_callback_on_writable( wsi );
			break;

		case LWS_CALLBACK_CLIENT_RECEIVE:
			/* Handle incomming messages here. */
			break;

		case LWS_CALLBACK_CLIENT_WRITEABLE:
		{
			unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + EXAMPLE_RX_BUFFER_LENGTH + LWS_SEND_BUFFER_POST_PADDING];
			unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
            unsigned char *start = p;
            memset(start, 0, EXAMPLE_RX_BUFFER_LENGTH);
            size_t length = 0;
            for(unsigned int i=0;i<EXAMPLE_RX_BUFFER_LENGTH;i++) {
                //size_t n = sprintf((char)&p[i], "%u", i);
                //count++;
                //std::cout << "n = " << n << std::endl;
                //length += n;
                length += sprintf((char*)p, "%c", 'a');
                p++;
            }
            //for(int i=0;i<10;i++){
            //    length += sprintf((char*)p, "%c", 'a');
            //    p++;
            //}
			//lws_write( wsi, p, n, LWS_WRITE_TEXT );
            std::cout << "length = " << length << std::endl;
            lws_write(wsi, start, length, LWS_WRITE_TEXT);
			break;
		}

		case LWS_CALLBACK_CLOSED:
		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			web_socket = NULL;
			break;

		default:
			break;
	}

	return 0;
}

enum protocols
{
	PROTOCOL_EXAMPLE = 0,
	PROTOCOL_COUNT
};

static struct lws_protocols protocols[] =
{
	{
		"example-protocol",
		callback_example,
		0,
        EXAMPLE_RX_BUFFER_LENGTH,
        //sizeof(per_session_example),
	},
	{ NULL, NULL, 0, 0 } /* terminator */
};

int main( int argc, char *argv[] )
{
	struct lws_context_creation_info info;
	memset( &info, 0, sizeof(info) );

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;
	info.gid = -1;
	info.uid = -1;

	struct lws_context *context = lws_create_context( &info );

	time_t old = 0;
	while( 1 )
	{
		struct timeval tv;
		gettimeofday( &tv, NULL );

		/* Connect if we are not connected to the server. */
		if( !web_socket && tv.tv_sec != old )
		{
			struct lws_client_connect_info ccinfo = {0};
			ccinfo.context = context;
			ccinfo.address = "192.168.0.4";
			ccinfo.port = 7100;
			ccinfo.path = "/";
			ccinfo.host = lws_canonical_hostname( context );
			ccinfo.origin = "origin";
			ccinfo.protocol = protocols[PROTOCOL_EXAMPLE].name;
			web_socket = lws_client_connect_via_info(&ccinfo);
		}

		if( tv.tv_usec/1000 != old )
		{
			/* Send a random number to the server every second. */
			lws_callback_on_writable( web_socket );
			//old = tv.tv_sec;
            old = tv.tv_usec/1000;
		}

		lws_service( context, /* timeout_ms = */ 250 );
	}

	lws_context_destroy( context );

	return 0;
}
