void heureUTC() {
  if (nbrheure == waitheure) {
    ;
  }else {
    Minute = GPSMINUTES;
    Second = GPSSECONDES;
    Hour   = GPSHOURS;
    Day    = GPSDAY;
    Month  = GPSMONTH;
    Year   = GPSYEAR;
    setTime(Hour, Minute, Second, Day, Month, Year);
    adjustTime(nbrheure * 3600);
    Time[12] = second() / 10 + '0';
    Time[13] = second() % 10 + '0';
    Time[9]  = minute() / 10 + '0';
    Time[10] = minute() % 10 + '0';
    Time[6]  = hour()   / 10 + '0';
    Time[7]  = hour()   % 10 + '0';
    Date[14] = (year()  / 10) % 10 + '0';
    Date[15] =  year()  % 10 + '0';
    Date[9]  =  month() / 10 + '0';
    Date[10] =  month() % 10 + '0';
    Date[6]  =  day()   / 10 + '0';
    Date[7]  =  day()   % 10 + '0';
    waitheure = nbrheure;
  }
}
