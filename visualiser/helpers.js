const fs = require('fs');

const prepareStringOutput = (data) => {
  return `${data.length}:${data}`;
};

const sendDataToSocket = ({ data, ws }) => {
  const preparedData = prepareStringOutput(data);

  return ws.send(preparedData);
};

const watchInputFile = (filename, cb) => {
  let prevLength = 0;

  const processFile = () => {
    const allContent = fs.readFileSync(filename)
      .toString()
      .split("\n");
    const sliceLength = prevLength > allContent.length ? 0 : prevLength;
    const newContent = allContent.slice(sliceLength)
      .filter((item, i, arr) => (
        (arr[i - 1] && arr[i - 1].indexOf('in:') === 0)
      ))
      .map((item) => {
        // delete redundant information for the client
        if (item.indexOf('state') > 0) {
          try {
            const parsed = JSON.parse(item);
            parsed.state = undefined;

            return JSON.stringify(parsed);
          } catch (e) {
            // ...
          }
        }

        return item;
      });
    prevLength = allContent.length;

    return cb(newContent)
  };

  setInterval((() => {
    let lastChangedTime;

    return () => {
      const stats = fs.statSync(filename);

      if (stats.atimeMs !== lastChangedTime) {
        lastChangedTime = stats.atimeMs;
        processFile();
      }
    };
  })(), 200);
};

module.exports = {
  watchInputFile,
  sendDataToSocket
};
