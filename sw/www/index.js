var EXAMPLE_RX_BUFFER_LENGTH = 512; // byte
var DEFAULT_DOMAIN = "192.168.0.4";
var DEFAULT_PORT = 7100;

var ws = new WebSocket("ws://" + DEFAULT_DOMAIN + ":" + DEFAULT_PORT, "example-protocol");
ws.binaryType = "arraybuffer";

function stringOfUint8Array(u8){
    var ret = "";
    for(i=0;i<u8.length;++i){
        var x = u8[i];
        if(x === 0){
            break;
        }
        ret += String.fromCharCode(x);
    }
    return ret;
}

function update_data(e_data){
    var h16 = new Uint16Array(e_data,0,2);
    var data0 = h16[0];
    var data1 = h16[1];
    var h8 = new Uint8Array(e_data, 2*2, EXAMPLE_RX_BUFFER_LENGTH);

    var moji = new stringOfUint8Array(h8);
    return [data0, data1, moji];
}

function update_test(e_data){
    var h8 = new Uint8Array(e_data, 0, 128);
    var moji = stringOfUint8Array(h8);
    return moji;
}

ws.onopen = function() { console.log("socket open");};
ws.onclose = function() { console.log("socket closed");};

ws.onmessage = function(e){
    //var[data0, data1, moji] = update_data(e.data);
    //console.log("socket data:  " + data0 + "  :  " + data1 + "  :  " + moji);
    //console.log("socket data: " + update_test(e.data));
    if(typeof e.data === "string"){
        console.log(e);
    } else {
        var message = update_test(e.data);
        console.log("socket data: " + message);
        document.getElementById("num").innerHTML = message;
    }
}
