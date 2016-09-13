import express from 'express';
import bodyParser from 'body-parser';

let app = express();
let handlebars = require('express-handlebars').create({ defaultLayout: 'main' });
let body;

app.use(bodyParser.urlencoded({ extended: true }));
app.engine('handlebars', handlebars.engine);
app.set('view engine', 'handlebars');
app.set('port', process.env.PORT || 3000);

app.get('/', (req, res) => {
  res.render('home');
});

app.listen(app.get('port'), () => {
  console.log('Express started on port ' + app.get('port'));
});
