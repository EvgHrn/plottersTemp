import express from 'express';
import bodyParser from 'body-parser';
import mongoose from 'mongoose';

mongoose.Promise = global.Promise;
mongoose.connect('mongodb://user1:kasper1988@ds029496.mlab.com:29496/plottersdb_test', (err, database) => {
  if (err) return console.log(err);
  app.listen(app.get('port'), () => {
    console.log('Express started on port ' + app.get('port'));
  });
});

let plotterSession = mongoose.model('plotterSession', {
  id: Number,
  plotter: Number,
  start_time: Date,
  stop_time: Date,
  passes: Number,
  meters: Number,
});

let app = express();
let handlebars = require('express-handlebars').create({ defaultLayout: 'main' });

app.use(bodyParser.urlencoded({ extended: true }));
app.engine('handlebars', handlebars.engine);
app.set('view engine', 'handlebars');
app.set('port', process.env.PORT || 3001);

app.get('/', (req, res) => {
  plotterSession.find((err, docs) => {
    if (err) {
      console.log(err);
    }
    res.render('home', { 'plotterSessions': docs });
  });
});

app.get('/input', (req, res) => {
  res.render('input');
});

app.get('/quotes', (req, res) => {
  // console.log(req.body);
  console.log(req.body);
  let session = new plotterSession({
    id: req.body.id,
    plotter: req.body.plotter,
    start_time: req.body.startTime,
    stop_time: req.body.stopTime,
    passes: req.body.passes,
    meters: req.body.meters,
  });
  session.save((err) => {
    if (err) {
      console.log(err);
    }
    else {
      console.log('plotterSession saved');
    }
  });

  res.redirect('/');
});
