#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <vector>

#include "DataCapture.h"

unsigned int count = 0;
static struct lws *web_socket = NULL;

#define EXAMPLE_RX_BUFFER_LENGTH (2048)
#define LENGTH (16)

struct per_session_example{
    unsigned char data[EXAMPLE_RX_BUFFER_LENGTH];
};

template<class T>
std::vector<T> generateData(){
    std::vector<T> ret(LENGTH);
    for(int i=0;i<LENGTH;i++){
        ret.at(i) = i;
    }
    return ret;
}

template<class T>
int getVectorLengthInUint8(const std::vector<T> &vec) {
    return vec.size() * sizeof(T);
}

DataCapture dc(128,0);

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
			//unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
            //unsigned char *start = p;
            //memset(start, 0, EXAMPLE_RX_BUFFER_LENGTH);
            //size_t length = 0;
            //for(unsigned int i=0;i<EXAMPLE_RX_BUFFER_LENGTH;i++) {
            //    length += sprintf((char*)p, "%c", 'a');
            //    p++;
            //}
            char name[128];
            memset(name, 0, 128);
            sprintf(name, "%s", "sample string");
            //auto data0 = generateData<uint8_t>();
            //auto data1 = generateData<uint16_t>();
            //auto data2 = generateData<uint32_t>();
            memset(&buf[LWS_SEND_BUFFER_PRE_PADDING], 0, EXAMPLE_RX_BUFFER_LENGTH);
            uint8_t uint8_buf[EXAMPLE_RX_BUFFER_LENGTH]; memset(uint8_buf, 0, EXAMPLE_RX_BUFFER_LENGTH);
            int writtenBytes = 0;
            //memcpy(&uint8_buf[writtenBytes], (uint8_t*)&data0[0], data0.size()); writtenBytes += getVectorLengthInUint8(data0);
            //memcpy(&uint8_buf[writtenBytes], (uint8_t*)&data1[0], getVectorLengthInUint8(data1)); writtenBytes += getVectorLengthInUint8(data1);

            //memcpy(&uint8_buf[writtenBytes], (uint8_t*)name, strlen(name)); writtenBytes += strlen(name);
            //memcpy(&uint8_buf[writtenBytes], (uint8_t*)&data2[0], getVectorLengthInUint8(data2)); writtenBytes += getVectorLengthInUint8(data2);
            auto vec = dc.runCapture();
            fprintf(stdout, "%d\n", vec.size());
            memcpy(&uint8_buf[writtenBytes], (uint8_t*)&vec[0], getVectorLengthInUint8(vec)); writtenBytes += getVectorLengthInUint8(vec);

            memcpy(&buf[LWS_SEND_BUFFER_PRE_PADDING], uint8_buf, EXAMPLE_RX_BUFFER_LENGTH);

            lws_write(wsi, &buf[LWS_SEND_BUFFER_PRE_PADDING], EXAMPLE_RX_BUFFER_LENGTH, LWS_WRITE_BINARY);
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

    dc.setLength(64);
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
			//ccinfo.address = "127.0.0.1";
			ccinfo.port = 7100;
			ccinfo.path = "/";
			ccinfo.host = lws_canonical_hostname( context );
			ccinfo.origin = "origin";
			ccinfo.protocol = protocols[PROTOCOL_EXAMPLE].name;
			web_socket = lws_client_connect_via_info(&ccinfo);
		}

        usleep(50 * 1000);
		lws_callback_on_writable( web_socket );
        
		if( tv.tv_sec != old )
		{
			/* Send a random number to the server every second. */
			//lws_callback_on_writable( web_socket );
			old = tv.tv_sec;
		}

		lws_service( context, /* timeout_ms = */ 250 );
	}

	lws_context_destroy( context );

	return 0;
}
