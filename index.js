import express from 'express';
import bodyParser from 'body-parser';
import mongo from 'mongodb';
import mongoose from 'mongoose';
import printSession from './models/plotters.js';

let opts = {
  server: {
    socketOptions: { keepAlive: 1 },
  },
};

switch (app.get('env')) {
  case 'development':
    mongoose.connect(credentials.mongo.development.connectionString, opts);
    break;
  case 'production':
    mongoose.connect(credentials.mongo.production.connectionString, opts);
    break;
  default:
    throw new Error('Unknown enviroment: ' + app.get('env'));
}

let app = express();
let handlebars = require('express-handlebars').create({ defaultLayout: 'main' });

app.use(bodyParser.urlencoded({ extended: true }));
app.engine('handlebars', handlebars.engine);
app.set('view engine', 'handlebars');
app.set('port', process.env.PORT || 3000);

app.get('/', (req, res) => {
  res.render('home');
});

app.post('/quotes', (req, res) => {
  console.log(req.body);
});

app.listen(app.get('port'), () => {
  console.log('Express started on port ' + app.get('port'));
});
