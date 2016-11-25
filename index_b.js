'use strict';

var _express = require('express');

var _express2 = _interopRequireDefault(_express);

var _bodyParser = require('body-parser');

var _bodyParser2 = _interopRequireDefault(_bodyParser);

var _mongoose = require('mongoose');

var _mongoose2 = _interopRequireDefault(_mongoose);

var _sum = require('sugar/array/sum');

var _sum2 = _interopRequireDefault(_sum);

var _zip = require('sugar/array/zip');

var _zip2 = _interopRequireDefault(_zip);

var _last = require('sugar/array/last');

var _last2 = _interopRequireDefault(_last);

var _unique = require('sugar/array/unique');

var _unique2 = _interopRequireDefault(_unique);

var _moment = require('moment');

var _moment2 = _interopRequireDefault(_moment);

var _quiche = require('quiche');

var _quiche2 = _interopRequireDefault(_quiche);

var _expressSession = require('express-session');

var _expressSession2 = _interopRequireDefault(_expressSession);

var _multer = require('multer');

var _multer2 = _interopRequireDefault(_multer);

var _xlsx = require('xlsx');

var _xlsx2 = _interopRequireDefault(_xlsx);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

//import "babel-polyfill";

//import path from 'path';
var FileStore = require('session-file-store')(_expressSession2.default);

var mult_storage = _multer2.default.diskStorage({
  destination: function destination(req, file, cb) {
    cb(null, './uploads/');
  },
  filename: function filename(req, file, cb) {
    cb(null, file.originalname);
  }
});

_mongoose2.default.Promise = global.Promise;
var connectionString = 'mongodb://user1:' + process.env.MONGOPASS + '@ds029496.mlab.com:29496/plottersdb_test';
_mongoose2.default.connect(connectionString, function (err, database) {
  if (err) return console.log(err);
  app.listen(app.get('port'), function () {
    console.log('Express started on port ' + app.get('port'));
  });
});

var plotterSession = _mongoose2.default.model('plotterSession', {
  id: Number,
  plotter: Number,
  start_time: Date,
  stop_time: Date,
  passes: Number,
  meters: Number
});

var app = (0, _express2.default)();
var handlebars = require('express-handlebars').create({ defaultLayout: 'main' });

app.use(_express2.default.static('public'));
app.use(_bodyParser2.default.urlencoded({ extended: true }));
app.use((0, _expressSession2.default)({
  secret: 'my express secret',
  saveUninitialized: true,
  resave: true,
  store: new FileStore()
}));
app.engine('handlebars', handlebars.engine);
app.set('view engine', 'handlebars');
app.set('port', process.env.PORT || 3000);

app.all('/', function (req, res) {
  console.log('got request');
  var start = void 0,
      stop = void 0,
      period = void 0;
  var meters1 = void 0,
      meters2 = void 0,
      meters3 = void 0,
      meters4 = void 0,
      meters5 = void 0;
  var periodForChart = void 0;
  var elementForCheck = void 0;
  //---------- if there is selected date/time---------------
  if (req.body.usestartTime !== undefined) {
    start = req.body.usestartTime;
    req.session.start = start; //save new cookie
    stop = req.body.usestopTime;
    req.session.stop = stop; //save new cookie
    period = req.body.period;
    req.session.period = period; //save new cookie
  } else {
    //---------- if date/time does not selected ---------------
    //---------- if there are cookies -------------------------
    if (req.session.start !== undefined) {
      start = req.session.start; //take cookies for variabled
      stop = req.session.stop;
      period = req.session.period;
    } else {
      //---------if there are not cookies----------------------
      //---------take today for variables----------------------
      //let d = moment().format();
      start = (0, _moment2.default)().startOf('isoWeek').format("YYYY-MM-DDTHH:mm"); //start of week
      stop = (0, _moment2.default)().endOf('isoWeek').format("YYYY-MM-DDTHH:mm"); //finish of week
      period = 'day';
      req.session.start = start; //set new cookies
      req.session.stop = stop;
      req.session.period = period;
    }
  }

  console.log('start, stop time from request\n', start);
  console.log(stop);

  start = (0, _moment2.default)(start).utc();
  stop = (0, _moment2.default)(stop).utc();

  console.log('UTC start, stop time from request\n', start);
  console.log(stop);

  plotterSession.find({ "start_time": { "$gte": start, "$lte": stop } }, function (err, docs) {
    if (err) {
      console.log(err);
    }
    var maxDatasOnChart = 5;
    var sum1 = parseFloat(calcSumMeters(1, docs));
    var sum2 = parseFloat(calcSumMeters(2, docs));
    var sum3 = parseFloat(calcSumMeters(3, docs));
    var sum4 = parseFloat(calcSumMeters(4, docs));
    var sum5 = parseFloat(calcSumMeters(5, docs));
    var sumAll = (sum2 + sum1 + sum3 + sum4 + sum5).toFixed(2);

    if (period === 'day') {
      var days = getDays(start, stop);
      //console.log('days ', days);
      meters1 = getMetersDays(1, days, docs);
      meters2 = getMetersDays(2, days, docs);
      meters3 = getMetersDays(3, days, docs);
      meters4 = getMetersDays(4, days, docs);
      meters5 = getMetersDays(5, days, docs);
      var daysLength = days.length;
      var chartDatesStep = Math.ceil(daysLength / maxDatasOnChart);

      //console.log('chartDatesStep', chartDatesStep);

      var i = 0;
      var daysForChart = days;
      while (i < daysLength) {
        if (i % chartDatesStep !== 0) {
          daysForChart[i] = '';
        }
        i++;
      }
      //console.log('daysForChart', daysForChart);
      periodForChart = daysForChart;
      elementForCheck = "day";
    }

    if (period === 'week') {
      var weeks = getWeeks(start, stop);
      //console.log('weeks ', weeks);
      meters1 = getMetersWeeks(1, weeks, docs);
      meters2 = getMetersWeeks(2, weeks, docs);
      meters3 = getMetersWeeks(3, weeks, docs);
      meters4 = getMetersWeeks(4, weeks, docs);
      meters5 = getMetersWeeks(5, weeks, docs);
      var weeksLength = weeks.length;
      var _chartDatesStep = Math.round(weeksLength / maxDatasOnChart);
      var _i = 0;
      var weeksForChart = weeks.map(function (week) {
        return (0, _moment2.default)(week).format('DD MMM YYYY');
      });
      while (_i < weeksLength) {
        if (_i % _chartDatesStep !== 0) {
          weeksForChart[_i] = '';
        }
        _i++;
      }
      periodForChart = weeksForChart;
      elementForCheck = "week";
    }

    if (period === 'month') {
      var months = getMonths(start, stop);
      //console.log('months', months);
      meters1 = getMetersMonths(1, months, docs);
      meters2 = getMetersMonths(2, months, docs);
      meters3 = getMetersMonths(3, months, docs);
      meters4 = getMetersMonths(4, months, docs);
      meters5 = getMetersMonths(5, months, docs);

      var monthsLength = months.length;
      var _chartDatesStep2 = Math.round(monthsLength / maxDatasOnChart);
      var _i2 = 0;
      var monthsForChart = months.map(function (month) {
        return (0, _moment2.default)(month).format('MMM YYYY');
      });
      while (_i2 < monthsLength) {
        if (_i2 % _chartDatesStep2 !== 0) {
          monthsForChart[_i2] = '';
        }
        _i2++;
      }
      periodForChart = monthsForChart;
      elementForCheck = "month";
    }

    var bar = new _quiche2.default('bar');
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
    res.render('home', { 'sum1': sum1, 'sum2': sum2, 'sum3': sum3, 'sum4': sum4, 'sum5': sum5, 'sumAll': sumAll, 'chartUrl': imageUrl, 'startCookie': req.session.start, 'stopCookie': req.session.stop, 'element': elementForCheck });
  });
});

app.get('/input', function (req, res) {
  res.render('input');
});

app.post('/quotes', function (req, res) {
  // console.log(req.body);
  //console.log(req.body);
  var session = new plotterSession({
    id: req.body.id,
    plotter: req.body.plotter,
    start_time: req.body.startTime,
    stop_time: req.body.stopTime,
    passes: req.body.passes,
    meters: req.body.meters
  });
  console.log('new data received\n', session);
  session.save(function (err) {
    if (err) {
      console.log(err);
    } else {
      console.log('plotterSession saved');
    }
  });

  res.redirect('/');
});

app.get('/compare', function (req, res) {
  var error = '';
  var reports = getReport();
  console.log('reports\n', reports);
  var dates = reports.map(function (obj) {
    var date = (0, _moment2.default)(obj['Дата'], "DD-MM-YY").format();
    if (!(0, _moment2.default)(date).isValid()) {
      error = 'ОШИБКА: неверная дата' + date;
      date = (0, _moment2.default)().format();
    }
    return date;
  });

  var datesMeters = reports.map(function (obj) {
    var date = (0, _moment2.default)(obj['Дата'], "DD-MM-YY").format();
    var long = obj['Длина'];
    if (long === undefined) {
      error = 'ОШИБКА: неуказана длина\n';
      long = 0;
    }
    if (!(0, _moment2.default)(date).isValid()) {
      error = 'ОШИБКА: неверная дата\n' + date;
      date = (0, _moment2.default)().format();
    }
    return { 'Дата': (0, _moment2.default)(date).format(), 'Длина': long };
  });
  //console.log('datesMeters\n', datesMeters);
  var datesMetersUnique = datesMeters.reduce(function (arrResult, obj) {
    arrResult[obj['Дата']] = (arrResult[obj['Дата']] || 0) + parseFloat(obj['Длина']);
    return arrResult;
  }, []);
  var datesMetersUnique_new = [];
  for (var key in datesMetersUnique) {
    datesMetersUnique_new.push({ 'Дата': key, 'МетрыОтчёт': datesMetersUnique[key] });
  }

  console.log('datesMeters\n', datesMeters);
  console.log('datesMetersUnique\n', datesMetersUnique);
  console.log('datesMetersUnique_new\n', datesMetersUnique_new);
  //console.log('dates\n', dates);
  dates.sort(function (a, b) {
    if ((0, _moment2.default)(a).isBefore((0, _moment2.default)(b))) return -1;
    if ((0, _moment2.default)(a).isAfter((0, _moment2.default)(b))) return 1;
  });
  //console.log('dates sorted\n', dates);
  var mini = (0, _moment2.default)(dates[0]).startOf('day').format();
  var maxi = (0, _moment2.default)(dates[dates.length - 1]).endOf('day').format();
  console.log('start', mini);
  console.log('stop', maxi);
  var toShow = [];
  plotterSession.find({ "start_time": { "$gte": mini, "$lte": maxi } }, function (err, docs) {
    toShow = datesMetersUnique_new.map(function (jsonItem) {
      var metersSensor = parseFloat(getMetersDays(1, getDays((0, _moment2.default)(jsonItem.Дата).format(), (0, _moment2.default)(jsonItem.Дата).format()), docs)) + parseFloat(getMetersDays(2, getDays((0, _moment2.default)(jsonItem.Дата).format(), (0, _moment2.default)(jsonItem.Дата).format()), docs)) + parseFloat(getMetersDays(3, getDays((0, _moment2.default)(jsonItem.Дата).format(), (0, _moment2.default)(jsonItem.Дата).format()), docs)) + parseFloat(getMetersDays(4, getDays((0, _moment2.default)(jsonItem.Дата).format(), (0, _moment2.default)(jsonItem.Дата).format()), docs)) + parseFloat(getMetersDays(5, getDays((0, _moment2.default)(jsonItem.Дата).format(), (0, _moment2.default)(jsonItem.Дата).format()), docs));
      metersSensor = metersSensor.toFixed(2);
      console.log('metersSensor\n', metersSensor);
      return { 'Дата': (0, _moment2.default)(jsonItem.Дата).format("DD.MM.YY"), 'МетрыОтчёт': parseFloat(jsonItem.МетрыОтчёт).toFixed(2), 'МетрыДатчик': metersSensor, 'Разница': (parseFloat(metersSensor) - parseFloat(jsonItem.МетрыОтчёт)).toFixed(2) };
    });
    //console.log('new reports', reports);
    console.log('toShow\n', toShow);
    res.render('compare', { 'report': toShow, 'error': error });
  });
});

app.post('/upload', (0, _multer2.default)({ storage: mult_storage, dest: './uploads/' }).single('upl'), function (req, res) {
  console.log(req.file);
  res.redirect('/compare');
});

var getMetersDays = function getMetersDays(pl, days, docs) {
  var sums = [];
  //console.log('docs\n', docs);
  var docs_definite_plotter = docs.filter(function (session) {
    return session.plotter === pl;
  });
  days.forEach(function (day) {
    var isodatestart = new Date(day + "T00:00:00.000Z");
    var isodatestop = new Date(day + "T23:59:59.000Z");
    var daySession = docs_definite_plotter.filter(function (session) {
      return session.start_time >= isodatestart && session.stop_time <= isodatestop;
    });
    sums.push(parseFloat(calcSumMeters(pl, daySession)));
  });
  return sums;
};

var getMetersMonths = function getMetersMonths(pl, months, docs) {
  var sums = [];
  var docs_definite_plotter = docs.filter(function (session) {
    return session.plotter === pl;
  });
  months.forEach(function (month) {
    var isodatestart = (0, _moment2.default)(month).startOf('month').format();
    var isodatestop = (0, _moment2.default)(month).endOf('month').format();
    var monthSession = docs_definite_plotter.filter(function (session) {
      return (0, _moment2.default)(session.start_time).isAfter(isodatestart) && (0, _moment2.default)(session.stop_time).isBefore(isodatestop);
    });
    sums.push(parseFloat(calcSumMeters(pl, monthSession)));
  });
  console.log('meters in months', sums);
  return sums;
};

var getMetersWeeks = function getMetersWeeks(pl, weeks, docs) {
  var sums = [];
  var docs_definite_plotter = docs.filter(function (session) {
    return session.plotter === pl;
  });
  weeks.forEach(function (week) {
    var isodatestart = (0, _moment2.default)(week).startOf('isoWeek').format();
    var isodatestop = (0, _moment2.default)(week).endOf('isoWeek').format();
    var weekSession = docs_definite_plotter.filter(function (session) {
      return (0, _moment2.default)(session.start_time).isAfter(isodatestart) && (0, _moment2.default)(session.stop_time).isBefore(isodatestop);
    });
    sums.push(parseFloat(calcSumMeters(pl, weekSession)));
  });
  console.log('meters in weeks', sums);
  return sums;
};

var calcSumMeters = function calcSumMeters(plotter, someDocs) {
  var result = (0, _sum2.default)(someDocs.filter(function (obj) {
    return obj.plotter === plotter;
  }).map(function (psession) {
    return psession.meters;
  }));
  result = parseFloat(result);
  result = result.toFixed(2);
  return result;
};

var getDays = function getDays(s, f) {
  var startDateTime = (0, _moment2.default)(s);
  var stopDateTime = (0, _moment2.default)(f);
  var startDate = (0, _moment2.default)(startDateTime).format("YYYY-MM-DD");
  var stopDate = (0, _moment2.default)(stopDateTime).format("YYYY-MM-DD");
  var days = [];
  var i = 0;
  var newdate = void 0;
  do {
    newdate = (0, _moment2.default)(startDate).add(i, 'days').format("YYYY-MM-DD");
    days.push(newdate);
    i++;
  } while (days[days.length - 1] !== stopDate);

  return days;
};

var getMonths = function getMonths(s, f) {
  var startDate = (0, _moment2.default)(s).startOf('month').format();
  var stopDate = (0, _moment2.default)(f).endOf('month').format();
  var months = [];
  months.push(startDate);
  var i = 0;
  var newdate = void 0;

  while ((0, _moment2.default)(months[months.length - 1]).endOf('month').isBefore(stopDate)) {
    i++;
    newdate = (0, _moment2.default)(startDate).add(i, 'M').format();
    months.push(newdate);
  }
  return months;
};

var getWeeks = function getWeeks(s, f) {
  var startDate = (0, _moment2.default)(s).startOf('isoWeek').format();
  var stopDate = (0, _moment2.default)(f).endOf('isoWeek').format();
  var weeks = [];
  weeks.push(startDate);
  var i = 0;
  var newdate = void 0;
  while ((0, _moment2.default)(weeks[weeks.length - 1]).endOf('isoWeek').isBefore(stopDate)) {
    i++;
    newdate = (0, _moment2.default)(startDate).add(i, 'w').format();
    weeks.push(newdate);
  }
  return weeks;
};

var getReportFromXls = function getReportFromXls(plotter) {
  var xlsPath = './uploads/plotter' + plotter + '.xls';
  console.log('xlsPath\n', xlsPath);
  var workbook = _xlsx2.default.readFile(xlsPath);
  var sheet_name_list = workbook.SheetNames;
  var report = _xlsx2.default.utils.sheet_to_json(workbook.Sheets[sheet_name_list[0]]);
  return report;
};

var getReport = function getReport() {
  var report1 = getReportFromXls(1);
  var report2 = getReportFromXls(2);
  var report3 = getReportFromXls(3);
  var report4 = getReportFromXls(4);
  var report5 = getReportFromXls(5);
  return report1.concat(report2, report3, report4, report5);
};

function getMetersFromTo(start, stop) {
  //console.log ('In getMetersFromTo');
  var meters = void 0;
  plotterSession.find({ "start_time": { "$gte": start, "$lte": stop } }, function (err, docs) {
    var metersArr = docs.map(function (doc) {
      return doc.meters;
    });
    meters = parseFloat((0, _sum2.default)(metersArr));
    meters = meters.toFixed(2);
    console.log('meters', meters);
  });
  return meters;
};

var errorHandler = function errorHandler(message) {};
