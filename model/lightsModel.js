const mongoose = require('mongoose');

// Define the schema for a switch event
const SwitchEventSchema = new mongoose.Schema({
  switch: { type: String, required: true },
  state: { type: String, required: true },
  timestamp: { type: Date, default: Date.now }
});

// Create the model
const SwitchEventModel = mongoose.model('SwitchEvent', SwitchEventSchema, "switchevents");

module.exports = SwitchEventModel;
