/*SweeplineAlgorithmus
Vorgehen: 
1) Behandle Antennen und entferne sie
2) Sortiere die Liste nach y-Koordinate und für jede y-Koordinate auch nach ihrer x-Koordinate
3) Erstelle zwei leere Listen von integerwerden. Eine vorläufige Liste und eine aktuelle Liste 
4) int Area=0;
4) Erstelle einen Iterator it für die sortierte Outlineliste
5) Solange der Iterator nicht am Ende der Liste angekommen ist, tue folgendes 
	  1) y_alt=y-Koordinate des vorherigen Listenelements (wenn diese nicht leer ist)
	  Liste_voläufig= Liste_aktuell;
	   solange y-Koordinate von *it_nächstes=y-Koordinate it* oder *it_vorheriges=it* tue folgendes:
	     ist *it schon in Liste_aktuell enthalten, so lösche das Element aus Liste_aktuell. Ansonsten füge *it in Liste_aktuell ein.
	2) Ist y_alt!=y_it so berechne Rechtecksfläche aus Differenz der y-Koordinaten und aus der Info der Liste_vorläufig. Ansonsten
	Bilde die Vereinigungsmenge M von List_vorläufig und Liste_aktuell;
	3) Füge zur Variable Area den Inhalt M hinzu
	it++