const express = require('express');
const bodyParser = require('body-parser');
// const mongoose = require('mongoose');
const connectDB = require('./database');
const SwitchEvent = require('./model/lightsModel'); // Ensure correct path to lightsModel.js
const SwitchEventModel = require('./model/lightsModel');

const app = express();
const port = 3000;

connectDB();


app.use(bodyParser.json());

// app.get('/api/switchEvents', async (req, res) => {
//   try {
//     const events = await SwitchEvent.find().sort({ timestamp: -1 }); 
//     res.json(events);
//   } catch (error) {
//     console.error(error);
//     res.status(500).send('Error retrieving events');
//   }
// });

// app.post('/api/switchEvents', async (req, res) => {
//   try {
//     const SwitchEvent = await new SwitchEvent({
//       switch: req.body.switch,
//       state: req.body.state,
//       timestamp: new Date()
//     });

//     const result = await SwitchEvent.save();
//     res.status(200).send(`Event inserted with ID: ${result._id}`);
//   } catch (error) {
//     console.error(error);
//     res.status(500).send('Error inserting event', error);
//     // console.error('Error inserting event:', error);
//   }
// });


app.get('/api/switchEvents', async (req, res) => {
  try {
    const events = await SwitchEventModel.find().sort({ timestamp: -1 });
    res.json(events);
  } catch (error) {
    console.error(error);
    res.status(500).send('Error retrieving events');
  }
});

app.post('/api/switchEvents', async (req, res) => {
  try {
    const newSwitchEvent = new SwitchEventModel({
      switch: req.body.switch,
      state: req.body.state,
      timestamp: new Date()
    });

    const result = await newSwitchEvent.save();
    res.status(200).send(`Event inserted with ID: ${result._id}`);
  } catch (error) {
    console.error(error);
    res.status(500).send('Error inserting event', error);
  }
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}/`);
});
