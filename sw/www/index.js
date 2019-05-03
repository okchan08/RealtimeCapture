var EXAMPLE_RX_BUFFER_LENGTH = 512; // byte
var DEFAULT_DOMAIN = "127.0.0.1";
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

function arrayToStringMessage(data){
    var ret = "";
    for(i=0;i<data.length;i++){
        var x = data[i] + '';
        ret += x + " ";
    }
    return ret;
}

function update_data(e_data){
    var h8 = new Uint8Array(e_data, 0, 16);
    var h16 = new Uint16Array(e_data, 16, 16);
    var h32 = new Uint32Array(e_data, 16 + 16*2, 16);
    var nameH8 = new Uint8Array(e_data, 16 + 16*2 + 16*4, 128);

    var moji = stringOfUint8Array(nameH8);
    return [h8, h16, h32, moji];
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
        console.log(e);
        var[h8, h16, h32, message] = update_data(e.data);
        var h8String = arrayToStringMessage(h8);
        var h16String = arrayToStringMessage(h16);
        var h32String = arrayToStringMessage(h32);
        document.getElementById("num").innerHTML = message;
        document.getElementById("h8").innerHTML = h8String;
        document.getElementById("h16").innerHTML = h16String;
        document.getElementById("h32").innerHTML = h32String;
    }
}
