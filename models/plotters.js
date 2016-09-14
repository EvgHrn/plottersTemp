import mongoose from 'mongoose';

let plottersSchema = mongoose.Schema({
  id: Number,
  plotter: Number,
  start_time: Date,
  stop_time: Date,
  passes: Number,
  meters: Number,
});

plottersSchema.methods.getPrintTime = () => {
  return (this.start_time - this.stop_time);
};

let printSession = mongoose.model('printSession', plottersSchema);

module.exports = printSession;
