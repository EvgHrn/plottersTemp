import express from 'express';
import bodyParser from 'body-parser';
import mongoose from 'mongoose';
import sum from 'sugar/array/sum';
//import floor from 'sugar/number/floor';

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
app.set('port', process.env.PORT || 3000);

app.get('/', (req, res) => {
    res.render('home');
});

app.get('/input', (req, res) => {
  res.render('input');
});

app.post('/results', (req, res) => {
  console.log(req.body.usestartTime);
  console.log(req.body.usestopTime);
  plotterSession.find({"start_time": { "$gte": req.body.usestartTime , "$lte": req.body.usestopTime }}, (err, docs) => {
    if (err) {
      console.log(err);
    }
    let sum1 = docs.filter((obj) => {
      return (obj.plotter === 1);
    });
    sum1 = sum1.map((psession) => {
      return psession.meters;
    });
    let sum1f = sum(sum1);
    sum1f = parseFloat(sum1f);
    sum1f = sum1f.toFixed(2);
    let sum2 = docs.filter((obj) => {
      return (obj.plotter === 2);
    });
    sum2 = sum2.map((psession) => {
      return psession.meters;
    });
    let sum2f = sum(sum2);
    sum2f = parseFloat(sum2f);
    sum2f = sum2f.toFixed(2);
    let sumAll = parseFloat(sum2f) + parseFloat(sum1f);
    sumAll =  parseFloat(sumAll);
    sumAll = sumAll.toFixed(2);
    console.log(sum1f);
    console.log(sum2f);
    console.log(sumAll);
    res.render('home', { 'sum1': sum1f, 'sum2': sum2f, 'sumAll': sumAll});
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
