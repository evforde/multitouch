var ws = new WebSocket("ws://localhost:8765");

var DEFAULT_VALUES = [
  38, 38, 42, 39, 38, 40, 41, 39, 43, 39, 39, 38, 43, 43, 47, 43, 43, 44]

var MAX_DIFFERENCE = 8;
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
    let opacity = Math.max(difference / MAX_DIFFERENCE, 0) + MIN_OPACITY;
    opacity = Math.min(opacity, 1);
    let selector = ".row:nth(" + row + ") .cell:nth(" + column +")";
    $(selector).css("opacity", opacity);
    $(selector).css("transform", "translateZ(" + opacity * 100 + "px)");
  }
};

console.log(ws);
