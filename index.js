import express from 'express';
import bodyParser from 'body-parser';
import mongoose from 'mongoose';
import sum from 'sugar/array/sum';
import zip from 'sugar/array/zip';
import last from 'sugar/array/last';
import unique from 'sugar/array/unique';
//import format from 'sugar/date/format';
import moment from 'moment';
import quiche from 'quiche';
import session from 'express-session';
var FileStore = require('session-file-store')(session);
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
app.use(session({
  secret: 'my express secret',
  saveUninitialized: true,
  resave: true,
  store: new FileStore
}));
app.engine('handlebars', handlebars.engine);
app.set('view engine', 'handlebars');
app.set('port', process.env.PORT || 3000);

app.all('/', (req, res) => {
  let start, stop, period;
  //---------- if there is selected date/time---------------
  if (req.body.usestartTime !== undefined){
    start = req.body.usestartTime;
    req.session.start = start;                //save new cookie
    stop = req.body.usestopTime;
    req.session.stop = stop;                  //save new cookie
    period = req.body.period;
    req.session.period = period;              //save new cookie
  } else {
    //---------- if date/time does not selected ---------------
    //---------- if there are cookies -------------------------
    if (req.session.start !== undefined){
      start = req.session.start;          //take cookies for variabled
      stop = req.session.stop;
      period = req.session.period;
    } else {
      //---------if there are not cookies----------------------
      //---------take today for variables----------------------
      let d = moment().format("YYYY-MM-DD");
      start = d + "T00:00";               //start of day
      stop = d + "T23:59";                //finish of day
      period = 'day';
      req.session.start = start;              //set new cookies
      req.session.stop = stop;
      req.session.period = period;
    }
  }
  let days = calcDayPeriod_days(start, stop);

  plotterSession.find({"start_time": { "$gte": start , "$lte": stop }}, (err, docs) => {
    if (err) {
      console.log(err);
    }

    let sum1 = parseFloat(calcSumMeters(1, docs));
    let sum2 = parseFloat(calcSumMeters(2, docs));
    let sumAll = (sum2 + sum1).toFixed(2);

    let meters1 = getMeters(1, days, docs);
    let meters2 = getMeters(2, days, docs);

    let maxDatasOnChart = 5;
    let daysLength = days.length;
    let chartDatesStep = Math.round(daysLength/maxDatasOnChart);
    let i = 0;
    let daysForChart = days;
    while (i < daysLength) {
      if ((i % chartDatesStep) !== 0) {
        daysForChart[i] = '';
      }
      i++;
    }
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
     bar.addAxisLabels('x', daysForChart);
     var imageUrl = bar.getUrl(true); // First param controls http vs. https
     res.render('home', { 'sum1': sum1, 'sum2': sum2, 'sumAll': sumAll, 'chartUrl': imageUrl, 'startCookie': req.session.start, 'stopCookie': req.session.stop});
  });
});

app.get('/input', (req, res) => {
  res.render('input');
});

app.get('/oneday', (req, res) => {
    res.render('oneday');
});

// app.post('/results', (req, res) => {
//   let start = req.body.usestartTime;
//   let stop = req.body.usestopTime;
//   let period = req.body.period;
//   let days = calcDayPeriod_days(start, stop);
//
//   plotterSession.find({"start_time": { "$gte": start , "$lte": stop }}, (err, docs) => {
//     if (err) {
//       console.log(err);
//     }
//
//     let sum1 = parseFloat(calcSumMeters(1, docs));
//     let sum2 = parseFloat(calcSumMeters(2, docs));
//     let sumAll = (sum2 + sum1).toFixed(2);
//
//     let meters1 = getMeters(1, days, docs);
//     let meters2 = getMeters(2, days, docs);
//
//     let maxDatasOnChart = 5;
//     let daysLength = days.length;
//     let chartDatesStep = Math.round(daysLength/maxDatasOnChart);
//     let i = 0;
//     let daysForChart = days;
//     while (i < daysLength) {
//       if ((i % chartDatesStep) !== 0) {
//         daysForChart[i] = '';
//       }
//       i++;
//     }
//     console.log(daysForChart);
//     let bar = new quiche('bar');
//      bar.setWidth(400);
//      bar.setHeight(265);
//      bar.setTitle('');
//      bar.setBarStacked(); // Stacked chart
//      bar.setBarWidth(0);
//      bar.setBarSpacing(6); // 6 pixles between bars/groups
//      bar.setLegendBottom(); // Put legend at bottom
//      bar.setTransparentBackground(); // Make background transparent
//      bar.addData(meters1, 'Плоттер 1', '00AB6F');
//      bar.addData(meters2, 'Плоттер 2', 'FF9700');
//      bar.setAutoScaling(); // Auto scale y axis
//      bar.addAxisLabels('x', daysForChart);
//      var imageUrl = bar.getUrl(true); // First param controls http vs. https
//      res.render('home', { 'sum1': sum1, 'sum2': sum2, 'sumAll': sumAll, 'chartUrl': imageUrl});
//   });
// });

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

let getMeters = (pl, days, docs) => {
  let sums = [];
  let docs_definite_plotter = docs.filter((session) => {
    return (session.plotter === pl);
  });
 days.forEach((day) => {
   let isodatestart = new Date(day + "T00:00:00.000Z");
   let isodatestop = new Date(day + "T23:59:59.000Z");
   let daySession = docs_definite_plotter.filter((session) => {
      return ((session.start_time >= isodatestart) && (session.start_time <= isodatestop));
    });
    sums.push( parseFloat( calcSumMeters(pl, daySession)) );
  });
  return sums;
 };

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

let calcDayPeriod_days = (s, f) => {
  let startDateTime = moment(s);
  let stopDateTime = moment(f);
  let startDate = moment(startDateTime).format("YYYY-MM-DD");
  let stopDate = moment(stopDateTime).format("YYYY-MM-DD");
  let days = [];
  let i = 0;
  let newdate;
  do {
    newdate = moment(startDate).add(i, 'days').format("YYYY-MM-DD");
    days.push (newdate);
    i++;
  } while (days[days.length - 1] !== stopDate)

  return days;
};

// let calcMonthPeriod = (pl, docs) => {
//   let sums = [];                                           //массив сумм каждого месяца
//   let docs_filtered = docs.filter((session) => {            //фильтр под определённый плоттер
//     return (session.plotter === pl);
//   });
//   let startDatesTimes = docs_filtered.map((session) => {    //получаем массив дат начала печати
//     return session.start_time;
//   });
//   //console.log(startDatesTimes);
//   let startMonths = startDatesTimes.map((dateTime) => {      //вычленяем только годы-месяцы
//     return moment(dateTime).format("YYYY-MM");
//   });
//   let startMonths_unique = startMonths.filter((item, i, arr) => { //избавляемся от дубликатов
//     return (arr.indexOf(item) === i);                               //в итоге у нас есть массив месяцев
//   });
//   startMonths_unique.forEach((date) => {                       //получаем суммы метров по каждому месяцу
//     let lastDay;
//     let month = moment(date).format("MM");
//     let isodatestart = new Date(date + "-01" + "T00:00:00.000Z");
//     let isodatestop = new Date(date + "T23:59:59.000Z");
//     let daySessions = docs.filter((session) => {
//       return ((session.start_time >= isodatestart) && (session.start_time <= isodatestop));
//     });
//     sums.push( parseFloat(calcSumMeters(pl, daySessions)) );
//   });
//   let result = zip(startMonths_unique, sums);
//   return result;
// };
