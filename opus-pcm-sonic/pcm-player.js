/*
 * @Author: xiuquanxu
 * @Company: kaochong
 * @Date: 2020-03-30 22:47:53
 * @LastEditors: xiuquanxu
 * @LastEditTime: 2020-07-28 16:15:00
 */
function PcmPlayer(config) {

  this.config = !!config ? config : {
    bufferSize: 1024,
  };
  this.audioContext = new (window.AudioContext || window.webkitAudioContext)();
  this.scriptNode = null;
  this.gainNode = null;
  this.bufferQue = new Float32Array(0);
}

PcmPlayer.prototype.play = function() {
  const _this = this;
  this.scriptNode = this.audioContext.createScriptProcessor(this.config.bufferSize, 1, 1);
  this.gainNode = this.audioContext.createGain();
  this.scriptNode.connect(this.gainNode);
  this.gainNode.connect(this.audioContext.destination);
  this.scriptNode.onaudioprocess = function(e) {
    if (_this.bufferLength()) {
      e.outputBuffer.getChannelData(0).set(_this.read(_this.config.bufferSize));
    } else {
      console.log('silence');
      // e.outputBuffer.getChannelData(0).set(_this.silence);
    } 
  }
}

PcmPlayer.prototype.read = function(len) {
  const outBuffer = this.bufferQue.subarray(0, len);
  const inBuffer = this.bufferQue.subarray(len, this.bufferQue.length);
  this.bufferQue = inBuffer;
  return outBuffer;
}

PcmPlayer.prototype.write = function(buffer) {
  // TODO: 此处可以使用循环队列实现，可能减少内存拷贝次数
  const len = buffer.length + this.bufferQue.length;
  const newBuffer = new Float32Array(len);
  newBuffer.set(this.bufferQue, 0);
  newBuffer.set(buffer, this.bufferQue.length);
  this.bufferQue = newBuffer;
}

PcmPlayer.prototype.bufferLength = function() {
  return this.bufferQue.length;
}