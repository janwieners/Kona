Kona
====

Short description: Experiments on image segmentation: identify text in digital images.

Intention, �berblick, Arbeitshypothesen
---------------------------------------

Mit "Kona" habe ich mich 2007 damit besch�ftigt, Text in Rastergraphiken von Nichttext zu differenzieren. "Kona" verfolgt drei der in den Arbeiten http://www.stanford.edu/class/ee368b/Projects/mkalman/report.html und http://mehr.sharif.edu/~scientia/v13n4pdf/katebi.pdf beschriebenen Algorithmen, die - basierend auf der diskreten Kosinustransformation (DCT) von nicht �berlappenden 8x8 Bildbl�cken - bestimmte Eigenschaften von Text in Bildern mit Text- und Grafikverbund charakterisieren.

Die verfolgten Ans�tze basieren dabei auf den folgenden Arbeitshypothesen:

I) Text zeichnet sich - in Abgrenzung zu Grafikmustern - aus durch eine h�here Energie in den �berg�ngen zwischen DCT-Bl�cken (jeweils 8x8).
Beobachtbar war in den Experimenten mit "Kona", dass eine bestimmte Konstante, die bei den Berechnungen am h�ufigsten auftaucht und zudem bei allen zum Test betrachteten Bildern gleich ist, leere, d.h. wei�e Fl�chen kennzeichnet.

Die Funktion bool TextSegment_DCT_energy(QImage *image) (KONAlib.cpp) f�hrt in 8x8 Pixelbl�cken (nicht �berlappend) durch das Bild, berechnet die DCT-Koeffizienten des jeweiligen Blocks und stellt schlie�lich den Mittelwert der Koeffizienten in Relation zu den drei umgebenden 8x8 Bl�cken. Intendiert ist damit, die Aktivit�t / Energie des betrachteten Blocks im Verh�ltnis zu den direkt umgebenden Bl�cken zu betrachten. Eine hohe Energie w�rde/k�nnte auf Text hindeuten, niedrige Energie auf Muster / Grafik.

Ia) Basierend auf der Annahme, dass eine bestimmte Konstante, die aus der DCT resultiert, Textinformation kennzeichnet, "trainiert" die Funktion bool TextSegment_DCT_energy_train(QImage *image) DCT-Konstanten auf Basis von Bildern mit ausschlie�lich Text. F�r den Mercatorplan von K�ln (http://arachne.uni-koeln.de/drupal/?q=de/node/205) bedeutet das: Zun�chst wird das Programm �ber einen Ausschnitt des Mercatorplans trainiert, der ausschlie�lich Text enth�lt. Anschlie�end m�sste sich der Text-/Musterverbund in MERCATOR-PLAN_koeln_text_is.jpg �ber die Funktion bool TextSegment_DCT_energy_ontrained(QImage *image) mit den trainierten Koeffizienten segmentieren lassen - funktioniert allerdings leider nicht so, wie intendiert. :)

II) Die dritte Hypothese sieht die ungef�hre Bitrate, die vonn�ten ist, um den 8x8 Pixelblock mittels RLE zu codieren, als essentiell an: Text ist gekennzeichnet durch eine hohe Bitrate (v�llig analog der Betrachtung hoher Kompressionsraten des jpg-Formates in Bildbereichen mit Text).

bool TextSegment_DCT_bitrate(QImage *image) berechnet somit auf Basis der Ausf�hrungen unter http://www.stanford.edu/class/ee368b/Projects/mkalman/report.html die Bitrate des jeweiligen 8x8 DCT-Blocks.

Ergebnisse
----------

Die Verfahren I) und II) bieten interessante, aber nicht textisolierende Ergebnisse. Beiden eigen ist die F�higkeit, leere Fl�chen im Bild zu finden und einzuf�rben, folglich Grafik und Text im Bild zu umranden.

Angesteuert werden die Verfahren im Programm �ber die Menupunkte
- "Vorverarbeitung" > "Text Segmentieren: DCT Energy"
- "Vorverarbeitung" > "Text Segmentieren: DCT Bitrate"
- "Vorverarbeitung" > "Text Segmentieren: DCT Energy Train" (Trainieren mit DCT-Koeffizienten eines reinen Textausschnittes des jeweiligen Bildes), anschlie�end mit "Vorverarbeitung" > "Text Segmentieren: DCT Energy Train (Trainiert)" das Bild mit den trainierten Koeffizienten bearbeiten.

Der Informationsgehalt des Eingabebildes muss bei allen Verfahren zun�chst �ber "Vorverarbeitung" > "In s/w konvertieren" reduziert werden, da die o.a. Funktionen nur auf/mit 1-Bit Bildern arbeiten.