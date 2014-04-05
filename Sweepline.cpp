/*SweeplineAlgorithmus
Vorgehen: 
1) Behandle Antennen und entferne sie
2) Sortiere die Liste nach y-Koordinate und f�r jede y-Koordinate auch nach ihrer x-Koordinate
3) Erstelle zwei leere Listen von integerwerden. Eine vorl�ufige Liste und eine aktuelle Liste 
4) int Area=0;
4) Erstelle einen Iterator it f�r die sortierte Outlineliste
5) Solange der Iterator nicht am Ende der Liste angekommen ist, tue folgendes 
	  1) y_alt=y-Koordinate des vorherigen Listenelements (wenn diese nicht leer ist)
	  Liste_vol�ufig= Liste_aktuell;
	   solange y-Koordinate von *it_n�chstes=y-Koordinate it* oder *it_vorheriges=it* tue folgendes:
	     ist *it schon in Liste_aktuell enthalten, so l�sche das Element aus Liste_aktuell. Ansonsten f�ge *it in Liste_aktuell ein.
	2) Ist y_alt!=y_it so berechne Rechtecksfl�che aus Differenz der y-Koordinaten und aus der Info der Liste_vorl�ufig. Ansonsten
	Bilde die Vereinigungsmenge M von List_vorl�ufig und Liste_aktuell;
	3) F�ge zur Variable Area den Inhalt M hinzu
	it++