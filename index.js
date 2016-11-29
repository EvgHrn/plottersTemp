import express from 'express';
import bodyParser from 'body-parser';
//import path from 'path';
import mongoose from 'mongoose';
import sum from 'sugar/array/sum';
import zip from 'sugar/array/zip';
import last from 'sugar/array/last';
import unique from 'sugar/array/unique';
import moment from 'moment';
import quiche from 'quiche';
import session from 'express-session';
import multer from 'multer';
import xlsx from 'xlsx';
//import "babel-polyfill";

var FileStore = require('session-file-store')(session);

var mult_storage = multer.diskStorage({
  destination: function (req, file, cb) {
    cb(null, './uploads/')
  },
  filename: function (req, file, cb) {
      cb(null, file.originalname);
  }
});

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

app.use(express.static('public'));
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
      //let d = moment().format();
      start = moment().startOf('isoWeek').format("YYYY-MM-DDTHH:mm");               //start of week
      stop = moment().endOf('isoWeek').format("YYYY-MM-DDTHH:mm");               //finish of week
      period = 'day';
      req.session.start = start;              //set new cookies
      req.session.stop = stop;
      req.session.period = period;
    }
  }

  console.log('start, stop time from request\n', start);
  console.log(stop);

  start = moment(start).utc().format();
  stop = moment(stop).utc().format();

  console.log('UTC start, stop time from request\n', start);
  console.log(stop);

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
      //console.log('days ', days);
      meters1 = getMetersDays(1, days, docs);
      meters2 = getMetersDays(2, days, docs);
      meters3 = getMetersDays(3, days, docs);
      meters4 = getMetersDays(4, days, docs);
      meters5 = getMetersDays(5, days, docs);
      let daysLength = days.length;
      let chartDatesStep = Math.ceil(daysLength/maxDatasOnChart);

      //console.log('chartDatesStep', chartDatesStep);

      let i = 0;
      let daysForChart = days;
      while (i < daysLength) {
        if ((i % chartDatesStep) !== 0) {
          daysForChart[i] = '';
        }
        i++;
      }
      //console.log('daysForChart', daysForChart);
      periodForChart = daysForChart;
      elementForCheck = "day";
    }

    if (period === 'week') {
      let weeks = getWeeks(start, stop);
      //console.log('weeks ', weeks);
      meters1 = getMetersWeeks(1, weeks, docs);
      meters2 = getMetersWeeks(2, weeks, docs);
      meters3 = getMetersWeeks(3, weeks, docs);
      meters4 = getMetersWeeks(4, weeks, docs);
      meters5 = getMetersWeeks(5, weeks, docs);
      let weeksLength = weeks.length;
      let chartDatesStep = Math.round(weeksLength/maxDatasOnChart);
      let i = 0;
      let weeksForChart = weeks.map((week) => {
         return (moment(week).format('DD MMM YYYY'));
       });
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
      //console.log('months', months);
      meters1 = getMetersMonths(1, months, docs);
      meters2 = getMetersMonths(2, months, docs);
      meters3 = getMetersMonths(3, months, docs);
      meters4 = getMetersMonths(4, months, docs);
      meters5 = getMetersMonths(5, months, docs);

      let monthsLength = months.length;
      let chartDatesStep = Math.round(monthsLength/maxDatasOnChart);
      let i = 0;
      let monthsForChart = months.map((month) => {
         return (moment(month).format('MMM YYYY'));
       });
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

app.post('/quotes', (req, res) => {
  // console.log(req.body);
  //console.log(req.body);
  let session = new plotterSession({
    id: req.body.id,
    plotter: req.body.plotter,
    start_time: req.body.startTime,
    stop_time: req.body.stopTime,
    passes: req.body.passes,
    meters: req.body.meters,
  });
  console.log('new data received\n', session);
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

app.get('/compare', (req, res) => {
  let error = '';
  let reports = getReport();
  console.log('reports\n', reports);
  let dates = reports.map((obj) => {
    let date = moment(obj['Дата'], "DD-MM-YY").format();
    if (! moment(date).isValid()) {
      error = 'ОШИБКА: неверная дата' + date;
      date = moment().format();
    }
    return date;
  });

  let datesMeters = reports.map((obj) => {
    let date = moment(obj['Дата'], "DD-MM-YY").format();
    let long = obj['Длина'];
    if (long === undefined) {
      error = 'ОШИБКА: не указана длина\nвставлено 0 м';
      long = '0';
    }
    if (! moment(date).isValid()) {
      error = 'ОШИБКА: неверная дата\n' + date;
      date = moment().format();
    }
    return {'Дата': moment(date).format(), 'Длина': long};
  });
  //console.log('datesMeters\n', datesMeters);
  let datesMetersUnique = datesMeters.reduce((arrResult, obj) => {
    arrResult[obj['Дата']] = (arrResult[obj['Дата']] || 0) + parseFloat(obj['Длина']);
    return arrResult;
  }, []);
  let datesMetersUnique_new = [];
  for (var key in datesMetersUnique) {
    datesMetersUnique_new.push({'Дата': key, 'МетрыОтчёт': datesMetersUnique[key]});
  }

  console.log('datesMeters\n', datesMeters);
  console.log('datesMetersUnique\n', datesMetersUnique);
  console.log('datesMetersUnique_new\n', datesMetersUnique_new);
  //console.log('dates\n', dates);
  dates.sort((a, b) => {
    if (moment(a).isBefore(moment(b))) return -1;
    if (moment(a).isAfter(moment(b))) return 1;
  });
  //console.log('dates sorted\n', dates);
  let mini = moment(dates[0]).startOf('day').format();
  let maxi = moment(dates[dates.length - 1]).endOf('day').format();
  console.log('start', mini);
  console.log('stop', maxi);
  let toShow = [];
  plotterSession.find({"start_time": { "$gte": mini , "$lte": maxi }}, function (err, docs){
    toShow = datesMetersUnique_new.map((jsonItem) => {
    let metersSensor = parseFloat(getMetersDays(1, getDays(moment(jsonItem.Дата).format(), moment(jsonItem.Дата).format()), docs))
                      + parseFloat(getMetersDays(2, getDays(moment(jsonItem.Дата).format(), moment(jsonItem.Дата).format()), docs))
                      + parseFloat(getMetersDays(3, getDays(moment(jsonItem.Дата).format(), moment(jsonItem.Дата).format()), docs))
                      + parseFloat(getMetersDays(4, getDays(moment(jsonItem.Дата).format(), moment(jsonItem.Дата).format()), docs))
                      + parseFloat(getMetersDays(5, getDays(moment(jsonItem.Дата).format(), moment(jsonItem.Дата).format()), docs));
      metersSensor = metersSensor.toFixed(2);
      console.log('metersSensor\n',metersSensor);
      return {'Дата': moment(jsonItem.Дата).format("DD.MM.YY"), 'МетрыОтчёт': (parseFloat(jsonItem.МетрыОтчёт)).toFixed(2), 'МетрыДатчик': metersSensor, 'Разница': (parseFloat(metersSensor) - parseFloat(jsonItem.МетрыОтчёт)).toFixed(2)};
    });
    //console.log('new reports', reports);
    console.log('toShow\n', toShow);
    res.render('compare', {'report': toShow, 'error': error});
  });
});

app.post('/upload', multer({storage: mult_storage,  dest: './uploads/'}).array('upl', 5), (req, res) => {
  console.log(req.file);
  res.redirect('/compare');
});

let getMetersDays = (pl, days, docs) => {
  let sums = [];
  //console.log('docs\n', docs);
  let docs_definite_plotter = docs.filter((session) => {
    return (session.plotter === pl);
  });
 days.forEach((day) => {
   let isodatestart = new Date(day + "T00:00:00.000Z");
   let isodatestop = new Date(day + "T23:59:59.000Z");
   let daySession = docs_definite_plotter.filter((session) => {
      return ((session.start_time >= isodatestart) && (session.stop_time <= isodatestop));
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
    let monthSession = docs_definite_plotter.filter((session) => {
       return ((moment(session.start_time).isAfter(isodatestart)) && (moment(session.stop_time).isBefore(isodatestop)));
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
     let isodatestart = moment(week).startOf('isoWeek').format();
     let isodatestop = moment(week).endOf('isoWeek').format();
     let weekSession = docs_definite_plotter.filter((session) => {
        return ((moment(session.start_time).isAfter(isodatestart)) && (moment(session.stop_time).isBefore(isodatestop)));
      });
      sums.push( parseFloat( calcSumMeters(pl, weekSession)) );
    });
    console.log('meters in weeks', sums);
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
  let startDate = moment(s).startOf('month').format();
  let stopDate = moment(f).endOf('month').format();
  let months = [];
  months.push (startDate);
  let i = 0;
  let newdate;

  while (moment(months[months.length - 1]).endOf('month').isBefore(stopDate)) {
    i++;
    newdate = moment(startDate).add(i, 'M').format();
    months.push (newdate);
  }
  return months;
};

let getWeeks = (s, f) => {
  let startDate = moment(s).startOf('isoWeek').format();
  let stopDate = moment(f).endOf('isoWeek').format();
  let weeks = [];
  weeks.push(startDate);
  let i = 0;
  let newdate;
  while (moment(weeks[weeks.length - 1]).endOf('isoWeek').isBefore(stopDate)) {
    i++;
    newdate = moment(startDate).add(i, 'w').format();
    weeks.push(newdate);
  }
  return weeks;
};

let getReportFromXls = (plotter) => {
  let xlsPath = './uploads/plotter' + plotter + '.xls';
  console.log('xlsPath\n', xlsPath);
  let workbook = xlsx.readFile(xlsPath);
  let sheet_name_list = workbook.SheetNames;
  let report = xlsx.utils.sheet_to_json(workbook.Sheets[sheet_name_list[0]]);
  return report;
};

let getReport = () => {
  let report1 = getReportFromXls(1);
  let report2 = getReportFromXls(2);
  let report3 = getReportFromXls(3);
  let report4 = getReportFromXls(4);
  let report5 = getReportFromXls(5);
  return report1.concat(report2, report3, report4, report5);
};



function getMetersFromTo (start, stop){
  //console.log ('In getMetersFromTo');
  let meters;
  plotterSession.find({"start_time": { "$gte": start , "$lte": stop }}, function (err, docs){
    let metersArr = docs.map((doc) => {
      return doc.meters;
    });
    meters = parseFloat(sum(metersArr));
    meters = meters.toFixed(2);
    console.log('meters', meters);
  });
  return meters;
};

let errorHandler = (message) => {

};
