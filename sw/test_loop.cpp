#include <limits>		// std::numeric_limits
#include <stdexcept>	// std::runtime_error
#include <iostream>
#include <list>

#include <string>		// std::string
#include <stdio.h>		// sprintf
#include <string.h>
#include <errno.h>		// errno
#include <sys/stat.h>	// struct stat.
#include <sys/types.h>

#include <libwebsockets.h>
#include <uv.h>
#include "WebsocketBuffer.h"

std::list<WebsocketBuffer*> connections;

int callback_http( struct lws *wsi, enum lws_callback_reasons reason,
                   void *user, void*in, size_t len){
    switch(reason){
        case LWS_CALLBACK_HTTP:
            lws_serve_http_file(wsi, "index.html", "text/html", NULL, 0);
            break;
        default:
            break;
    }
    return 0;
}

int callback_test( struct lws *wsi, enum lws_callback_reasons reason,
                   void *user, void *in, size_t len){
    switch(reason){
        case LWS_CALLBACK_ESTABLISHED:
            connections.push_front(new WebsocketBuffer(wsi));
        case LWS_CALLBACK_RECEIVE:
            std::cout << "callback receive" << std::endl;
            break;
        case LWS_CALLBACK_SERVER_WRITEABLE:
            std::cout << "callback server writeable" << std::endl;
            break;
        default:
            break;
    }
    return 0;
}

void timer_loop(uv_timer_t *w){
    std::cout << "timer_loop" << std::endl;
}

/// List of the protocols supported.
enum server_protocols {
	/* always first */
	PROTOCOL_HTTP = 0,

    // test
	PROTOCOL_TEST,

	/* always last */
	PROTOCOL_COUNT
};

struct per_session_data_http{
    unsigned char buf[4096];
    int fd;
};

static struct lws_protocols protocols[] = {
    {
        "http-only",  // name
        callback_http, // callback function
        sizeof(per_session_data_http), // per_session_data_size
        0,
    },
    {
        "test-loop",
        callback_test,
        0,
        10,
    },
    { NULL, NULL, 0, 0} // terminator
};


int main(int argc, char **argv){
    //uv_loop_t *loop = uv_default_loop();
    uv_timer_t timer1;
    //uv_timer_init(loop,&timer1);

	struct lws_context_creation_info info;
	memset( &info, 0, sizeof(info) );

	info.port = 7100;
	info.protocols = protocols;
	info.gid = -1;
	info.uid = -1;
    //info.max_http_header_pool = 1;
    //info.timeout_secs = 5;
    //info.options = LWS_SERVER_OPTION_LIBUV;

    lwsl_notice("info initialize complete\n");

    struct lws_context *context = lws_create_context(&info);
    if(context == nullptr){
        std::cerr << "libwebsocket init failed" << std::endl;
        return -1;
    }

    //if(lws_uv_initloop(context, NULL, 0)){
    //    std::cerr << "lws_uv_initloop failed " << std::endl;
    //    return -1;
    //}
    //uv_timer_start(&timer1, &timer_loop, 0, 1000);
    //elops_init_pt_uv(context, NULL, 0);
    //uv_timer_init(lws_uv_getloop(context, 0), &timer1);
    //uv_timer_init(&context->pt[0]->uv.io_loop, &timer1);

    uv_loop_t *loop = uv_default_loop();
    uv_timer_init(loop, &timer1);
    
    std::cout << "b" << std::endl;
    uv_timer_start(&timer1, timer_loop, 0, 1000);

    //std::cout << "main func" << std::endl;

    lws_context_destroy(context);
    return uv_run(loop, UV_RUN_DEFAULT);
}
