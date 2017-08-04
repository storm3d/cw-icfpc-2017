const fs = require('fs');

const prepareStringOutput = (data) => {
  return `${data.length}:${data}`;
};

const sendDataToSocket = ({ data, ws }) => {
  const preparedData = prepareStringOutput(data);

  return ws.send(preparedData);
};

const watchInputFile = (filename, cb) => {
  let previousContent = [];

  const processFile = () => {
    const allContent = fs.readFileSync(filename)
      .toString()
      .split("\n");
    const newDataTOffset = previousContent.length < allContent.length ? previousContent.length : 0;
    previousContent = allContent;

    return cb(allContent.slice(newDataTOffset))
  };

  processFile();
  fs.watchFile(filename, processFile);
};

module.exports = {
  watchInputFile,
  sendDataToSocket
};
