import express from 'express';
import bodyParser from 'body-parser';
import mongoose from 'mongoose';
import sum from 'sugar/array/sum';
//import floor from 'sugar/number/floor';

mongoose.Promise = global.Promise;
let connectionString = 'mongodb://user1:' + process.env.MONGOPASS + '@ds029496.mlab.com:29496/plottersdb_test';
mongoose.connect(connectionString, (err, database) => {
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
app.set('port', process.env.PORT || 3000);

app.get('/', (req, res) => {
    res.render('home');
});

app.get('/input', (req, res) => {
  res.render('input');
});

app.get('/oneday', (req, res) => {
    res.render('oneday');
});

app.post('/results', (req, res) => {
  let start = req.body.usestartTime;
  let stop = req.body.usestopTime;
  let period = req.body.period;
  console.log(start);
  console.log(stop);
  plotterSession.find({"start_time": { "$gte": start , "$lte": stop }}, (err, docs) => {
    if (err) {
      console.log(err);
    }
    console.log(docs);
    let sum1 = parseFloat(calcSumMeters(1, docs));
    let sum2 = parseFloat(calcSumMeters(2, docs));
    let sumAll = (sum2 + sum1).toFixed(2);
    res.render('home', { 'sum1': sum1, 'sum2': sum2, 'sumAll': sumAll});
  });
});

app.post('/oneday', (req, res) => {
  let date = req.body.useDay;
  console.log(date);
  let isodatestart = new Date(date + "T00:00:00.000Z");
  let isodatestop = new Date(date + "T23:59:59.000Z");
  console.log(isodatestart);
  console.log(isodatestop);
  plotterSession.find( {"start_time": { "$gte": isodatestart, "$lte": isodatestop }}, (err, docs) => {
    if (err) {
      console.log(err);
    }
    console.log(docs);
    let sum1 = parseFloat(calcSumMeters(1, docs));
    let sum2 = parseFloat(calcSumMeters(2, docs));
    let sumAll = (sum2 + sum1).toFixed(2);
    res.render('oneday', { 'sum1': sum1, 'sum2': sum2, 'sumAll': sumAll});
  });
});

app.post('/quotes', (req, res) => {
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

let calcSumMeters = (plotter, someDocs) => {
  let result  = sum((someDocs.filter((obj) => {
    return (obj.plotter === plotter);
  })).map((psession) => {
    return psession.meters;
  }));
  result = parseFloat(result);
  result = result.toFixed(2);
  return result;
}
