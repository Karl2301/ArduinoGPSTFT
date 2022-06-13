void SDverif() {
  Serial.println("Reconstruction du dossier /trajet/ ");
  if (!SD.exists("/trajet/")) {
    SD.mkdir("/trajet/");
    SD.mkdir("/trajet/KMLFILE/");
    SD.mkdir("/trajet/XMLFILE/");
    SD.mkdir("/trajet/GPXFILE/");
    SD.mkdir("/trajet/TXTFILE/");
    Serial.println("Dossier crée");
  }

  //créé fichier code
  Serial.println("Reconstruction du dossier /p/ ");
  if (!SD.exists("/p/")) {
    SD.mkdir("/p/");
    Serial.println("Reconstruction du fichier /p/code.txt");
    File code = SD.open("/p/code.txt", FILE_WRITE);
    if (code) {
      code.print("123456");
      Serial.println("Reconstruction du fichier /p/code.txt réussi !");
    }
    code.close();
    //fin fichier code

    //créé fichier oaram
    Serial.println("Reconstruction du fichier /p/param.txt ");
    File param = SD.open("/p/param.txt", FILE_WRITE);
    if (param) {
      String objectjson = "";

      StaticJsonBuffer<1024> jsonBuffer;
      JsonObject& data = jsonBuffer.createObject();

      //Assign collected data to JSON Object
      data["SD"] = "1";
      data["UTC"] = "2";
      data["activelog"] = "1";
      data["activecode"] = "0";
      data["reptrajet"] = "1";
      data.printTo(objectjson);
      Serial.println(objectjson);
      jsonBuffer.clear();
      param.print(objectjson);
      Serial.println("Reconstruction du fichier /p/param.txt réussi !");
    }
    param.close();
    //fin fichier param

    //créé fichier code
    Serial.println("Reconstruction du dossier /p/t/");
    SD.mkdir("/p/t/");
    Serial.println("Reconstruction du fichier p/t/tjtcour.txt ");
    File trjtencour = SD.open("p/t/tjtcour.txt", FILE_WRITE);
    if (trjtencour) {
      trjtencour.print("");
      Serial.println("Reconstruction du fichier p/t/tjtcour.txt reussi !");
    }
    trjtencour.close();
    //fin fichier code

    //créé fichier code
    Serial.println("Reconstruction du dossier /p/d/");
    SD.mkdir("/p/d/");
    Serial.println("Reconstruction du fichier p/d/distance.txt ");
    File distSD = SD.open("p/d/distance.txt", FILE_WRITE);
    if (distSD) {
      distSD.print("");
      Serial.println("Reconstruction du fichier p/d/distance.txt reussi !");
    }
    distSD.close();
    //fin fichier code
  }
}
