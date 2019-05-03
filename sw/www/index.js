var EXAMPLE_RX_BUFFER_LENGTH = 512; // byte
var DEFAULT_DOMAIN = "127.0.0.1";
var DEFAULT_PORT = 7100;

var ws = new WebSocket("ws://" + DEFAULT_DOMAIN + ":" + DEFAULT_PORT, "example-protocol");
ws.binaryType = "arraybuffer";

var canvas = null;
var ctx = null;
var chart = null;

function init_canvas(){
    canvas = document.getElementById('sample');
    if(canvas.getContext){
        ctx = canvas.getContext('2d');
        chart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: new Array(100),
                datasets: [{
                    label:'temp data',
                    data: [],
                }]
            },
                options: {
                    elements: {
                        line: {
                            tension: 0
                        }
                    },
                    scales: {
                        xAxes: [{
                            ticks:{
                                beginAtZero: true,
                                min: 0,
                                max: 100
                            }
                        }],
                        yAxes: [{
                            ticks:{
                                beginAtZero: true,
                                min: 0,
                                max: 12
                            }
                        }]
                    }
                } 
            });
        for(i=0;i<100;++i){
            chart.data.datasets[0].data.push(0);
        }
    }
}

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

function getRandomInt(min, max) {
  min = Math.ceil(min);
  max = Math.floor(max);
  return Math.floor(Math.random() * (max - min)) + min; //The maximum is exclusive and the minimum is inclusive
}

function copy_data(dst_data, length, src_data){
    for(i=0;i<length;++i){
        dst_data[i] = src_data[i];
    }
}

function drawChart(){
    var data_x = [];
    var data_y = [];
    var length = 100;
    for(i=0;i<length;++i){
        data_x.push(i);
        //data_y.push(getRandomInt(0,10));
        data_y.push(i);
        chart.data.datasets[0].data[i] = getRandomInt(0,10);
    }
    chart.update();
}

ws.onopen = function() { console.log("socket open");};
ws.onclose = function() { console.log("socket closed");};

ws.onmessage = function(e){
    drawChart();
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
