let paused = false;
let timeoutId;
let isMessageProcessing = false;

const processedMessages = [];
const queuedMessages = [];
const freeEdgeColor = '#009';

const pause = () => {
  paused = true;
  clearInterval(timeoutId);
  $('#pause').html('>');
};

const resume = () => {
  paused = false;
  handleMessageQueue();
  $('#pause').html('| |');
};

const prev = () => {
  if (paused === false) {
    pause();
  }

  if (processedMessages.length) {
    const prevMessage = processedMessages.pop();
    undo(prevMessage);
    queuedMessages.unshift(prevMessage);
  }
};

const next = () => {
  if (paused === false) {
    pause();
  }

  handleOneMessageFromQueue();
};

const undo = (message) => {
  const msg = JSON.parse(message.data.split(/:(.+)/)[1]);

  if (msg && msg.move && msg.move.moves) {
    msg.move
      .moves
      .map((move) => {
        if (move.claim) {
          dropEdgeOwner(move.claim.source, move.claim.target);
        }
      });
  }
};

const dropEdgeOwner = (source, target) => {
  const es = getEdge({ source, target });

  if (es.length > 0) {
    const e = es[0];
    e.data()['owner'] = -1;
    e.style('line-color', freeEdgeColor);
  }

  rmLastFromLog();
};

const sendMessageToQueue = (message) => {
  queuedMessages.push(message);
};

const handleMessageQueue = () => {
  timeoutId = setInterval(handleOneMessageFromQueue, 1);
};

const handleOneMessageFromQueue = () => {
  if (isMessageProcessing === true) {
    return;
  }

  if (queuedMessages.length) {
    const message = queuedMessages.shift();

    isMessageProcessing = true;
    const start = +new Date();
    handleMessage(message);
    processedMessages.push(message);
    const finish = +new Date();
    console.log(finish - start);
    isMessageProcessing = false;
  }
};

const getEdge = ({ source, target }) => {
  let es = cy.edges("[source=\"" + source + "\"][target=\"" + target + "\"]");
  if (es.length <= 0) {
    es = cy.edges("[source=\"" + target + "\"][target=\"" + source + "\"]");
  }

  return es;
};

handleMessageQueue();

$('#pause').click(() => (paused === true) ? resume() : pause());
$('#prev').click(prev);
$('#next').click(next);