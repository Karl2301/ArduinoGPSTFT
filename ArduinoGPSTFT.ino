// URTouch_ButtonTest
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program is a quick demo of how create and use buttons.
//
// This program requires the UTFT library.
//
// It is assumed that the display module is connected to an
// appropriate shield or that you know how to change the pin
// numbers in the setup.
//

#include <UTFT.h>
#include <URTouch.h>
#include <UTFT_Buttons.h>
#include <TinyGPS++.h>
#include <TimeLib.h>
#include <Wire.h>
#include "variable.h"
#include <stdint.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SPI.h>                        // bibliothèque bus SPI
#include <SD.h>
#include <DHT.h>

#define MaxMenu 11
#define GPSECHO  false
#define delayled 100
#define brocheDeBranchementDHT 2   // La ligne de communication du DHT22 sera donc branchée sur la pin 2 de l'Arduino
#define typeDeDHT DHT11

TinyGPSPlus gps;
File myFile;
DHT dht(brocheDeBranchementDHT, typeDeDHT);
SoftwareSerial nodemcu(17, 16);
UTFT    myGLCD(ILI9341_16, 38, 39, 40, 41);
URTouch  myTouch( 6, 5, 4, 3, 2);
UTFT_Buttons  myButtons(&myGLCD, &myTouch);

extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t Dingbats1_XL[];
extern uint8_t SevenSegNumFont[];

int but1;
int but2;
int but3;
int but4;
int but5;
int but6;
int but7;
int but8;
int but9;
int but10;
int but11;
int but12;
int butOK;
int retour;

int datasave = 0;

int reglage = 1;

byte code = 0;
String codestr = "";
byte numcode = 0;

byte nombrenumcode = 6;

byte menuTFT = 0;
int sat, satmax, speed, speedmax, alt, altmax, angle, distparc;
float lat, lng;

void (*reset)() = 0;
uint32_t SDrestart = millis();
void setup()
{
  // Initial setup
  Serial.begin(115200);
  Serial3.begin(9600);
  myGLCD.InitLCD();
  myGLCD.clrScr();


  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);

  myGLCD.setBackColor(0, 0, 0);

  myButtons.setSymbolFont(Dingbats1_XL);


  myButtons.setSymbolFont(Dingbats1_XL);
  myButtons.setTextFont(SmallFont);
  myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_GRAY);

  dht.begin();
  nodemcu.begin(115200);



  if (!SD.begin(53)) {
    Serial.println(F("SD absente ou HS. stop."));

  }
  Serial.println(F("SD OK"));

  if (!SD.exists("/p/") || !SD.exists("/trajet/")) {
    Serial.println("Reconstruction des fichiers de la carte SD");
    SDverif();
    Serial.println("Reconstruction terminer");
  }

  String codegps;
  File codegpsfile = SD.open("p/code.txt");
  if (codegpsfile) {
    while (codegpsfile.available()) {
      codegps = codegpsfile.readStringUntil('\n');
      codeverif = codegps;
      Serial.print(F("Code: ")); Serial.println(codeverif);
    }
    codegpsfile.close();
  }

  File Paramfile = SD.open("/p/param.txt");
  if (Paramfile) {
    while (Paramfile.available()) {
      String ParamRecup = "";
      ParamRecup = Paramfile.readStringUntil('\n');
      Serial.print(F("ParamRecup: ")); Serial.println(ParamRecup);
      DynamicJsonBuffer jsonBuffer;
      JsonObject& obj = jsonBuffer.parseObject(ParamRecup);
      String nbrheureSTR = obj["UTC"].as<String>();
      String frequenceSDSTR = obj["SD"].as<String>();
      String activelogSTR = obj["activelog"].as<String>();
      String activecodeSTR = obj["activecode"].as<String>();
      String reptrajetSTR = obj["reptrajet"].as<String>();

      nbrheure = nbrheureSTR.toInt();
      frequenceSD = frequenceSDSTR.toInt();
      activelog = activelogSTR.toInt();
      code = activecodeSTR.toInt();
      reptrajet =  reptrajetSTR.toInt();

      Serial.print(F("UTC: ")); Serial.println(nbrheure);
      Serial.print(F("SD: ")); Serial.println(frequenceSD);
      Serial.print(F("Activelog: ")); Serial.println(activelog);
      Serial.print(F("Activecode: ")); Serial.println(activecode);
      Serial.print(F("Reptrajet: ")); Serial.println(reptrajet);
      jsonBuffer.clear();
    }
    Paramfile.close();
  } else {
    Serial.println(F("Pas de données de Paramétre"));
  }

  File trajetencourfile = SD.open("p/t/tjtcour.txt");
  if (trajetencourfile) {
    while (trajetencourfile.available()) {
      trajetencour = trajetencourfile.readStringUntil('\n');
      Serial.print(F("trajet en cour: ")); Serial.println(trajetencour);
    }
    trajetencourfile.close();
  }
  //String heureutc = SD.open("p/heure.txt", FILE_READ).readStringUntil('\r').close();
  //nbrheure = heureutc.toInt();

  File DistDestifile = SD.open("p/d/distance.txt");
  if (DistDestifile) {
    while (DistDestifile.available()) {
      String dataDistanceRecup = "";
      dataDistanceRecup = DistDestifile.readStringUntil('\n');
      Serial.print(F("DataDistanceRecup: ")); Serial.println(dataDistanceRecup);
      DynamicJsonBuffer jsonBuffer;
      JsonObject& obj = jsonBuffer.parseObject(dataDistanceRecup);
      latDesti = obj["Lat"].as<String>();
      lonDesti = obj["Lon"].as<String>();
      Serial.print(F("Lat: ")); Serial.println(latDesti);
      Serial.print(F("Lon: ")); Serial.println(lonDesti);
      jsonBuffer.clear();
    }
    DistDestifile.close();
  } else {
    Serial.println(F("Pas de données de Distance"));
  }


  //lonDesti = SD.open("p/d/lon.txt", FILE_READ).readStringUntil('\r');

  //latDesti = SD.open("p/d/lat.txt", FILE_READ).readStringUntil('\r');

  //String trajetencour = SD.open("p/t/tjtcour.txt", FILE_READ).readStringUntil('\r');
  if (reptrajet == 1) {
    if (trajetencour != 0) {
      Serial.println(F("trajet peu etre repris"));
      Serial.print(F("Nom du trajet repris du debut: ")); Serial.println(trajetencour);
      trajet = 2;
      if (trajetencour.indexOf("kml") != -1) {
        typedefichier = "kml";
        trajetencour.replace(".kml", "");
        Serial.println(F(".kml remplacer"));
      } else if (trajetencour.indexOf("gpx") != -1) {
        typedefichier = "gpx";
        trajetencour.replace(".gpx", "");
        Serial.println(F(".gpx remplacer"));
      } else if (trajetencour.indexOf("txt") != -1) {
        typedefichier = "txt";
        trajetencour.replace(".txt", "");
        Serial.println(F(".txt remplacer"));
      } else if (trajetencour.indexOf("xml") != -1) {
        typedefichier = "xml";
        trajetencour.replace(".xml", "");
        Serial.println(F(".xml remplacer"));
      }
      nameFile = trajetencour;
      Serial.print(F("Nom du trajet repris de fin: ")); Serial.println(nameFile);
      String datarecup = "";
      String cheminData = "/trajet/" + typedefichier + "file/" + nameFile + "/data.txt";
      datarecup = SD.open(cheminData).readStringUntil('\r');
      Serial.print(F("Data: ")); Serial.println(datarecup);

      DynamicJsonBuffer jsonBuffer;
      JsonObject& obj = jsonBuffer.parseObject(datarecup);
      String Maxsatstr;
      String MaxAltstr;
      String MaxSpeedstr;
      String compteTempsstr;
      String distanceparcourustr;
      Maxsatstr = obj["SatMax"].as<String>();
      MaxAltstr = obj["AltMax"].as<String>();
      MaxSpeedstr = obj["SpeedMax"].as<String>();
      compteTempsstr = obj["temps"].as<String>();
      distanceparcourustr = obj["distParc"].as<String>();
      MaxSat = Maxsatstr.toInt();
      MaxAlt = MaxAltstr.toInt();
      MaxSpeed =  MaxSpeedstr.toInt();
      compteTemps = compteTempsstr.toInt();
      distanceparcouru = distanceparcourustr.toFloat();
      secondeTrajet = compteTemps % 60;
      minuteTrajet = (compteTemps / 60) % 60;
      heureTrajet = (compteTemps / (60 * 60));
      Serial.print(F("MaxSat: ")); Serial.println(MaxSat);
      Serial.print(F("MaxAlt: ")); Serial.println(MaxAlt);
      Serial.print(F("MaxSpeed: ")); Serial.println(MaxSpeed);
      Serial.print(F("Compte temps: ")); Serial.println(compteTemps);
      Serial.print(F("Distance parourue: ")); Serial.println(distanceparcouru);
      Serial.println(F("Trajet redémarré"));
    } else {
      Serial.println(F("Pas de trajet repris"));
    }
  }
  //on compte le nombre de fichier dans la carte sd
  Serial.println(codegps);
  nbFichiers = compteNbFichiers();
  Serial.print(nbFichiers);  Serial.println(F(" fichiers à la racine"));
  Serial.print(nbFichiers); Serial.print(F("\t"));
  Serial.println(affiFichier(nbFichiers));
  Serial.println(F("\n+ = fichier suivant, - = fichier précédent"));
  noFichier = nbFichiers; // n° de fichier dans la liste, on part du dernier
  if (++noFichier > nbFichiers) {
    noFichier--;
  }
  nomFichier = affiFichier(noFichier);  // affichage nom fichier indexé par noFichier > 0
  Serial.print(noFichier); Serial.print(F("\t"));
  Serial.println(nomFichier);

  if (code == 1) {
    EnterCode();
  } else {
    code = 0;
    codestr = "";
    numcode = 0;
    myGLCD.clrScr();
    myButtons.deleteAllButtons();
    menuTFT = 0;
    menuTFT1();
  }
}

void infofileEff() {
  File datafile = SD.open("/trajet/" + extentionfichier + "file/" + fichierseul + "/datasave/datasave.txt");
  if (datafile) {
    String Data = datafile.readStringUntil('\n');
    DynamicJsonBuffer jsonBuffer;
    JsonObject& obj = jsonBuffer.parseObject(Data);
    Maxsatstr = obj["SatMax"].as<String>();
    MaxAltstr = obj["AltMax"].as<String>();
    MaxSpeedstr = obj["SpeedMax"].as<String>();
    compteTempsstr = obj["temps"].as<String>();
    distanceparcourustr = obj["distParc"].as<String>();
    minuteduTrajet = obj["minute"].as<String>();
    heureduTrajet = obj["heure"].as<String>();
    secondeduTrajet = obj["seconde"].as<String>();
    MaxSatellite = Maxsatstr.toInt();
    MaxAltitude = MaxAltstr.toInt();
    MaxVitesse =  MaxSpeedstr.toInt();
    compteTempsTrajet = compteTempsstr.toInt();
    distanceparcouruTrajet = distanceparcourustr.toFloat();
    heuredata = heureduTrajet.toInt();
    minutedata = minuteduTrajet.toInt();
    secondedata = secondeduTrajet.toInt();
    Serial.print(F("MaxSat: ")); Serial.println(MaxSatellite);
    Serial.print(F("MaxAlt: ")); Serial.println(MaxAltitude);
    Serial.print(F("MaxSpeed: ")); Serial.println(MaxVitesse);
    Serial.print(F("Compte temps: ")); Serial.println(compteTempsTrajet);
    Serial.print(F("Distance parourue: ")); Serial.println(distanceparcouruTrajet);
    datafile.close();
  } else {
    Serial.println(F("Pas de Data de trajet"));
  }

}


void menuTFT1() {

  myGLCD.setFont(BigFont);
  myGLCD.print("Arduino GPS", CENTER, 5);
  myGLCD.setFont(SmallFont);

  but1 = myButtons.addButton( 10,  40, 130,  30, "Infogps");
  but2 = myButtons.addButton( 10,  80, 130,  30, "Date et Heure");

  if (trajet == 0) {
    but3 = myButtons.addButton( 10, 120, 130,  30, "Nouveau Trajet");
  } else {
    but3 = myButtons.addButton( 10, 120, 130,  30, "Arreter Trajet");
  }
  but4 = myButtons.addButton( 10, 160, 130,  30, "List Trajet");

  but6 = myButtons.addButton( 180,  40, 130,  30, "Redemarrer");
  but7 = myButtons.addButton( 180,  80, 130,  30, "Verouiller");
  but5 = myButtons.addButton( 180, 120, 130,  30, "Distance");
  but8 = myButtons.addButton( 180, 160, 130,  30, "Reglage");

  but9 = myButtons.addButton( 10, 200, 130,  30, "ESP8266");

  myButtons.drawButtons();
}

void infogps() {
  //A FAIRE DANS LA BOUCLE DE DEBUT POUR QUE LE TEXTE EN BLANC NE SE REACTUALISE PAS CHAQUE SECONDES

  myGLCD.setFont(BigFont);
  myGLCD.print("Information", CENTER, 5);

  retour = myButtons.addButton( 5,  5, 40,  20, "<--");
  myButtons.drawButtons();

  myGLCD.print("Sat", LEFT, 35);
  myGLCD.print("Sat max", LEFT, 55);
  myGLCD.print("Speed", LEFT, 75);
  myGLCD.print("Speed max", LEFT, 95);
  myGLCD.print("Alt", LEFT, 115);
  myGLCD.print("Alt max", LEFT, 135);
  myGLCD.print("Angle", LEFT, 155);
  myGLCD.print("Route", LEFT, 175);
  myGLCD.print("Lat", LEFT, 195);
  myGLCD.print("Lon", LEFT, 215);

  myGLCD.print("km/h", RIGHT, 75);
  myGLCD.print("km/h", RIGHT, 95);
  myGLCD.print("m", RIGHT, 115);
  myGLCD.print("m", RIGHT, 135);
  myGLCD.print("deg", RIGHT , 155);
  myGLCD.print("km", RIGHT, 175);
  infogpsReload();
}

void infogpsReload() {
  myGLCD.setFont(BigFont);
  myGLCD.setColor(VGA_RED);

  myGLCD.print("   ", 150, 35);
  myGLCD.print(String(GPSSAT), 150, 35);

  myGLCD.print("   ", 150, 55);
  myGLCD.print(String(MaxSat), 150, 55);

  myGLCD.print("    ", 150, 75);
  myGLCD.print(String((int)GPSSPEED), 150, 75);

  myGLCD.print("    ", 150, 95);
  myGLCD.print(String((int)MaxSpeed), 150, 95);

  myGLCD.print("     ", 150, 115);
  myGLCD.print(String((int)GPSALT), 150, 115);

  myGLCD.print("     ", 150, 135);
  myGLCD.print(String((int)MaxAlt), 150, 135);

  myGLCD.print("      ", 150, 155);
  myGLCD.print(String((int)GPSDEG), 150, 155);

  myGLCD.print("       ", 150, 175);
  myGLCD.print(String(distanceparcouru, 1), 150, 175);

  myGLCD.print("             ", 150, 195);
  myGLCD.print(String(GPSLAT, 8), 150, 195);

  myGLCD.print("             ", 150, 215);
  myGLCD.print(String(GPSLONG, 8), 150, 215);

  myGLCD.setColor(VGA_WHITE);
  myGLCD.setFont(SmallFont);
}

void dateheure() {

  myGLCD.setFont(BigFont);
  myGLCD.print("Date et Heure", CENTER, 5);
  myGLCD.setFont(SmallFont);

  retour = myButtons.addButton( 5,  5, 40,  20, "<--");
  myButtons.drawButtons();
  myGLCD.setFont(SevenSegNumFont);

  myGLCD.drawLine(35, 130, 275, 130);

}

void dateheureReload() {
  //HEURE DEBUT CODE

  myGLCD.setFont(SevenSegNumFont);
  myGLCD.print(heureGPS, 25, 60);


  myGLCD.fillCircle(100, 75, 5);
  myGLCD.fillCircle(100, 95, 5);

  myGLCD.print(minutesGPS, 115, 60);

  myGLCD.fillCircle(190, 75, 5);
  myGLCD.fillCircle(190, 95, 5);

  myGLCD.print(secondeGPS, 205, 60);


  //HEURE FIN CODE

  //DATE DEBUT CODE

  myGLCD.print(jourGPS, 25, 150);

  myGLCD.fillCircle(102, 192, 5);

  myGLCD.print(moisGPS, 115, 150);

  myGLCD.fillCircle(192, 192, 5);

  myGLCD.print(anneGPS, 205, 150);

  if (jour != Savejour) {
    myGLCD.clrScr();
    myButtons.deleteAllButtons();
    dateheure();
  }


  myGLCD.setFont(BigFont);
  myGLCD.print(jour, CENTER, 220);
  myGLCD.setFont(SmallFont);
  Savejour = jour;

  //DATE FIN CODE
}

void demtrajet() {

  myGLCD.setFont(BigFont);
  myGLCD.print("Nouveau Trajet", CENTER, 5);
  myGLCD.setFont(SmallFont);
  myButtons.setTextFont(SmallFont);

  retour = myButtons.addButton( 5,  5, 40,  20, "<--");
  myButtons.drawButton(retour);

  myButtons.setTextFont(BigFont);
  but1 = myButtons.addButton( 20,  60, 130,  60, ".KML");
  but2 = myButtons.addButton( 20,  140, 130,  60, ".GPX");
  but3 = myButtons.addButton( 165, 60, 130,  60, ".XML");
  but4 = myButtons.addButton( 165, 140, 130,  60, ".TXT");

  myButtons.drawButton(but1);
  myButtons.drawButton(but2);
  myButtons.drawButton(but3);
  myButtons.drawButton(but4);

  myButtons.setTextFont(SmallFont);
}

void demtrajetTYPE() {

  myGLCD.setFont(BigFont);
  myGLCD.print("Nouveau Trajet", CENTER, 5);
  myGLCD.setFont(BigFont);
  myGLCD.setColor(VGA_GREEN);
  myGLCD.print("Trajet en " + typedefichier, CENTER, 120);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setFont(SmallFont);
}

void arrtrajet() {

  myGLCD.setFont(BigFont);
  myGLCD.print("Arret Trajet", CENTER, 5);
  myGLCD.setFont(BigFont);
  myGLCD.setColor(VGA_RED);
  myGLCD.print("Arrete du trajet", CENTER, 120);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setFont(SmallFont);
}

void efftrajet() {
  efftrajetnormal();
  myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_GRAY);
  retour = myButtons.addButton( 5,  5, 40,  20, "<--");
  myButtons.drawButton(retour);

  myGLCD.setFont(BigFont);
  myGLCD.print("List Trajet", CENTER, 5);
  myGLCD.setFont(SmallFont);
  myButtons.setTextFont(SmallFont);

  myGLCD.print("Fichier:", 10, 50);
  myGLCD.print("Taille:", 10, 90);
  myGLCD.setColor(VGA_RED);
  myGLCD.setFont(BigFont);
  if (nbFichiers != 0) {
    myGLCD.print(tailledufichier, 100, 90);
    myGLCD.print(nomFichier, 100, 50);
  } else {
    myGLCD.print("             ", 100, 90);
    myGLCD.print("             ", 100, 50);
    myGLCD.print("NULL", 100, 90);
    myGLCD.print("NULL", 100, 50);
  }
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);

  myButtons.setButtonColors(VGA_WHITE, VGA_BLUE, VGA_WHITE, VGA_RED, VGA_BLUE);
  but1 = myButtons.addButton( 10,  140, 80,  40, "<--");
  myButtons.drawButton(but1);

  but2 = myButtons.addButton( 230, 140, 80,  40, "-->");
  myButtons.drawButton(but2);

  myButtons.setButtonColors(VGA_WHITE, VGA_GREEN, VGA_WHITE, VGA_RED, VGA_GREEN);
  myButtons.setTextFont(BigFont);
  but3 = myButtons.addButton( 100, 140, 120,  40, "Effacer");
  myButtons.drawButton(but3);
  myButtons.setTextFont(SmallFont);

  myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_GRAY);
  but4 = myButtons.addButton( 10, 190, 210,  40, "Information sur le trajet");
  but5 = myButtons.addButton( 230, 190, 80,  40, "Transfer");
  myButtons.drawButton(but4);
  myButtons.drawButton(but5);
}

void efftrajetReload() {
  myGLCD.setColor(VGA_RED);
  myGLCD.setFont(BigFont);
  if (nbFichiers != 0) {
    myGLCD.print(tailledufichier, 100, 90);
    myGLCD.print(nomFichier, 100, 50);

    nomSansExtentionsDuFichier = nomFichier;
    if (nomFichier.indexOf("KML") != -1) {
      nomExtentionsDuFichier = "kml";
      nomSansExtentionsDuFichier.replace(".KML", "");
    } else if (nomFichier.indexOf("GPX") != -1) {
      nomExtentionsDuFichier = "gpx";
      nomSansExtentionsDuFichier.replace(".GPX", "");
    } else if (nomFichier.indexOf("XML") != -1) {
      nomExtentionsDuFichier = "xml";
      nomSansExtentionsDuFichier.replace(".XML", "");
    } else if (nomFichier.indexOf("TXT") != -1) {
      nomExtentionsDuFichier = "txt";
      nomSansExtentionsDuFichier.replace(".TXT", "");
    }
  } else {
    myGLCD.print("             ", 100, 90);
    myGLCD.print("             ", 100, 50);
    myGLCD.print("NULL", 100, 90);
    myGLCD.print("NULL", 100, 50);
  }
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
}

void efftrajetmoins() {
  efftrajettaille();
  menueffafichmoins();
}

void efftrajetplus() {
  efftrajettaille();
  menueffafichplus();
}

void efftrajetnormal() {
  compterfichier();
  efftrajettaille();
}

void efftrajettaille() {
  fichierseul = nomFichier;
  if (nomFichier.indexOf("KML") != -1) {
    extentionfichier = "kml";
    fichierseul.replace(".KML", "");
  } else if (nomFichier.indexOf("GPX") != -1) {
    extentionfichier = "gpx";
    fichierseul.replace(".GPX", "");
  } else if (nomFichier.indexOf("XML") != -1) {
    extentionfichier = "xml";
    fichierseul.replace(".XML", "");
  } else if (nomFichier.indexOf("TXT") != -1) {
    extentionfichier = "txt";
    fichierseul.replace(".TXT", "");
  }
  Serial.print("fichier : "); Serial.println(nomFichier);
  Serial.print("extention : "); Serial.println(extentionfichier);
  Serial.print("fichierseul : "); Serial.println(fichierseul);
  tailledufichiercoisi();
}

void tailledufichiercoisi() {
  String cheminfichier = "/trajet/" + extentionfichier + "file/" + fichierseul + "/" + nomFichier;
  File fichiersize = SD.open(cheminfichier);
  if (fichiersize) {
    octetFile = fichiersize.size();
    Serial.println(cheminfichier);
    Serial.print(F("Size: ")); Serial.println(octetFile);
    if (octetFile > 1000) {
      octetFile = octetFile / 1000;
      if (octetFile > 1000) {
        octetFile = octetFile / 1000;
        tailledufichier = String(octetFile) + " GB            ";
      } else {
        tailledufichier = String(octetFile) + " KB            ";
      }
    } else {
      tailledufichier = String(octetFile) + " octets          ";
    }
    fichiersize.close();
  }
}
void distanceparc() {

  myGLCD.setFont(BigFont);
  myGLCD.print("Distance", CENTER, 5);
  myGLCD.setFont(BigFont);
  myButtons.setTextFont(SmallFont);

  retour = myButtons.addButton( 5,  5, 40,  20, "<--");
  myButtons.drawButton(retour);

  myGLCD.print("Arrivee Lat:", 10, 40);
  myGLCD.print("Arrivee Long:", 10, 110);

  myButtons.setTextFont(BigFont);
  myButtons.setButtonColors(VGA_WHITE, VGA_BLACK, VGA_WHITE, VGA_WHITE, VGA_BLACK);
  myGLCD.setFont(BigFont);
  myGLCD.print(latDesti, 20, 70);
  myGLCD.print(lonDesti, 20, 140);
  but1 = myButtons.addButton( 200, 70, 60,  20,  "...");
  but2 = myButtons.addButton( 200, 140, 60,  20, "...");
  myButtons.drawButton(but1);
  myButtons.drawButton(but2);

  myGLCD.drawLine(35, 170, 285, 170);

  myGLCD.setFont(SevenSegNumFont);
  myGLCD.print("00000", 10, 180);

  myGLCD.setFont(BigFont);
  myGLCD.print("Km", 190, 210);

  myButtons.setTextFont(SmallFont);
  myGLCD.setFont(SmallFont);
}

void distanceparcReload() {
  if (lonDesti != "" && latDesti != "" && latNOW != 0.0 && lonNOW != 0.0) {
    distance = calcule_distance( latNOW, lonNOW, latDesti.toDouble(), lonDesti.toDouble());
  }
  myGLCD.setColor(VGA_BLACK);
  myGLCD.fillRect(0, 180, 180, 230);
  myGLCD.setColor(VGA_WHITE);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.print(String((int)distance), 10, 180);
}

void datasaveeff() {
  infofileEff();
  myGLCD.setFont(BigFont);
  myGLCD.print("Info Trajet", CENTER, 5);
  myGLCD.setFont(SmallFont);
  myButtons.setTextFont(SmallFont);

  retour = myButtons.addButton( 5,  5, 40,  20, "<--");
  myButtons.drawButton(retour);

  myGLCD.print("Nom du fichier:", 20, 40);
  myGLCD.print("Taille du fichier:", 20, 60);
  myGLCD.print("Satellite Max:", 20, 80);
  myGLCD.print("Altitude Max:", 20, 100);
  myGLCD.print("Vitesse Max:", 20, 120);
  myGLCD.print("Temps:", 20, 140);
  myGLCD.print("Distance parcouru:", 20, 160);

  myGLCD.setColor(VGA_RED);

  String tempsduparc = String(heuredata) + ":" + String(minutedata) + ":" + String(secondedata);

  myGLCD.print(nomFichier, 150, 40);
  myGLCD.print(tailledufichier, 170, 60);
  myGLCD.print(String(MaxAlt), 140, 80);
  myGLCD.print(String(MaxSat), 130, 100);
  myGLCD.print(String(MaxSpeed), 120, 120);
  myGLCD.print(tempsduparc, 70, 140);
  myGLCD.print(String(DParcourue, 3), 170, 160);

  myGLCD.setColor(VGA_WHITE);

}

void EnterCode() {

  myButtons.setTextFont(BigFont);
  myButtons.setButtonColors(VGA_WHITE, VGA_BLUE, VGA_WHITE, VGA_RED, VGA_BLUE);
  but1 = myButtons.addButton( 75,  154, 50, 40, "1");
  but2 = myButtons.addButton( 135,  154, 50,  40, "2");
  but3 = myButtons.addButton( 195,  154, 50,  40, "3");
  but4 = myButtons.addButton( 75,  112, 50,  40, "4");
  but5 = myButtons.addButton( 135,  112, 50,  40, "5");
  but6 = myButtons.addButton( 195,  112, 50,  40, "6");
  but7 = myButtons.addButton( 75,  70, 50,  40, "7");
  but8 = myButtons.addButton(135,  70, 50,  40, "8");
  but9 = myButtons.addButton( 195,  70, 50,  40, "9");
  but10 = myButtons.addButton( 75,  196, 170,  35, "0");
  myButtons.drawButtons();

  EnterCodeReload();
}

void EnterCodeReload() {
  myGLCD.drawLine(34, 60, 284, 60);
  myGLCD.setColor(VGA_RED);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.print(codestr, CENTER, 5);
  myGLCD.setColor(VGA_WHITE);
  myButtons.setTextFont(SmallFont);
  myGLCD.setFont(SmallFont);
}

void clavierDigitale() {
  myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_GRAY);
  retour = myButtons.addButton( 5,  5, 40,  20, "<--");
  myButtons.drawButton(retour);

  myGLCD.drawLine(30, 70, 290, 70);
  myGLCD.setFont(BigFont);
  myGLCD.print(clavier, CENTER, 30);
  myButtons.setTextFont(BigFont);
  but1 = myButtons.addButton( 140,  190, 40, 40, "1");
  but2 = myButtons.addButton( 90,  190, 40,  40, "2");
  but3 = myButtons.addButton( 40,  190, 40,  40, "3");
  but4 = myButtons.addButton( 40,  140, 40,  40, "4");
  but5 = myButtons.addButton( 90,  140, 40,  40, "5");
  but6 = myButtons.addButton( 140,  140, 40,  40, "6");
  but7 = myButtons.addButton( 40,  90, 40,  40, "7");
  but8 = myButtons.addButton(90,  90, 40,  40, "8");
  but9 = myButtons.addButton( 140,  90, 40,  40, "9");
  but10 = myButtons.addButton( 190,  190, 40,  40, "0");
  but11 = myButtons.addButton(190,  140, 40,  40, ".");
  but12 = myButtons.addButton( 190,  90, 40,  40, "-");
  butOK = myButtons.addButton( 240,  90, 40,  140, "OK");
  myButtons.drawButton(but1);
  myButtons.drawButton(but2);
  myButtons.drawButton(but3);
  myButtons.drawButton(but4);
  myButtons.drawButton(but5);
  myButtons.drawButton(but6);
  myButtons.drawButton(but7);
  myButtons.drawButton(but8);
  myButtons.drawButton(but9);
  myButtons.drawButton(but10);
  myButtons.drawButton(but11);
  myButtons.drawButton(but12);
  myButtons.drawButton(butOK);
  myButtons.setTextFont(SmallFont);
  myGLCD.setFont(SmallFont);

  clavierDigitaleReload();
}

void clavierDigitaleReload() {
  myGLCD.setFont(BigFont);
  myGLCD.print(clavier, CENTER, 30);
  myGLCD.setFont(SmallFont);
}

void MenuReglage() {
  myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_GRAY);
  myButtons.setTextFont(SmallFont);
  retour = myButtons.addButton( 5,  5, 50,  20, "<--");
  myButtons.drawButton(retour);

  myGLCD.setFont(BigFont);
  myGLCD.print("Reglage", CENTER, 5);
  myButtons.setTextFont(BigFont);

  myGLCD.setFont(BigFont);

  myGLCD.print("Ecriture SD", LEFT, 40);
  myGLCD.print("Heure UTC", LEFT, 80);
  myGLCD.print("Activer Log", LEFT, 120);
  myGLCD.print("Rep. trajet", LEFT, 160);
  myGLCD.print("Activer Code", LEFT, 200);

  but1 = myButtons.addButton( 205,  35, 25, 25, "-");
  but2 = myButtons.addButton( 275,  35, 25,  25, "+");
  but3 = myButtons.addButton( 205,  75, 25,  25, "-");
  but4 = myButtons.addButton( 275,  75, 25,  25, "+");
  but5 = myButtons.addButton( 205,  110, 95,  25, "NON");
  but6 = myButtons.addButton( 205,  152, 95,  25, "NON");
  but7 = myButtons.addButton( 205,  195, 95,  25, "NON");
  myGLCD.setFont(SmallFont);
  if (frequenceSD < 10) {
    myGLCD.print("0" + String(frequenceSD), 245, 40);
  } else {
    myGLCD.print(String(frequenceSD), 245, 40);
  }
  if (nbrheure < 10 && nbrheure > 0) {
    myGLCD.print("0" + String(nbrheure), 245, 80);
  } else {
    myGLCD.print(String(nbrheure), 245, 80);
  }
  myGLCD.setFont(BigFont);
  myButtons.drawButton(but1);
  myButtons.drawButton(but2);
  myButtons.drawButton(but3);
  myButtons.drawButton(but4);
  myButtons.drawButton(but5);
  myButtons.drawButton(but6);
  myButtons.drawButton(but7);
  myButtons.drawButton(but8);
  myButtons.drawButton(but9);
  myButtons.drawButton(but10);
  myButtons.drawButton(but11);
  myButtons.drawButton(but12);
  myButtons.drawButton(butOK);
  myButtons.setTextFont(SmallFont);
  myGLCD.setFont(SmallFont);

  MenuReglageReload();
}

void MenuESP() {
  myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_GRAY);
  retour = myButtons.addButton( 5,  5, 40,  20, "<--");
  myButtons.drawButton(retour);

  myGLCD.setFont(BigFont);
  myGLCD.print("ESP8266 WIFI", CENTER, 5);
  myGLCD.setFont(SmallFont);
  myButtons.setTextFont(SmallFont);

  myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_GRAY);
  but1 = myButtons.addButton( 30, 50, 250,  40, "Activer le WIFI");
  myButtons.drawButton(but1);

  myGLCD.setFont(SmallFont);
  myGLCD.print("Blynk: YHJSDjxqo6aceb-vW_srzDr8jXW3Pli6", 0, 120);
  myGLCD.setFont(BigFont);

}

void MenuReglageReload() {
  myButtons.setTextFont(BigFont);
  myGLCD.print("   ", 245, 40);
  if (frequenceSD < 10) {
    myGLCD.print("0" + String(frequenceSD), 245, 40);
  } else {
    myGLCD.print(String(frequenceSD), 245, 40);
  }
  myGLCD.print("   ", 245, 80);
  if (nbrheure < 10 && nbrheure > 0) {
    myGLCD.print("0" + String(nbrheure), 245, 80);
  } else {
    myGLCD.print(String(nbrheure), 245, 80);
  }

  if (activelog == 0) {
    myButtons.relabelButton(but5, "NON");
    myButtons.drawButton(but5);
  } else if (activelog == 1) {
    myButtons.relabelButton(but5, "OUI");
    myButtons.drawButton(but5);
  }

  if (activecode == 0) {
    myButtons.relabelButton(but7, "NON");
    myButtons.drawButton(but7);
  } else if (activecode == 1) {
    myButtons.relabelButton(but7, "OUI");
    myButtons.drawButton(but7);
  }

  if (reptrajet == 0) {
    myButtons.relabelButton(but6, "NON");
    myButtons.drawButton(but6);
  } else if (reptrajet == 1) {
    myButtons.relabelButton(but6, "OUI");
    myButtons.drawButton(but6);
  }
  myButtons.setTextFont(SmallFont);
}

void syncroFichier(String fichitype, String nomsansexten) {
  String chemin = "/trajet/" + fichitype + "file/" + nomsansexten + "/" + nomsansexten + "." + fichitype;
  myFile = SD.open(chemin);
  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    nodemcu.print("\n");
    nodemcu.print("startrecording26050640.kml");
    nodemcu.print("\n");
    myGLCD.clrScr();
    televersementESP();
    myGLCD.setFont(BigFont);
    int countligne = 0;
    int lignesidentique = 0;
    String olddata = "";
    while (myFile.available()) {
      String mydata = myFile.readStringUntil('\r');
      if (mydata != olddata) {
        nodemcu.print(mydata);
        countligne ++;
      } else {
        lignesidentique ++;
      }
      myGLCD.setColor(VGA_GREEN);
      myGLCD.print(String(countligne), 30, 200);
      myGLCD.setColor(VGA_WHITE);

      myGLCD.setColor(VGA_RED);
      myGLCD.print(String(lignesidentique), 210, 200);
      myGLCD.setColor(VGA_WHITE);
      olddata = mydata;
    }
    myGLCD.setFont(SmallFont);
    countligne = 0;
    nodemcu.print("\n");
    nodemcu.print("endrecording");
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void televersementESP() {
  myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_GRAY);
  myGLCD.setFont(BigFont);
  myGLCD.print("Televersement", 40, 30);
  myGLCD.print("en cour ...", 60, 50);
  myGLCD.print("Ne pas debrancher", 20, 80);
  myGLCD.print("avant la fin", 50, 100);
  myGLCD.print("de l'envoie", 60, 120);
  myGLCD.print("Ligne envoyee :", 10, 170);
  myGLCD.setFont(SmallFont);
}

uint32_t timer = millis();
uint32_t timerSD = millis();
void loop()
{

  while (Serial3.available() > 0)
    if (gps.encode(Serial3.read()))
      if (millis() > 5000 && gps.charsProcessed() < 10)
      {
        Serial.println(F("No GPS detected: check wiring."));
      }

  if (gps.location.isValid())
  {
    GPSLONG = gps.location.lng();
    //float
    GPSLAT = gps.location.lat();
  }
  //int
  GPSSPEED = gps.speed.kmph();
  //int
  GPSALT = gps.altitude.meters();
  //int
  GPSDEG = gps.course.deg();
  //int
  GPSSAT = gps.satellites.value();

  if (gps.time.isValid())
  {
    //int
    GPSHOURS = gps.time.hour();
    //int
    GPSMINUTES = gps.time.minute();
    //int
    GPSSECONDES = gps.time.second();
  }

  if (gps.date.isValid())
  {
    //int
    GPSDAY = gps.date.day();
    //int
    GPSMONTH = gps.date.month();
    //int
    GPSYEAR = gps.date.year();
  }

  if (GPSSAT < 20) {
    if (GPSSAT > MaxSat) {
      MaxSat = GPSSAT;
    }
  }
  if (GPSALT > MaxAlt) {
    MaxAlt = GPSALT;
  }
  if (GPSSPEED > MaxSpeed) {
    MaxSpeed = GPSSPEED;
  }

  int pressed_button;
  boolean default_colors = false;

  if (millis() - timer > 1000) {  //toute les 1 secondes on fait une action
    timer = millis(); // reset the timer

    if (initvaldef == 0) {
      MaxSat = 0;
      MaxAlt = 0;
      MaxSpeed = 0;
      distanceparcouru = 0.0;
      initvaldef = 1;
    }

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

    TRAJET[0] = day()   / 10 + '0';
    TRAJET[1] = day()   % 10 + '0';
    TRAJET[2] = month() / 10 + '0';
    TRAJET[3] = month() % 10 + '0';
    TRAJET[4] = hour()   / 10 + '0';
    TRAJET[5] = hour()   % 10 + '0';
    TRAJET[6] = minute() / 10 + '0';
    TRAJET[7] = minute() % 10 + '0';

    heureGPS[0] = hour()   / 10 + '0';
    heureGPS[1] = hour()   % 10 + '0';
    minutesGPS[0] = minute()   / 10 + '0';
    minutesGPS[1] = minute()   % 10 + '0';
    secondeGPS[0] = second()   / 10 + '0';
    secondeGPS[1] = second()   % 10 + '0';

    anneGPS[0] = (year()  / 10) % 10 + '0';
    anneGPS[1] = year()  % 10 + '0';
    moisGPS[0] = month() / 10 + '0';
    moisGPS[1] = month() % 10 + '0';
    jourGPS[0] = day()   / 10 + '0';
    jourGPS[1] = day()   % 10 + '0';
    Serial.print(heureGPS + ":"); Serial.print(minutesGPS + ":"); Serial.println(secondeGPS);
    Serial.print(jourGPS + "/"); Serial.print(moisGPS + "/"); Serial.println(anneGPS);
    print_wday(weekday());

    Serial.print(" ");
    Serial.print(Time); //on affiche l'heure
    Serial.print(" ");
    Serial.print(Date);//on affiche la date
    Serial.print(" Trajet name: ");
    Serial.print(TRAJET);//on affiche la date
    Serial.println();

    StaticJsonBuffer<1024> jsonBuffer;
    JsonObject& data = jsonBuffer.createObject();

    //Assign collected data to JSON Object

    if (GPSSAT > 4) {
      data["Time"] = Time;
      data["Date"] = Date;
      data["Sat"] = GPSSAT;
      data["MaxSat"] = MaxSat;
      data["Vitesse"] = GPSSPEED;
      data["VitesseMax"] = MaxSpeed;
      data["Altitude"] = GPSALT;
      data["AltitudeMax"] = MaxAlt;
      data["Angle"] = GPSDEG;
      data["DistParcouru"] = String(distanceparcouru, 3);
      data["Hum"] = h;
      data["Temp"] = t;
      data["Lat"] = String(GPSLAT, 8);
      data["Long"] = String(GPSLONG, 8);
      //Send data to NodeMCU
      String nodemcuStr = "";
      data.prettyPrintTo(nodemcuStr);
      jsonBuffer.clear();
      nodemcu.print(nodemcuStr);

      latNOW = GPSLAT;
      lonNOW = GPSLONG;
      if (calculeladistance == 10) {
        if (GPSLONG != 0.0 && GPSLAT != 0.0) {
          distancecalc = calcule_distance( latNOW, lonNOW, oldlatnow, oldlonnow);
          if (distancecalc < 0.100) {
            distanceparcouru = distanceparcouru + distancecalc;
          }
        }
      } else {
        calculeladistance ++;
      }
      oldlatnow = latNOW;
      oldlonnow = lonNOW;

      if (menuTFT == 1) {
        infogpsReload();
      } else if (menuTFT == 2) {
        dateheureReload();
      } else if (menuTFT == 4) {
        if (datasave != 1) {
          tailledufichiercoisi();
          efftrajetReload();
        }
      }
      else if (menuTFT == 5 && clavierdigit == 0) {
        distanceparcReload();
      }

    }
  }
  if (GPSSAT > 4) {
    if (millis() - timerSD > (frequenceSD * 1000)) {  //toute les 1 secondes on fait une action
      timerSD = millis(); // reset the timer

      if (trajet == 2) {
        if (String(GPSLONG, 8) != "0.00000000" && String(GPSLAT, 8) != "0.00000000") {
          write_sd_card();
        }
      }

    }
  }
  if (myTouch.dataAvailable() == true) {
    pressed_button = myButtons.checkButtons();

    if (pressed_button != -1) {
      if (code != 1) {
        if (menuTFT == 0) {                   //menuTFT 1
          if (pressed_button == but1) {
            Serial.println("but1");
            myGLCD.clrScr();
            myButtons.deleteAllButtons();
            menuTFT = 1;
            infogps();
          } else if (pressed_button == but2) {
            Serial.println("but2");
            myGLCD.clrScr();
            myButtons.deleteAllButtons();
            menuTFT = 2;
            dateheure();
          } else if (pressed_button == but3) {
            Serial.println("but3");
            myGLCD.clrScr();
            myButtons.deleteAllButtons();
            menuTFT = 3;
            if (trajet == 0) {
              demtrajet();
            } else {
              arrtrajet();
              delay(1500);
              myGLCD.clrScr();
              myButtons.deleteAllButtons();
              menuTFT = 0;
              distanceparcouru = 0.0;
              octetFile = 0;
              compteTemps = 0;
              finish_file();
              trajet = 0;
              menuTFT1();
            }
          } else if (pressed_button == but4) {
            Serial.println("but4");
            myGLCD.clrScr();
            myButtons.deleteAllButtons();
            menuTFT = 4;
            efftrajet();
          } else if (pressed_button == but5) {
            Serial.println("but5");
            myGLCD.clrScr();
            myButtons.deleteAllButtons();
            menuTFT = 5;
            distanceparc();
          } else if (pressed_button == but6) {
            Serial.println("but6");
            myGLCD.clrScr();
            myButtons.deleteAllButtons();
            menuTFT = 6;
            reset();
          } else if (pressed_button == but7) {
            Serial.println("but7");
            myGLCD.clrScr();
            myButtons.deleteAllButtons();
            menuTFT = 7;
            code = 1;
            EnterCode();
          } else if (pressed_button == but8) {
            Serial.println("but8");
            myGLCD.clrScr();
            myButtons.deleteAllButtons();
            menuTFT = 8;
            MenuReglage();
          } else if (pressed_button == but9) {
            Serial.println("but9");
            myGLCD.clrScr();
            myButtons.deleteAllButtons();
            menuTFT = 9;
            MenuESP();
          }
        } else if (menuTFT != 0) {           //page infogps

          if (pressed_button == retour && datasave == 0 && clavierdigit == 0) {
            if (menuTFT == 8) {
              String objectjson = "";

              StaticJsonBuffer<1024> jsonBuffer;
              JsonObject& data = jsonBuffer.createObject();

              //Assign collected data to JSON Object
              data["SD"] = frequenceSD;
              data["UTC"] = nbrheure;
              data["activelog"] = activelog;
              data["activecode"] = activecode;
              data["reptrajet"] = reptrajet;
              data.printTo(objectjson);
              Serial.println(objectjson);
              jsonBuffer.clear();

              SD.remove("/p/param.txt");
              myFile = SD.open("/p/param.txt", FILE_WRITE);
              myFile.print(objectjson);
              myFile.close();
            }
            Serial.println("retour");
            myGLCD.clrScr();
            myButtons.deleteAllButtons();
            menuTFT = 0;
            menuTFT1();
          } else if (menuTFT == 1) {

          } else if (menuTFT == 2) {

          } else if (menuTFT == 3) {
            if (trajet == 0) {
              if (pressed_button == but1) {
                typedefichier = "kml";
                trajet = 1;
              } else if (pressed_button == but2) {
                typedefichier = "gpx";
                trajet = 1;
              } else if (pressed_button == but3) {
                typedefichier = "xml";
                trajet = 1;
              } else if (pressed_button == but4) {
                typedefichier = "txt";
                trajet = 1;
              }

              nameFile = (String)TRAJET;
              myGLCD.clrScr();
              myButtons.deleteAllButtons();
              start_file();
              trajet = 2;
              demtrajetTYPE();
              delay(1500);
              myGLCD.clrScr();
              myButtons.deleteAllButtons();
              menuTFT = 0;
              menuTFT1();
            }
          } else if (menuTFT == 4) {
            if (datasave == 0) {
              if (pressed_button == but1) {
                menueffafichmoins();
                efftrajettaille();
                efftrajetReload();
                tailledufichiercoisi();
              } else if (pressed_button == but2) {
                menueffafichplus();
                efftrajettaille();
                efftrajetReload();
                tailledufichiercoisi();
              } else if (pressed_button == but3) {
                nbFichiers = nbFichiers - 1;
                compterfichier();
                Serial.print("fichier : "); Serial.println(nomFichier);
                Serial.print("extention : "); Serial.println(extentionfichier);
                Serial.print("fichierseul : "); Serial.println(fichierseul);
                Serial.println("EFFACER");
                SD.remove("/trajet/" + extentionfichier + "file/" + fichierseul + "/" + nomFichier);
                SD.remove("/trajet/" + extentionfichier + "file/" + fichierseul + "/data.txt");
                SD.remove("/trajet/" + extentionfichier + "file/" + fichierseul + "/log.txt");
                SD.remove("/trajet/" + extentionfichier + "file/" + fichierseul + "/recap.txt");
                SD.remove("/trajet/" + extentionfichier + "file/" + fichierseul + "/datasave/datasave.txt");
                SD.remove("/trajet/nomfich/" + nomFichier);
                SD.rmdir("/trajet/" + extentionfichier + "file/" + fichierseul + "/datasave/");
                SD.rmdir("/trajet/" + extentionfichier + "file/" + fichierseul);
                compterfichier();
                efftrajettaille();
                tailledufichiercoisi();
                efftrajetReload();
              } else if (pressed_button == but4) {
                myGLCD.clrScr();
                myButtons.deleteAllButtons();
                datasaveeff();
                datasave = 1;
              } else if (pressed_button == but5) {
                if (nbFichiers != 0) {
                  syncroFichier(nomExtentionsDuFichier, nomSansExtentionsDuFichier);
                  myGLCD.clrScr();
                  myButtons.deleteAllButtons();
                  menuTFT = 4;
                  efftrajet();
                }
              }
            } else if (datasave == 1) {
              myGLCD.clrScr();
              myButtons.deleteAllButtons();
              efftrajet();
              datasave = 0;
            }
          } else if (menuTFT == 5) {
            if (clavierdigit == 0) {
              Saveclavier = clavier;
              clavier = "";
              myGLCD.clrScr();
              myButtons.deleteAllButtons();
              clavierDigitale();
              if (pressed_button == but1) {
                clavierdigit = 1;
                Serial.println("clavier Lat");
              } else if (pressed_button == but2) {
                clavierdigit = 2;
                Serial.println("clavier Lon");
              }
            } else if (clavierdigit == 1 || clavierdigit == 2) {
              if (pressed_button == retour) {
                myGLCD.clrScr();
                myButtons.deleteAllButtons();
                clavierdigit = 0;
                clavier = Saveclavier;
                Serial.println(clavier);
                distanceparc();
              } else if (pressed_button == but1) {
                clavier = clavier + "1";
              } else if (pressed_button == but2) {
                clavier = clavier + "2";
              } else if (pressed_button == but3) {
                clavier = clavier + "3";
              } else if (pressed_button == but4) {
                clavier = clavier + "4";
              } else if (pressed_button == but5) {
                clavier = clavier + "5";
              } else if (pressed_button == but6) {
                clavier = clavier + "6";
              } else if (pressed_button == but7) {
                clavier = clavier + "7";
              } else if (pressed_button == but8) {
                clavier = clavier + "8";
              } else if (pressed_button == but9) {
                clavier = clavier + "9";
              } else if (pressed_button == but10) {
                clavier = clavier + "0";
              } else if (pressed_button == but11) {
                clavier = clavier + ".";
              } else if (pressed_button == but12) {
                clavier = clavier + "-";
              } else if (pressed_button == butOK) {

                if (clavierdigit == 1) {
                  latDesti = clavier;
                } else if (clavierdigit == 2) {
                  lonDesti = clavier;
                }

                String objectjson = "";

                StaticJsonBuffer<1024> jsonBuffer;
                JsonObject& data = jsonBuffer.createObject();

                //Assign collected data to JSON Object
                data["Lat"] = latDesti;
                data["Lon"] = lonDesti;
                data.printTo(objectjson);
                Serial.println(objectjson);
                jsonBuffer.clear();

                SD.remove("/p/d/distance.txt");
                myFile = SD.open("/p/d/distance.txt", FILE_WRITE);
                myFile.print(objectjson);
                myFile.close();

                myGLCD.clrScr();
                myButtons.deleteAllButtons();
                clavierdigit = 0;
                Serial.println(clavier);
                distanceparc();
              }

              if (clavierdigit != 0) {
                Serial.println(clavier);
                clavierDigitaleReload();
              }
            }
          } else if (menuTFT == 6) {

          } else if (menuTFT == 7) {

          } else if (menuTFT == 8) {
            if (pressed_button == but1) {
              if (frequenceSD != 1) {
                frequenceSD --;
                MenuReglageReload();
              }
            } else if (pressed_button == but2) {
              if (frequenceSD != 99) {
                frequenceSD ++;
                MenuReglageReload();
              }
            } else if (pressed_button == but3) {
              if (nbrheure != -12) {
                nbrheure --;
                MenuReglageReload();
              }
            } else if (pressed_button == but4) {
              if (nbrheure != 12) {
                nbrheure ++;
                MenuReglageReload();
              }
            } else if (pressed_button == but5) {
              if (activelog == 0) {
                activelog = 1;
              } else if (activelog == 1) {
                activelog = 0;
              }
              MenuReglageReload();
            } else if (pressed_button == but6) {
              if (reptrajet == 0) {
                reptrajet = 1;
              } else if (reptrajet == 1) {
                reptrajet = 0;
              }
              MenuReglageReload();

            } else if (pressed_button == but7) {
              if (activecode == 0) {
                activecode = 1;
              } else if (activecode == 1) {
                activecode = 0;
              }
              MenuReglageReload();

            }
          } else if (menuTFT == 9) {
            if (pressed_button == but1) {
              nodemcu.print("startwebserver");
              delay(50);
              myGLCD.clrScr();
              myGLCD.setFont(BigFont);
              myGLCD.print("Nom du Portail :", 10, 10);
              myGLCD.print("GPS-WIFI-Config", 30, 50);
              myGLCD.print("Mot de passe :", 10, 110);
              myGLCD.print("Esp8266gps123!", 30, 150);
              delay(10000);
              myGLCD.clrScr();
              myButtons.deleteAllButtons();
              menuTFT = 0;
              menuTFT1();
            }
          }

        }
      } else if (code == 1) {
        if (numcode < nombrenumcode) {
          if (pressed_button == but1) {
            codestr = codestr + "1";
            numcode ++;
          } else if (pressed_button == but2) {
            codestr = codestr + "2";
            numcode ++;
          } else if (pressed_button == but3) {
            codestr = codestr + "3";
            numcode ++;
          } else if (pressed_button == but4) {
            codestr = codestr + "4";
            numcode ++;
          } else if (pressed_button == but5) {
            codestr = codestr + "5";
            numcode ++;
          } else if (pressed_button == but6) {
            codestr = codestr + "6";
            numcode ++;
          } else if (pressed_button == but7) {
            codestr = codestr + "7";
            numcode ++;
          } else if (pressed_button == but8) {
            codestr = codestr + "8";
            numcode ++;
          } else if (pressed_button == but9) {
            codestr = codestr + "9";
            numcode ++;
          } else if (pressed_button == but10) {
            codestr = codestr + "0";
            numcode ++;
          }
        }
        EnterCodeReload();
        if (numcode >= nombrenumcode) {
          if (codestr == codeverif) {
            code = 0;
            codestr = "";
            numcode = 0;
            myGLCD.clrScr();
            myButtons.deleteAllButtons();
            menuTFT = 0;
            menuTFT1();
          } else {
            delay(500);
            codestr = "";
            numcode = 0;
            myGLCD.clrScr();
            myButtons.deleteAllButtons();
            EnterCode();
          }
        }
      }
      Serial.println(codestr);
      delay(150);
    }
    myButtons.setButtonColors(VGA_WHITE, VGA_GRAY, VGA_WHITE, VGA_RED, VGA_GRAY);
  }
}

void print_wday(byte wday)
{
  switch (wday)
  {

    case 1:  Serial.print(F("Dimanche")); jour = "Dimanche";  break;
    case 2:  Serial.print(F("Lundi")); jour = "Lundi";  break;
    case 3:  Serial.print(F("Mardi")); jour = "Mardi";  break;
    case 4:  Serial.print(F("Mercredi")); jour = "Mercredi";  break;
    case 5:  Serial.print(F("Jeudi")); jour = "Jeudi";  break;
    case 6:  Serial.print(F("Vendredi")); jour = "Vendredi";  break;
    default: Serial.print(F("Samedi")); jour = "Samedi";

  }

}
