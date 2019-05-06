var EXAMPLE_RX_BUFFER_LENGTH = 512; // byte
//var DEFAULT_DOMAIN = "127.0.0.1";
var DEFAULT_DOMAIN = "192.168.0.4";
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
                labels: [],
                datasets: [{
                    label:'temp data',
                    data: [],
                    backgroundColor: 'rgba(255,0,0, 0)',
                    borderColor: 'rgba(255,0,0,1)',
                }]
            },
                options: {
                    elements: {
                        line: {
                            tension: 0,
                        },
                        point:{
                            radius: 0
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
                                //min: -10,
                                max: 20000 
                            }
                        }]
                    },
                    responsive: true,
                    maintainAspectRatio: true,
                    animation: false
                } 
            });
        for(i=0;i<128*2;++i){
            chart.data.labels.push(i);
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
    //var nameH8 = new Uint8Array(e_data, 0, 128);
    var h32 = new Uint32Array(e_data, 0, 128*2);

    var data = [];
    for(i=0;i<h32.length;i+=2){
        data.push(h32[i]);
    }

    //var moji = stringOfUint8Array(nameH8);
    var moji = "";
    return [moji, data];
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

function drawChart(input){
    for(i=0;i<input.length;++i){ // from Uint32Array to 64bit integer
        //chart.data.datasets[0].data[i] = getRandomInt(-10,10);
        chart.data.datasets[0].data[i] = input[i];
    }
    chart.update();
}

ws.onopen = function() { console.log("socket open");};
ws.onclose = function() { console.log("socket closed");};

ws.onmessage = function(e){
    if(typeof e.data === "string"){
        console.log(e);
    } else {
        console.log(e);
        var[message, h32] = update_data(e.data);
        drawChart(h32);
        //document.getElementById("num").innerHTML = message;
        //document.getElementById("h8").innerHTML = h8String;
        //document.getElementById("h16").innerHTML = h16String;
        //document.getElementById("h32").innerHTML = h32String;
    }
}
