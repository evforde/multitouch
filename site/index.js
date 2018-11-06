var ws = new WebSocket("ws://localhost:8765");

// map from index to [row, column]
var READING_IDX_TO_CELL = [
  [7, 0],
  [7, 1],
  [6, 0],
  [6, 1],
  [5, 0],
  [5, 1],
  [4, 0],
  [4, 1],
  [2, 0],
  [2, 1],
  [3, 0],
  [3, 1],
  [0, 0],
  [0, 1],
  [1, 0],
  [1, 1],
]

var DEFAULT_VALUES = [
  24, 26, 24, 26, 23, 24, 24, 25, 23, 25, 25, 25, 24, 14, 25, 25];

var MAX_DIFFERENCE = 8;
var MIN_OPACITY = 0.2;

ws.onopen = function () {
  ws.send("This is the browser!");
};

ws.onmessage = function (e) {
  let readings = JSON.parse(e.data);
  if (readings.length != 16) {
    console.log("Strange reading...", readings);
    return;
  }
  for (let i = 0; i < readings.length; i++) {
    let cell = READING_IDX_TO_CELL[i];
    let row = cell[0];
    let column = cell[1];
    let difference = DEFAULT_VALUES[i] - readings[i];
    let opacity = Math.max(difference / MAX_DIFFERENCE, 0) + MIN_OPACITY;
    opacity = Math.min(opacity, 1);
    let selector = ".row:nth(" + row + ") .cell:nth(" + column +")";
    $(selector).css("opacity", opacity);
    $(selector).css("transform", "translateZ(" + opacity * 100 + "px)");
  }
};

console.log(ws);
