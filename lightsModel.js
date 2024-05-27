const mongoose = require('mongoose');

const SwitchEventSchema = new mongoose.Schema({
  switch: String,
  state: String,
  timestamp: Date,
});

const LightsModel = mongoose.model('SwitchEvent', SwitchEventSchema);

module.exports = LightsModel;