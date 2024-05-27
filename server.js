const express = require('express');
const bodyParser = require('body-parser');
const mongoose = require('mongoose');
const connectDB = require('./database');
const SwitchEvent = require('./model/lightsModel'); // Ensure correct path to lightsModel.js

const app = express();
const port = 3000;

connectDB();

mongoose.connect('mongodb://127.0.0.1/switchEvents', {
  useNewUrlParser: true,
  useUnifiedTopology: true,
}).then(() => {
  console.log('Connected to MongoDB');
}).catch(err => {
  console.error('Error connecting to MongoDB', err);
});

app.use(bodyParser.json());

app.post('/api/switchEvents', async (req, res) => {
  try {
    const switchEvent = new SwitchEvent({
      switch: req.body.switch,
      state: req.body.state,
      timestamp: new Date()
    });

    const result = await switchEvent.save();
    res.status(200).send(`Event inserted with ID: ${result._id}`);
  } catch (error) {
    console.error(error);
    res.status(500).send('Error inserting event');
  }
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}/`);
});
