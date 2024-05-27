const express = require('express');
const mongoose = require('mongoose');
const bodyParser = require('body-parser');
const connectDB = require('./database');
const SwitchEvent = require('./models/lightsModel');

const app = express();

connectDB();

app.use(bodyParser.json());

mongoose.connect('mongodb://localhost:27017/switchEvents', {
  useNewUrlParser: true,
  useUnifiedTopology: true,
});

app.post('/api/switch-events', async (req, res) => {
  try {
    const { switch: switchName, state, timestamp } = req.body;
    const switchEvent = new SwitchEvent({
      switch: switchName,
      state,
      timestamp: new Date(parseInt(timestamp)),
    });

    await switchEvent.save();
    res.status(201).send('Event saved');
  } catch (err) {
    res.status(500).send('Server Error');
  }
});

app.get('/api/switch-events', async (req, res) => {
  try {
    const events = await SwitchEvent.find().sort({ timestamp: -1 });
    res.json(events);
  } catch (err) {
    res.status(500).send('Server Error');
  }
});

const PORT = process.env.PORT || 5000;
app.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${port}`);
});