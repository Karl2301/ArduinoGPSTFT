float calcule_distance(float old_lat, float old_lon, float new_lat, float new_lon)
{
  float latRad, lonRad;
  float tlatRad, tlonRad;
  float midLat, midLon;
  float dist = 0.0;
  
  
  //convertion des valeures du degree vers le radian
  latRad = old_lat* 0.017453293;
  lonRad = old_lon* 0.017453293;
  tlatRad = new_lat * 0.017453293;
  tlonRad = new_lon * 0.017453293;

  midLat = tlatRad - latRad;
  midLon = tlonRad - lonRad;

  //Calcule de la distance en Km
  float latSin = sin((latRad - tlatRad)/2);
  float lonSin = sin((lonRad - tlonRad)/2);
  
  dist = 2 * asin(sqrt((latSin*latSin) + cos(latRad) * cos(tlatRad) * (lonSin * lonSin)));
  
  dist = dist * 6371; // pour la distance en Km il faut multiplier la valeure trouvée par le rayon de la terre
  
  return dist;
  
}
