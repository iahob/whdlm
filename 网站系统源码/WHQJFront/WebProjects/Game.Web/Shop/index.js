const fs = require("fs-extra");
const fextract = require("files-extractor");
const eventproxy = require("eventproxy");
const STATUS = fextract.STATUS;
const path = require("path");
const DATE_TOMORROW = new Date(
  new Date().getFullYear(),
  new Date().getMonth(),
  new Date().getDate() + 1
);
const optsDist = {
  start: new Date(2017, 0, 1),
  end: DATE_TOMORROW,
  files: `vue/?(dist)/**/*`,
  output: "../../../../网站发布/WHQJFront/",
  terminDirName: `Shop`
};

const copyDist = function(cb) {
  let dirObj = getDirObj(optsDist);
  let copy = [];
  let x = fextract(optsDist);
  x.on("message", msg => {
    switch (msg.status) {
      case STATUS.FAILED:
        console.error(msg.data);
        return cb(err);
      case STATUS.WARNING:
        console.warn(msg.data);
        break;
      case STATUS.BOOTSTRAP:
      case STATUS.SEARCHED:
      case STATUS.FILTERED:
        if (Array.isArray(msg.data)) {
          msg.data.forEach(p => {
            let obj = {
              src: path.join(dirObj.originDirPath, p),
              dest: path.join(dirObj.originDirPath, p.replace("vue/dist/", ""))
            };
            copy.push(obj);
          });
        }
        break;
      case STATUS.EXTRACTED:
        let result = null;
        let epRCReady = new eventproxy();
        epRCReady.once("ready", ready => {
          if (fs.existsSync(dirObj.originDirPath)) {
            fs.removeSync(path.join(dirObj.originDirPath, "vue"));
            fs.copySync(dirObj.originDirPath, dirObj.terminDirPath, {
              preserveTimestamps: true
            });
            fs.removeSync(dirObj.originDirPath);
            result = {
              name: dirObj.terminDirName,
              path: dirObj.terminDirPath
            };
            cb(null, result);
          }
        });
        if (copy.length > 0) {
          let epRCMove = new eventproxy();
          epRCMove.after("moved", copy.length, moved => {
            epRCReady.emit("ready", true);
          });
          copy.forEach(obj => {
            fs.copySync(obj.src, obj.dest, {
              preserveTimestamps: true
            });
            epRCMove.emit("moved", true);
          });
        } else {
          epRCReady.emit("ready", true);
        }
        break;
    }
  });
};
/**
 * Pad start string with 0
 *
 * @param {String} value
 * @param {Number} length
 * @param {String} pad
 * @returns {String}
 */
const pad = String.prototype.padStart
  ? function(value, length, pad) {
      return String.prototype.padStart.call(String(value), length, pad || "0");
    }
  : function(value, length, pad) {
      value = String(value);
      length = length >> 0;
      pad = pad || "0";

      if (value.length > length) {
        return value;
      } else {
        length = length - value.length;

        if (length > pad.length) {
          pad += pad.repeat(length / pad.length);
        }

        return pad.slice(0, length) + value;
      }
    };
/**
 * Format date
 *
 * @param {Date} date
 * @returns {String}
 */
const formatDate = function(date, easy = false) {
  let year = date.getFullYear();
  let month = pad(date.getMonth() + 1, 2);
  let day = pad(date.getDate(), 2);
  let hour = pad(date.getHours(), 2);
  let minutes = pad(date.getMinutes(), 2);
  let seconds = pad(date.getSeconds(), 2);

  return easy
    ? `${year}${month}${day}`
    : `${year}-${month}-${day} ${hour}.${minutes}.${seconds}`;
};
const getDirObj = function(opts) {
  // console.info(opts.start.toISOString(),opts.end.toISOString());
  let originDirName = formatDate(opts.start) + " & " + formatDate(opts.end);
  let terminDirName = opts.terminDirName;
  let originDirPath = path.join(__dirname, opts.output, originDirName);
  let terminDirPath = originDirPath.replace(originDirName, terminDirName);
  return {
    originDirName: originDirName,
    terminDirName: terminDirName,
    originDirPath: originDirPath,
    terminDirPath: terminDirPath
  };
};
copyDist((err, result) => {
  console.info("copyDist finished");
});
