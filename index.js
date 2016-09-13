import express from 'express';

let app = express();
let handlebars = require('express-handlebars').create({ defaultLayout: 'main' });

app.engine('handlebars', handlebars.engine);
app.set('view engine', 'handlebars');
app.set('port', process.env.PORT || 3000);

app.get('/', (req, res) => {
  res.type('text/plain');
  res.send('Plotters statistics');
});

app.listen(app.get('port'), () => {
  console.log('Express started on port ' + app.get('port'));
});
