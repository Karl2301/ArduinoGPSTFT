void compterfichier() {
  nbFichiers = compteNbFichiers();
  nomFichier = affiFichier(noFichier);
  Serial.print(nbFichiers);  Serial.println(F(" fichiers à la racine"));
  Serial.print(nbFichiers); Serial.print(F("\t"));
  Serial.println(affiFichier(nbFichiers));
  Serial.println(F("\n+ = fichier suivant, - = fichier précédent"));
  noFichier = nbFichiers; // n° de fichier dans la liste, on part du dernier
}

void menueffafichplus() {
  if (++noFichier > nbFichiers) {
    noFichier--;
  }
  nomFichier = affiFichier(noFichier);  // affichage nom fichier indexé par noFichier > 0
  Serial.print(noFichier); Serial.print(F("\t"));
  Serial.println(nomFichier);
}

void menueffafichmoins() {
  if (--noFichier < 1) {
    noFichier = 1;
  }
  Serial.print(noFichier); Serial.print(F("\t"));
  nomFichier = affiFichier(noFichier);  // affichage nom fichier indexé par noFichier > 0
  Serial.println(nomFichier);
}
