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
  console.log('got request');
  let start, stop, period;
  let meters1, meters2, meters3, meters4, meters5;
  let periodForChart;
  let elementForCheck;
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

  plotterSession.find({"start_time": { "$gte": start , "$lte": stop }}, (err, docs) => {
    if (err) {
      console.log(err);
    }
    let maxDatasOnChart = 5;
    let sum1 = parseFloat(calcSumMeters(1, docs));
    let sum2 = parseFloat(calcSumMeters(2, docs));
    let sum3 = parseFloat(calcSumMeters(3, docs));
    let sum4 = parseFloat(calcSumMeters(4, docs));
    let sum5 = parseFloat(calcSumMeters(5, docs));
    let sumAll = (sum2 + sum1 + sum3 + sum4 + sum5).toFixed(2);

    if (period === 'day') {
      let days = getDays(start, stop);
      meters1 = getMetersDays(1, days, docs);
      meters2 = getMetersDays(2, days, docs);
      meters3 = getMetersDays(3, days, docs);
      meters4 = getMetersDays(4, days, docs);
      meters5 = getMetersDays(5, days, docs);
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
      periodForChart = daysForChart;
      elementForCheck = "day";
    }

    if (period === 'week') {
      let weeks = getWeeks(start, stop);
      meters1 = getMetersWeeks(1, weeks, docs);
      meters2 = getMetersWeeks(2, weeks, docs);
      meters3 = getMetersWeeks(3, weeks, docs);
      meters4 = getMetersWeeks(4, weeks, docs);
      meters5 = getMetersWeeks(5, weeks, docs);
      let weeksLength = weeks.length;
      let chartDatesStep = Math.round(weeksLength/maxDatasOnChart);
      let i = 0;
      let weeksForChart = weeks;
      while (i < weeksLength) {
        if ((i % chartDatesStep) !== 0) {
          weeksForChart[i] = '';
        }
        i++;
      }
      periodForChart = weeksForChart;
      elementForCheck = "week";
    }

    if (period === 'month') {
      let months = getMonths(start, stop);
      console.log('months', months);
      meters1 = getMetersMonths(1, months, docs);
      meters2 = getMetersMonths(2, months, docs);
      meters3 = getMetersMonths(3, months, docs);
      meters4 = getMetersMonths(4, months, docs);
      meters5 = getMetersMonths(5, months, docs);

      let monthsLength = months.length;
      let chartDatesStep = Math.round(monthsLength/maxDatasOnChart);
      let i = 0;
      let monthsForChart = months;
      while (i < monthsLength) {
        if ((i % chartDatesStep) !== 0) {
          monthsForChart[i] = '';
        }
        i++;
      }
      periodForChart = monthsForChart;
      elementForCheck = "month";
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
     bar.addData(meters1, 'Плоттер 1', 'D11141');
     bar.addData(meters2, 'Плоттер 2', '00B159');
     bar.addData(meters3, 'Плоттер 3', '00AEDB');
     bar.addData(meters4, 'Плоттер 4', 'F37735');
     bar.addData(meters5, 'Плоттер 5', 'FFC425');
     bar.setAutoScaling(); // Auto scale y axis
     bar.addAxisLabels('x', periodForChart);
     var imageUrl = bar.getUrl(true); // First param controls http vs. https
     res.render('home', { 'sum1': sum1, 'sum2': sum2, 'sum3': sum3, 'sum4': sum4, 'sum5': sum5, 'sumAll': sumAll, 'chartUrl': imageUrl, 'startCookie': req.session.start, 'stopCookie': req.session.stop, 'element': elementForCheck});
  });
});

app.get('/input', (req, res) => {
  res.render('input');
});

app.get('/oneday', (req, res) => {
    res.render('oneday');
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

let getMetersDays = (pl, days, docs) => {
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

 let getMetersMonths = (pl, months, docs) => {
   let sums = [];
   let docs_definite_plotter = docs.filter((session) => {
     return (session.plotter === pl);
   });
  months.forEach((month) => {
    let isodatestart = moment(month).startOf('month').format();
    let isodatestop = moment(month).endOf('month').format();
    console.log('start of month', isodatestart);
    console.log('end of month', isodatestop);
    let monthSession = docs_definite_plotter.filter((session) => {
       return ((session.start_time >= isodatestart) && (session.start_time <= isodatestop));
     });
     sums.push( parseFloat( calcSumMeters(pl, monthSession)) );
   });
   console.log('meters in months', sums);
   return sums;
  };

  let getMetersWeeks = (pl, weeks, docs) => {
    let sums = [];
    let docs_definite_plotter = docs.filter((session) => {
      return (session.plotter === pl);
    });
   weeks.forEach((week) => {
     let isodatestart = moment(week).format();
     let isodatestop = moment(week).endOf('w').format();
     let weekSession = docs_definite_plotter.filter((session) => {
        return ((session.start_time >= isodatestart) && (session.start_time <= isodatestop));
      });
      sums.push( parseFloat( calcSumMeters(pl, weekSession)) );
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

let getDays = (s, f) => {
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

let getMonths = (s, f) => {
  let startDateTime = moment(s);
  let stopDateTime = moment(f);
  let startDate = moment(startDateTime).startOf('month').format();
  let stopDate = moment(stopDateTime).endOf('month').format();
  let months = [startDate];
  let i = 1;
  let newdate;

  while (moment(months[months.length - 1]).isBefore(stopDate)) {
    newdate = moment(startDate).add(i, 'M').format();
    months.push (newdate);
    i++;
  }
  // do {
  //   newdate = moment(startDate).add(i, 'M').format();
  //   months.push (newdate);
  //   i++;
  // } while (moment(months[months.length - 1]).isBefore(stopDate))
  // //} while (months[months.length - 1] < stopDate)

  return months;
};

let getWeeks = (s, f) => {
  let startDateTime = moment(s);
  let stopDateTime = moment(f);
  let startDate = moment(startDateTime).startOf('week').format("YYYY-MM-DD");
  let stopDate = moment(stopDateTime).endOf('week').format("YYYY-MM-DD");
  let weeks = [];
  let i = 0;
  let newdate;
  do {
    newdate = moment(startDate).add(i, 'w').format("YYYY-MM-DD");
    weeks.push (newdate);
    i++;
  } while (weeks[weeks.length - 1] !== stopDate)

  return weeks;
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
