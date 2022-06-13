# ArduinoGPSTFT
Mon traceur GPS avec un écran TFT

Composant:
  - Arduino Mega
  - Sield d'écrans tactile TFT
  - Un ecrans TFT tactile
  - Un module GPS NEO6M-V2
  - Un module DHT22
  - Carte ESP8266
  - Antenne GPS

Branchement:

    GPS --- Arduino
    Vin  ->   5V
    GND  ->   GND
    TX   ->   RX3
    RX   ->   TX3
   
    ESP8266 ------- Arduino
    VIN    ->   5V
    GND    ->   GND
    6      ->   8
    7      ->   9
    
  
# Adapter son code a ses branchements: 

Changer la broche du DHT22:

    #define brocheDeBranchementDHT 2

Changer les broche de communication avec l'esp:

    SoftwareSerial nodemcu(8, 9);

Changer les broche pour l'écran ( ce sont normalement les pin par défauts ):

    UTFT    myGLCD(ILI9341_16, 38, 39, 40, 41);
    URTouch  myTouch( 6, 5, 4, 3, 2);
 
Changer la vitesse de communication avec l'esp:

    Serial3.begin(9600);
  
Changer le pin de la carte SD:

    if (!SD.begin(53)) {
      Serial.println(F("SD absente ou HS. stop."));
    }
  
Si vous voulez avoir les retour du GPS remplacez "false" par "true":

    #define GPSECHO  false
    
    
# Fonctionnement du boitier:

Quand on lance un trajet, le boitier commencé à créer un fichier d'existence (pour pouvoir reprendre un trajet si le boitier s'éteint ou crash).Puis il crée le fichier KML si le Gps capte et recois d'information des satellites. Ensuite, toutes les secondes, le boitier enregistre les données dans le fichier kml et dans un fichier de log. Quand on veut arrêter le trajet, le boitier créé un fichier de récapitulation du trajet.

# L' ESP8266

L'esp8266 est un module Wi-Fi, il se connecte à des réseaux Wi-Fi 2.4G et communiquent les données à l'application Blynk pour pouvoir contrôler le boitier
sans fil. On peut configurer le Wi-Fi via le boitier, l'esp créé un Wi-Fi et ensuite héberge une page où l'on peut configurer le Wi-Fi.
