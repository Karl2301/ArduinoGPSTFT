void start_file() {
  Serial.println(F("creation du dossier"));
  SD.mkdir("/trajet/" + typedefichier + "file/" + nameFile + "/");
  Serial.println(F("dossier créé. Creation du chemin."));
  String chemin = "/trajet/" + typedefichier + "file/" + nameFile + "/" + nameFile + "." + typedefichier;
  Serial.println(F("chemin créé. debut du fichier."));
  myFile = SD.open(chemin, FILE_WRITE);
  if (typedefichier == "kml") {
    myFile.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    myFile.println("<kml xmlns=\"http://www.opengis.net/kml/2.2\">");
    myFile.println("<Document>");
    myFile.println("<name>GPS Route</name>");
    myFile.println("<Placemark>");
    myFile.println("<name>" + nameFile + " GPS Route</name>");
    myFile.println("<LineString>");
    myFile.println("<coordinates>");
  } else if (typedefichier == "gpx") {
    myFile.println("<?xml version=\"1.0\"?>");
    myFile.println("<gpx version=\"1.1\" creator=\"Karl\">");
  } else if (typedefichier == "xml") {
    myFile.println("<?xml version=\"1.0\"?>");
    myFile.println("<gpx version=\"1.1\" creator=\"Karl\">");
  }
  myFile.close();
  Serial.println(F("debut du fichier créé. supression de tjtcour.txt"));
  SD.remove("p/t/tjtcour.txt");
  Serial.println(F("supression de tjtcour.txt fait. creation du fichier d'existance"));
  File mySave = SD.open("/trajet/nomfich/" + nameFile + "." + typedefichier, FILE_WRITE);
  mySave.print(nameFile + "." + typedefichier);
  mySave.close();
  Serial.println(F("fichier créé. creation de tjtcour.txt"));

  File myFileFolder = SD.open("p/t/tjtcour.txt", FILE_WRITE);
  myFileFolder.print(nameFile + "." + typedefichier);
  myFileFolder.close();
  Serial.println(F("fichier créé."));
  //{"SatMax":"12","AltMax":"80","SpeedMax":"130"}
  Serial.println(F("démarrage du trajet"));
  typedefichier = typedefichier;
}

void write_sd_card() {
  compteTemps = compteTemps + 1;
  myFile = SD.open("/trajet/" + typedefichier + "file/" + nameFile + "/" + nameFile + "." + typedefichier, FILE_WRITE);
  if (typedefichier == "kml") {
    myFile.print(GPSLONG, 6);
    myFile.print(",");
    myFile.print(GPSLAT, 6);
    myFile.print(",");
    myFile.println("5000");
  } else if (typedefichier == "gpx") {
    myFile.print("<wpt lat=\"");
    myFile.print(GPSLAT, 6);
    myFile.print("\" lon=\"");
    myFile.print(GPSLONG, 6);
    myFile.println("\">");
    myFile.print("<ele>5000</ele>");
    myFile.print("<time>");
    myFile.print(nameFile);
    myFile.print("</time>");
    myFile.println("</wpt>");
  } else if (typedefichier == "xml") {
    myFile.print("<wpt lat=\"");
    myFile.print(GPSLAT, 6);
    myFile.print("\" lon=\"");
    myFile.print(GPSLONG, 6);
    myFile.println("\">");
    myFile.print("<ele>5000</ele>");
    myFile.print("<time>");
    myFile.print(nameFile);
    myFile.println("</time>");
    myFile.println("</wpt>");
  } else if (typedefichier == "txt") {
    myFile.print(nameFile);
    myFile.print(",");
    myFile.print(compteTemps);
    myFile.print(",");
    myFile.print(String(GPSLAT, 8));
    myFile.print(",");
    myFile.print(String(GPSLONG, 8));
    myFile.println(",5000");

  }
  myFile.close();
  SD.remove("/trajet/" + typedefichier + "file/" + nameFile + "/data.txt");
  String Jsonformat = "{\"SatMax\":\"" + String(MaxSat) + "\",\"AltMax\":\"" + String(MaxAlt) + "\",\"SpeedMax\":\"" + String(MaxSpeed) + "\", \"temps\":\"" + String(compteTemps) + "\", \"distParc\":\"" + String(distanceparcouru, 3) + "\"}";
  File myData = SD.open("/trajet/" + typedefichier + "file/" + nameFile + "/data.txt", FILE_WRITE);
  myData.print(Jsonformat);
  Serial.println(F("Ecriture de Data"));
  myData.close();
  Serial.println(F("Fin ecriture de Data"));
  
  if (activelog == 1) {
    File myLog = SD.open("/trajet/" + typedefichier + "file/" + nameFile + "/log.txt", FILE_WRITE);
    String Log = String(Time) + " - " + String(Date) + ": Sat: " + String(GPSSAT) + " , Sat Max: " + String(MaxSat) + " , Vitesse: " + String(GPSSPEED) + "k/h , Vitesse Max: " + String(MaxSpeed) + "k/h , Altitude: " + String(GPSALT) + "m , Altitude Max: " + String(MaxAlt) + "m , Angle: " + String(GPSDEG) + "deg , dist. Parcourue: " + String(distanceparcouru, 3) + "km , Humidité: " + String(h) + "% , Temperature: " + String(t) + "°C , Latitude: " + String(GPSLAT, 8) + " , Longitude: " + String(GPSLONG, 8);
    myLog.println(Log);
    myLog.close();
  }
  
  Serial.println(F("Ecriture sur la carte sd"));
  Serial.println(F("Fin écriture sur la carte sd"));
}
void finish_file() {
  if (typedefichier == "kml") {
    myFile = SD.open("/trajet/" + typedefichier + "file/" + nameFile + "/" + nameFile + "." + typedefichier, FILE_WRITE);
    myFile.println("</coordinates>");
    myFile.println("</LineString>");
    myFile.println("<Style>");
    myFile.println("<LineStyle>");
    myFile.println("<color>##F1C40F</color>");
    myFile.println("<width>5</width>");
    myFile.println("</LineStyle>");
    myFile.println("</Style>");
    myFile.println("</Placemark>");
    myFile.println("</Document>");
    myFile.print("</kml>");
  } else if (typedefichier == "gpx") {
    myFile.print("</gpx>");
  } else if (typedefichier == "xml") {
    myFile.print("</gpx>");
  }
  myFile.close();
  SD.remove("p/t/tjtcour.txt");
  SD.remove("/trajet/" + typedefichier + "file/" + nameFile + "/time.txt");
  SD.remove("/trajet/" + typedefichier + "file/" + nameFile + "/temps.txt");
  SD.remove("/trajet/" + typedefichier + "file/" + nameFile + "/smax.txt");
  SD.remove("/trajet/" + typedefichier + "file/" + nameFile + "/vmax.txt");
  SD.remove("/trajet/" + typedefichier + "file/" + nameFile + "/amax.txt");
  File mySave = SD.open("p/t/tjtcour.txt", FILE_WRITE);
  mySave.print("");
  mySave.close();

  secondeTrajet = compteTemps % 60;
  minuteTrajet = (compteTemps / 60) % 60;
  heureTrajet = (compteTemps / (60 * 60));

  Serial.print(F("Le trajet a duré ")); Serial.print(heureTrajet); Serial.print(F(" heure(s), ")); Serial.print(minuteTrajet); Serial.print(F(" minute(s), ")); Serial.print(secondeTrajet); Serial.println(F("seconde(s)"));

  File myDebrif = SD.open("/trajet/" + typedefichier + "file/" + nameFile + "/recap.txt", FILE_WRITE);
  myDebrif.print("Vitesse Maximum : ");
  myDebrif.println(MaxSpeed);
  myDebrif.print("Altitude Maximum : ");
  myDebrif.println(MaxAlt);
  myDebrif.print("Satellite Maximum : ");
  myDebrif.println(MaxSat);
  myDebrif.print("Angle : ");
  myDebrif.println(GPSDEG);
  myDebrif.print("Distance parcourue : ");
  myDebrif.println(distanceparcouru, 3);
  myDebrif.print("Temp de trajet : ");
  myDebrif.print("Le trajet a duré "); myDebrif.print(heureTrajet); myDebrif.print(" heure(s), "); myDebrif.print(minuteTrajet); myDebrif.print(" minute(s), "); myDebrif.print(secondeTrajet); myDebrif.println("seconde(s)");
  myDebrif.close();

  Serial.print(F("creation du dossier datasave: "));
  SD.mkdir("/trajet/" + typedefichier + "file/" + nameFile + "/datasave/");

  String Jsonformat = "{\"SatMax\":\"" + String(MaxSat) + "\",\"AltMax\":\"" + String(MaxAlt) + "\",\"SpeedMax\":\"" + String(MaxSpeed) + "\",\"temps\":\"" + String(compteTemps) + "\",\"distParc\":\"" + String(distanceparcouru, 3) +  "\",\"heure\":\"" + String(heureTrajet) + "\",\"minute\":\"" + String(minuteTrajet) + "\",\"seconde\":\"" + String(secondeTrajet) + "\"}";
  File myData = SD.open("/trajet/" + typedefichier + "file/" + nameFile + "/datasave/datasave.txt", FILE_WRITE);
  if (myData) {
    myData.print(Jsonformat);
    Serial.println(F("Ecriture de Datasave"));
  }
  myData.close();

  distanceparcouru = 0.0;
  Serial.println(F("fin du trajet"));
}
