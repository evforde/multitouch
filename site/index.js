var ws = new WebSocket("ws://localhost:8765");

var DEFAULT_VALUES = [105, 116, 106, 105, 105, 105, 105, 115, 105, 105, 104, 104, 109, 121, 110, 110, 109, 109, 107, 117, 107, 108, 106, 106, 106, 117, 107, 107, 106, 106, 108, 116, 107, 107, 106, 105, 106, 113, 103, 103, 103, 103, 106, 116, 105, 105, 105, 105, 104, 115, 104, 104, 104, 104, 103, 114, 104, 103, 103, 102, 103, 115, 103, 104, 102, 103, 101, 112, 102, 102, 101, 101];

var MAX_DIFFERENCE = 20;
var MIN_OPACITY = 0.2;

ws.onopen = function () {
  ws.send("This is the browser!");
};

ws.onmessage = function (e) {
  let readings = JSON.parse(e.data);
  if (readings.length != DEFAULT_VALUES.length) {
    console.log("Strange reading...", readings);
    return;
  }
  for (let i = 0; i < readings.length; i++) {
    let row = Math.trunc(i / 6);
    let column = Math.trunc(i % 6);
    let difference = DEFAULT_VALUES[i] - readings[i];
    /*if (difference > 3)
      difference = 8;
    else
      difference = 0;*/
    difference = Math.min(difference, MAX_DIFFERENCE);
    difference = difference * difference;
    let opacity = Math.max(difference / (MAX_DIFFERENCE * MAX_DIFFERENCE), 0) + MIN_OPACITY;
    opacity = Math.min(opacity, 1);
    let selector = ".row:nth(" + row + ") .cell:nth(" + column +")";
    $(selector).css("opacity", opacity);
    $(selector).css("transform", "translateZ(" + opacity * 100 + "px)");
  }
};

console.log(ws);
