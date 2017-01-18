import moment from 'moment';

export const isValid = (json) => {
  if ((json.plotter < 1) || (json.plotter > 5)) {
    return `db validation: invalid plotter number from plotter`;
  }
  if (moment(json.start_time).isBefore('2014-01-01', 'year')) {
    return `db validation: invalid start_time from ${json.plotter}`;
  }
  if (moment(jsonSession.stop_time).isBefore('2014-01-01', 'year') || moment(jsonSession.stop_time).isBefore(jsonSession.start_time)) {
    return `db validation: invalid stop_time from ${json.plotter}`;
  }
  if ((json.meters < 0) || (json.meters > 100)) {
    return `db validation: invalid meters from ${json.plotter}`;
  }
  return true;
};
