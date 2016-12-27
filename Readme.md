Kona
====

Short description: Experiments on image segmentation: identify text in digital images.

Intention, Überblick, Arbeitshypothesen
---------------------------------------

Mit "Kona" habe ich mich 2007 damit beschäftigt, Text in Rastergraphiken von Nichttext zu differenzieren. "Kona" verfolgt drei der in den Arbeiten http://www.stanford.edu/class/ee368b/Projects/mkalman/report.html und http://mehr.sharif.edu/~scientia/v13n4pdf/katebi.pdf beschriebenen Algorithmen, die - basierend auf der diskreten Kosinustransformation (DCT) von nicht überlappenden 8x8 Bildblöcken - bestimmte Eigenschaften von Text in Bildern mit Text- und Grafikverbund charakterisieren.

Die verfolgten Ansätze basieren dabei auf den folgenden Arbeitshypothesen:

I) Text zeichnet sich - in Abgrenzung zu Grafikmustern - aus durch eine höhere Energie in den Übergängen zwischen DCT-Blöcken (jeweils 8x8).
Beobachtbar war in den Experimenten mit "Kona", dass eine bestimmte Konstante, die bei den Berechnungen am häufigsten auftaucht und zudem bei allen zum Test betrachteten Bildern gleich ist, leere, d.h. weiße Flächen kennzeichnet.

Die Funktion bool TextSegment_DCT_energy(QImage *image) (KONAlib.cpp) fährt in 8x8 Pixelblöcken (nicht überlappend) durch das Bild, berechnet die DCT-Koeffizienten des jeweiligen Blocks und stellt schließlich den Mittelwert der Koeffizienten in Relation zu den drei umgebenden 8x8 Blöcken. Intendiert ist damit, die Aktivität / Energie des betrachteten Blocks im Verhältnis zu den direkt umgebenden Blöcken zu betrachten. Eine hohe Energie würde/könnte auf Text hindeuten, niedrige Energie auf Muster / Grafik.

Ia) Basierend auf der Annahme, dass eine bestimmte Konstante, die aus der DCT resultiert, Textinformation kennzeichnet, "trainiert" die Funktion bool TextSegment_DCT_energy_train(QImage *image) DCT-Konstanten auf Basis von Bildern mit ausschließlich Text. Für den Mercatorplan von Köln (https://arachne.dainst.org/entity/1222601) bedeutet das: Zunächst wird das Programm über einen Ausschnitt des Mercatorplans trainiert, der ausschließlich Text enthält. Anschließend müsste sich der Text-/Musterverbund in MERCATOR-PLAN_koeln_text_is.jpg über die Funktion bool TextSegment_DCT_energy_ontrained(QImage *image) mit den trainierten Koeffizienten segmentieren lassen - funktioniert allerdings leider nicht so, wie intendiert. :)

II) Die dritte Hypothese sieht die ungefähre Bitrate, die vonnöten ist, um den 8x8 Pixelblock mittels RLE zu codieren, als essentiell an: Text ist gekennzeichnet durch eine hohe Bitrate (völlig analog der Betrachtung hoher Kompressionsraten des jpg-Formates in Bildbereichen mit Text).

bool TextSegment_DCT_bitrate(QImage *image) berechnet somit auf Basis der Ausführungen unter http://www.stanford.edu/class/ee368b/Projects/mkalman/report.html die Bitrate des jeweiligen 8x8 DCT-Blocks.

Ergebnisse
----------

Die Verfahren I) und II) bieten interessante, aber nicht textisolierende Ergebnisse. Beiden eigen ist die Fähigkeit, leere Flächen im Bild zu finden und einzufärben, folglich Grafik und Text im Bild zu umranden.

Angesteuert werden die Verfahren im Programm über die Menupunkte
- "Vorverarbeitung" > "Text Segmentieren: DCT Energy"
- "Vorverarbeitung" > "Text Segmentieren: DCT Bitrate"
- "Vorverarbeitung" > "Text Segmentieren: DCT Energy Train" (Trainieren mit DCT-Koeffizienten eines reinen Textausschnittes des jeweiligen Bildes), anschließend mit "Vorverarbeitung" > "Text Segmentieren: DCT Energy Train (Trainiert)" das Bild mit den trainierten Koeffizienten bearbeiten.

Der Informationsgehalt des Eingabebildes muss bei allen Verfahren zunächst über "Vorverarbeitung" > "In s/w konvertieren" reduziert werden, da die o.a. Funktionen nur auf/mit 1-Bit Bildern arbeiten.
