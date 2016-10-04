import express from 'express';
import bodyParser from 'body-parser';
import mongoose from 'mongoose';
import sum from 'sugar/array/sum';
import zip from 'sugar/array/zip';
import unique from 'sugar/array/unique';
//import format from 'sugar/date/format';
import moment from 'moment';
import quiche from 'quiche';
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
  plotterSession.find({"start_time": { "$gte": start , "$lte": stop }}, (err, docs) => {
    if (err) {
      console.log(err);
    }
    //console.log(docs);
    let sum1 = parseFloat(calcSumMeters(1, docs));
    let sum2 = parseFloat(calcSumMeters(2, docs));
    let sumAll = (sum2 + sum1).toFixed(2);
    //let d3dataday = calcDayPeriod(1, docs);
    let days1 = calcDayPeriod_days(1, docs);
    let meters1 = calcDayPeriod_meters(1, docs);
    let days2 = calcDayPeriod_days(2, docs);
    let meters2 = calcDayPeriod_meters(2, docs);

    let bar = new quiche('bar');
     bar.setWidth(400);
     bar.setHeight(265);
     bar.setTitle('');
     bar.setBarStacked(); // Stacked chart
     bar.setBarWidth(0);
     bar.setBarSpacing(6); // 6 pixles between bars/groups
     bar.setLegendBottom(); // Put legend at bottom
     bar.setTransparentBackground(); // Make background transparent
     bar.addData(meters1, 'Плоттер 1', '00AB6F');
     bar.addData(meters2, 'Плоттер 2', 'FF9700');
     bar.setAutoScaling(); // Auto scale y axis
     bar.addAxisLabels('x', days1);
     var imageUrl = bar.getUrl(true); // First param controls http vs. https

     res.render('homeGoogle', { 'sum1': sum1, 'sum2': sum2, 'sumAll': sumAll, 'dataarr': d3dataday});
    //res.render('home', { 'sum1': sum1, 'sum2': sum2, 'sumAll': sumAll, 'chartUrl': imageUrl});
  });
});

app.post('/oneday', (req, res) => {
  let date = req.body.useDay;
  let isodatestart = new Date(date + "T00:00:00.000Z");
  let isodatestop = new Date(date + "T23:59:59.000Z");
  plotterSession.find( {"start_time": { "$gte": isodatestart, "$lte": isodatestop }}, (err, docs) => {
    if (err) {
      console.log(err);
    }
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
};


let calcDayPeriod = (pl, docs) => {
  let sums = [];                                           //массив сумм каждого дня
  let docs_filtered = docs.filter((session) => {
    return (session.plotter === pl);
  });
  let startDatesTimes = docs_filtered.map((session) => {
    return session.start_time;
  });
  //console.log(startDatesTimes);
  let startDates = startDatesTimes.map((dateTime) => {
    return moment(dateTime).format("YYYY-MM-DD");
  });
  let startDates_unique = startDates.filter((item, i, arr) => {
    return (arr.indexOf(item) === i);
  });
  startDates_unique.forEach((date) => {
    let isodatestart = new Date(date + "T00:00:00.000Z");
    let isodatestop = new Date(date + "T23:59:59.000Z");
    let daySessions = docs.filter((session) => {
      return ((session.start_time >= isodatestart) && (session.start_time <= isodatestop));
    });
    sums.push( parseFloat(calcSumMeters(pl, daySessions)) );
  });
  let result = zip(startDates_unique, sums);
  console.log(result);
  return result;
};

let calcDayPeriod_days = (pl, docs) => {
  let sums = [];                                           //массив сумм каждого дня
  let docs_filtered = docs.filter((session) => {
    return (session.plotter === pl);
  });
  let startDatesTimes = docs_filtered.map((session) => {
    return session.start_time;
  });
  //console.log(startDatesTimes);
  let startDates = startDatesTimes.map((dateTime) => {
    return moment(dateTime).format("YYYY-MM-DD");
  });
  let startDates_unique = startDates.filter((item, i, arr) => {
    return (arr.indexOf(item) === i);
  });
  startDates_unique.forEach((date) => {
    let isodatestart = new Date(date + "T00:00:00.000Z");
    let isodatestop = new Date(date + "T23:59:59.000Z");
    let daySessions = docs.filter((session) => {
      return ((session.start_time >= isodatestart) && (session.start_time <= isodatestop));
    });
    sums.push( parseFloat(calcSumMeters(pl, daySessions)) );
  });
  let result = zip(startDates_unique, sums);
  return startDates_unique;
};

let calcDayPeriod_meters = (pl, docs) => {
  let sums = [];                                           //массив сумм каждого дня
  let docs_filtered = docs.filter((session) => {
    return (session.plotter === pl);
  });
  let startDatesTimes = docs_filtered.map((session) => {
    return session.start_time;
  });
  //console.log(startDatesTimes);
  let startDates = startDatesTimes.map((dateTime) => {
    return moment(dateTime).format("YYYY-MM-DD");
  });
  let startDates_unique = startDates.filter((item, i, arr) => {
    return (arr.indexOf(item) === i);
  });
  startDates_unique.forEach((date) => {
    let isodatestart = new Date(date + "T00:00:00.000Z");
    let isodatestop = new Date(date + "T23:59:59.000Z");
    let daySessions = docs.filter((session) => {
      return ((session.start_time >= isodatestart) && (session.start_time <= isodatestop));
    });
    sums.push( parseFloat(calcSumMeters(pl, daySessions)) );
  });
  let result = zip(startDates_unique, sums);
  return sums;
};

let calcMonthPeriod = (pl, docs) => {
  let sums = [];                                           //массив сумм каждого месяца
  let docs_filtered = docs.filter((session) => {            //фильтр под определённый плоттер
    return (session.plotter === pl);
  });
  let startDatesTimes = docs_filtered.map((session) => {    //получаем массив дат начала печати
    return session.start_time;
  });
  //console.log(startDatesTimes);
  let startMonths = startDatesTimes.map((dateTime) => {      //вычленяем только годы-месяцы
    return moment(dateTime).format("YYYY-MM");
  });
  let startMonths_unique = startMonths.filter((item, i, arr) => { //избавляемся от дубликатов
    return (arr.indexOf(item) === i);                               //в итоге у нас есть массив месяцев
  });
  startMonths_unique.forEach((date) => {                       //получаем суммы метров по каждому месяцу
    let lastDay;
    let month = moment(date).format("MM");
    switch ()
    let isodatestart = new Date(date + "-01" + "T00:00:00.000Z");
    let isodatestop = new Date(date + "T23:59:59.000Z");
    let daySessions = docs.filter((session) => {
      return ((session.start_time >= isodatestart) && (session.start_time <= isodatestop));
    });
    sums.push( parseFloat(calcSumMeters(pl, daySessions)) );
  });
  let result = zip(startMonths_unique, sums);
  return result;
};
