const WebSocketServer = require('ws').Server;
const wss = new WebSocketServer({ port: 9998 });
const inputFilename = __dirname + '/log_input.txt';
const { watchInputFile, sendDataToSocket } = require('./helpers');

wss.on('connection', (ws) => {
  watchInputFile(inputFilename, (newItems) => {
    newItems.map((data) => sendDataToSocket({ data, ws }));
  });

  ws.on('message', (message) => {
    console.log(message);
  });
});