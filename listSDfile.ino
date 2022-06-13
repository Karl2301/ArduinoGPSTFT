uint16_t compteNbFichiers()
{
  uint16_t n = 0;
  File dossier = SD.open("/trajet/nomfich/");
  File fichier;

  if (dossier) {
    dossier.rewindDirectory(); // on se met au début

    while (fichier = dossier.openNextFile()) {
      //verifier si c'est un repertoire
      Serial.println(fichier.name());
      String filenameverif = fichier.name();
      int dtctiretdubas = filenameverif.indexOf("_");
      Serial.print("dtctiretdubas: "); Serial.println(dtctiretdubas);
      if (dtctiretdubas == -1) {
        n++;
      } else if (SD.remove(fichier.name())) {
        Serial.println(fichier.name());
      }

      fichier.close(); // on ferme celui là
    }
    dossier.close();
  } else Serial.println(F("Erreur lecture racine"));

  return n;
}

//-------------------------------------------------
// affiche un fichier à la racine de la carte SD.
// le nombre total de fichiers doit être correct
//-------------------------------------------------
const char * affiFichier(uint16_t num) {
  uint16_t n = 0; // +1 pour le '\0' à la fin, static pour que la mémoire reste allouée car on exporte ce pointeur vers  le reste du programme
  const byte maxFileLength = 100;
  static char nomDuFichier[maxFileLength + 1];
  nomDuFichier[0] = '\0'; // initialisé à vide

  File dossier = SD.open("/trajet/nomfich/");
  dossier.rewindDirectory(); // on se met au début
  File fichier;

  if (num <= nbFichiers) {
    while (n <= num) {
      fichier = dossier.openNextFile(); // on passe au suivant
      if (fichier) {
        n++; // on se souvient où on est
        if (n == num) { // si on a trouvé le bon, on copie son nom
          strncpy(nomDuFichier, fichier.name(), maxFileLength);
          nomDuFichier[maxFileLength] = '\0'; // par précaution si le nom était trop long
          Serial.print(fichier.name());
          Serial.print(F(" "));
          Serial.print(octetFile); Serial.print(F(" octect"));
          nomFichier = fichier.name();
        }
        fichier.close();

      } else break; // erreur de lecture
    }
    // si en sortie du while on n'a pas rempli le nom de fichier, c'est qu'il y a eu un soucis
    if (nomDuFichier[0] == '\0') Serial.println(F("erreur de lecture"));
  } else {
    Serial.println(F("index incorrect"));
  }
  dossier.close();
  return nomDuFichier;
}
