#include <QFile>
#include <QTextStream>
#include <qimage.h>
#include <qvector.h>
#include <math.h>
#include <iostream>
#include <list>
#include <algorithm>

#include "KONAlib.h"
#include "edge.h"

using namespace std;

// Kein log2(x) in math.h ?
#define log2(x) (log (x) / M_LOG2_E)
#define M_LOG2_E 0.693147180559945309417

// Testweise Global: STL Liste mit trainierten DCT Energy Konstanten
list<int> dct_energy_trainlist;

// *****
// Reduzieren auf Graustufen
// *****
bool Convert2Grayscale(QImage *image)
{
	QRgb *RGB;
	for (int y=0; y<image->height(); ++y)
    	for (int x=0; x<image->width(); ++x)
    	{
    		RGB=(QRgb *)image->scanLine(y) + x;
			unsigned char gray = 0.299*qRed(*RGB) + 0.587*qGreen(*RGB) + 0.114*qBlue(*RGB);
			*RGB = qRgb(gray,gray,gray);
		}
	
	return true;
}

// *****
// Reduktion auf 1 monocolor (brute-force threshold Methode)
// Jan Wieners
// *****
bool Convert2Mono(QImage *image, int threshold)
{
	QRgb *RGB;
	unsigned char oldVal, newVal;

	// Brute Force Binarization; zur Texterkennung nicht geeignet.
	// ToDo: leistungsfähigeren Algorithmus implementieren, ie.
	// "Quantile Linear Algorithm for Robust Binarization [...]"
	// (page.mi.fu-berlin.de/block/ICDAR_Binarization_2007.pdf)
	for (int y=0;y<image->height();y++)
		for (int x=0;x<image->width();x++)
		{
			RGB=(QRgb *)image->scanLine(y) + x;
			oldVal=qRed(*RGB); // it's grayscale: equal color values
			if(oldVal>=threshold) 
				newVal=255;
			else 
				newVal=0;
	        *RGB = qRgb(newVal,newVal,newVal);
	    }
	
	*image=image->convertToFormat(QImage::Format_Indexed8);
   	    
	return true;
}

double c(int number)
{
	if(number == 0)
		return 1/sqrt((float)2);
	else
		return 1;
}

// *****
// DCT-Routine von
// http://www.thescripts.com/forum/thread472410.html
// Jan Wieners
// *****
void dct(int pic_in[8][8], int enc_out[8][8])
{
	double pi=3.14159265;

	// *****
	// DCT-Routine von
	// http://www.iee.et.tu-dresden.de/iee/hpsn/lv/procdesign/jpeg/sim/source.html
	// Liefert falsche Werte
	// *****
	//int u,v,x,y;
	//float u_cs, v_cs, Pi;
	//
	//Pi=3.1415927;

	//for ( u=0 ; u<8 ; u++ )
	//{
	//	for ( v=0 ; v<8 ; v++ )
	//	{
	//		for ( x=0 ; x<8 ; x++ )
	//		{
	//			for ( y=0 ; y<8 ; y++ )
	//			{
	//				if(u==0)
	//					u_cs=(1/(sqrt((float)2)));
	//				else
	//					u_cs=cos(((2*x+1)*u*Pi)/16);

	//				if(v==0)
	//					v_cs=(1/(sqrt((float)2)));
	//				else
	//					v_cs=cos(((2*y+1)*v*Pi)/16);

	//				enc_out[v][u]+=0.25*pic_in[y][x]*u_cs*v_cs;
	//			}
	//		 }
	//	}
	//}
	// *****
	for(int u=0;u<8;u++)
	{
		for(int v=0;v<8;v++)
		{
			double temp = 0.0;

			for(int x=0;x<8;x++)
			{
				for(int y=0;y<8;y++)
				{
					temp+=pic_in[x][y]*cos(pi*(2*x+1)*u/16)*cos(pi*(2*y+1)*v/16);
				}
			}
			enc_out[u][v]=temp*(c(u)/2)*(c(v)/2);
		}
	}
}

// *****
// Ansatz I: Text- von Bildinformation extrahieren über DCT Energy
// Jan Wieners
// *****
bool TextSegment_DCT_energy(QImage *image)
{
	int width, height, baseline;
	int candidate;

	width=image->width();
	height=image->height();

	// To remember: dct_energy_matrix ist Zeiger auf int-Zeiger
	int ** dct_energy_matrix;

	// Speicher reservieren für zweidimensionale DCT Energie Matrix
	dct_energy_matrix=(int **)malloc((height/8) * sizeof(int));

	// Navigation in Pixmap per 8x8 Matrix
	// Muster:
	// . - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	int Xoffset[64] = { 0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						};
	int Yoffset[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
						1, 1, 1, 1, 1, 1, 1, 1,
						2, 2, 2, 2, 2, 2, 2, 2,
						3, 3, 3, 3, 3, 3, 3, 3,
						4, 4, 4, 4, 4, 4, 4, 4,
						5, 5, 5, 5, 5, 5, 5, 5,
						6, 6, 6, 6, 6, 6, 6, 6,
						7, 7, 7, 7, 7, 7, 7, 7,
						};

	// Ausgabe der Pixelwerte in Txt File
	QFile file("pxvalues_raw.txt");
	file.open(QIODevice::WriteOnly);
	QTextStream out(&file);

	// DCT Energy Matrix generieren
	QFile file_dct_energy_matrix("dct_energy_matrix.txt");
	file_dct_energy_matrix.open(QIODevice::WriteOnly);
	QTextStream out_dct_energy_matrix(&file_dct_energy_matrix);

	// Eingabe- und Ausgabematrix (8x8 Block)
	int input8x8[8][8];
	int output8x8[8][8];

	int counta=0, countb=0;
	// Zaehlvariable Array DCT Energy Matrix
	int dct_energy_counta=0;
	int dct_energy_countb=0;

	// Navigation durch QImage über 8x8 Block (nicht ueberlappend)
	for (int y=1; y<height-8; y+=8)
	{
		dct_energy_matrix[dct_energy_counta]=(int *)malloc((width/8) * sizeof(int));
		for (int x=0; x<width; x+=8)
		{
			baseline=y;
			counta=0;
			countb=0;
			// 8x8 Block bearbeiten
			for (int i=0; i<64; i++)
			{
				candidate=*(image->scanLine(baseline + Yoffset[i]) + x + Xoffset[i]);

				input8x8[counta][countb]=candidate;
				countb++;

				if((i+1)%8==0) { counta++; countb=0; }
			}
			// DCT des Blockes berechnen
			dct(input8x8, output8x8);
			
			// Pixelwerte blockweise in Textdatei schreiben
			out << "Raw PX Data: \n";
			for(int count1=0; count1<8; count1++)
			{
				for(int count2=0; count2<8; count2++)
					out << input8x8[count1][count2] << " ";

				out << "\n";
			}
			
			// DCT Koeffizienten in Textdatei schreiben
			out << "\nDCT-Transformiert: \n";
			for(int count1=0; count1<8; count1++)
			{
				for(int count2=0; count2<8; count2++)
					out << output8x8[count1][count2] << " ";

				out << "\n";
			}

			// Berechnungen auf DCT-Koeffizienten
			// Beschreibung der Verfahren/Algorithmen unter
			// http://www.stanford.edu/class/ee368b/Projects/mkalman/report.html
			// und
			// http://dust.ess.uci.edu/~wangd/pub/classification_compound.pdf
			int dct_sum=0;
			int dct_mean=0;
			int dct_energy=0;

			// DCT Koeffizienten summieren
			for(int count1=0; count1<8; count1++)
				for(int count2=0; count2<8; count2++)
				{
					dct_sum+=output8x8[count1][count2];
				}

			dct_mean=dct_sum/64;
			dct_energy=(dct_sum*dct_sum)/64;

			//out << "\n\nKoeffizienten Mittelwert: \n";
			//out << dct_mean << "\n";
			//out << "\n\nKoeffizienten Energie: \n";
			//out << dct_energy << "\n";

			// DCT Energy Matrix in RAM schreiben
			dct_energy_matrix[dct_energy_counta][dct_energy_countb]=dct_energy;

			dct_energy_countb++;
		}
		dct_energy_counta++;
		dct_energy_countb=0;
	}
	
	// DCT Energy Matrix in TXT-Datei schreiben
	out_dct_energy_matrix << "DCT-Energy Matrix: \n\n";
	for(int count1=0; count1<(height/8)-1; count1++)
	{
		for(int count2=0; count2<(width/8)-1; count2++)
			out_dct_energy_matrix << dct_energy_matrix[count1][count2] << " ";

		out_dct_energy_matrix << "\n\n";
	}

	// *****
	// Der spannende Teil: Bildinformation segmentieren
	// Arbeitshypothese I: Text zeichnet sich durch höhere Energie in den Übergängen zwischen
	// DCT-Blöcken aus
	
	// Navigation in DCT Energy Matrix per
	// . -
	// - -
	int dct_energy_Xoffset[4]={ 0, 1, 0, 1 };
	int dct_energy_Yoffset[4]={ 0, 0, 1, 1 };
	int dct_energy_transition_sum=0;
	int actual_x=0;
	int actual_y=0;

	for(int count1=0; count1<(height/8)-1; count1++)
	{
		for(int count2=0; count2<(width/8)-1; count2++)
		{
			// Energie im 2x2 Block mitteln
			for(int count=0; count<2; count++)
				dct_energy_transition_sum+=(dct_energy_matrix[count1+dct_energy_Yoffset[count]][count2+dct_energy_Xoffset[count]]);

			// Mittelwert 2x2 DCT
			dct_energy_transition_sum/=4;
			
			out_dct_energy_matrix << "\n" << dct_energy_transition_sum;

			// Unterscheidung Text- und Grafikinformation
			// Interessant: In allen Testbildern steht Wert 23112 für freie Flächen
			// Grundfrage: Welche Konstanten kennzeichnen Text, welche Grafik?
			if(dct_energy_transition_sum==23112 || dct_energy_transition_sum==23085)
			{
				for (int i=0; i<64; i++)
				{
					*(image->scanLine(actual_y*8 + Yoffset[i]) + actual_x*8 + Xoffset[i])=0;
				}
			}
			actual_x++;
			dct_energy_transition_sum=0;
		}
		actual_x=0;
		actual_y++;
	}

	// Speicher freigeben
	// Funktioniert nicht - warum? (ToDo)
	//for(int i=0; i<dct_energy_counta; i++)
	//	free(dct_energy_matrix[i]);
	//free(dct_energy_matrix);

	// Ausgabedatei pxvalues_raw.txt schließen
	file.close();
	// Ausgabedatei dct_energy_matrix.txt schließen
	file_dct_energy_matrix.close();

	postprocess(image);

	return true;
}

// *****
// Ansatz I a): Text ist in Bild gekennzeichnet durch Konstanten der DCT Energy Fkt.
// Folglich: Konstanten herausfinden durch Training; Input=Bilder mit ausschließlich Text,
// -> Aufbau einer Konstantentabelle / Lookup-Table.
// Jan Wieners
bool TextSegment_DCT_energy_train(QImage *image)
{
	int width, height, baseline;
	int candidate;

	width=image->width();
	height=image->height();

	// To remember: dct_energy_matrix ist Zeiger auf int-Zeiger
	int ** dct_energy_matrix;

	// Speicher reservieren für zweidimensionale DCT Energie Matrix
	dct_energy_matrix=(int **)malloc((height/8) * sizeof(int));

	int Xoffset[64] = { 0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						};
	int Yoffset[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
						1, 1, 1, 1, 1, 1, 1, 1,
						2, 2, 2, 2, 2, 2, 2, 2,
						3, 3, 3, 3, 3, 3, 3, 3,
						4, 4, 4, 4, 4, 4, 4, 4,
						5, 5, 5, 5, 5, 5, 5, 5,
						6, 6, 6, 6, 6, 6, 6, 6,
						7, 7, 7, 7, 7, 7, 7, 7,
						};

	// Eingabe- und Ausgabematrix (8x8 Block)
	int input8x8[8][8];
	int output8x8[8][8];

	int counta=0, countb=0;
	// Zaehlvariable Array DCT Energy Matrix
	int dct_energy_counta=0;
	int dct_energy_countb=0;

	// Navigation durch QImage über 8x8 Block (nicht ueberlappend)
	for (int y=1; y<height-8; y+=8)
	{
		dct_energy_matrix[dct_energy_counta]=(int *)malloc((width/8) * sizeof(int));
		for (int x=0; x<width; x+=8)
		{
			baseline=y;
			counta=0;
			countb=0;
			// 8x8 Block bearbeiten
			for (int i=0; i<64; i++)
			{
				candidate=*(image->scanLine(baseline + Yoffset[i]) + x + Xoffset[i]);

				input8x8[counta][countb]=candidate;
				countb++;

				if((i+1)%8==0) { counta++; countb=0; }
			}
			// DCT des Blockes berechnen
			dct(input8x8, output8x8);
			
			int dct_sum=0;
			int dct_mean=0;
			int dct_energy=0;

			// DCT Koeffizienten summieren
			for(int count1=0; count1<8; count1++)
				for(int count2=0; count2<8; count2++)
				{
					dct_sum+=output8x8[count1][count2];
				}

			dct_mean=dct_sum/64;
			dct_energy=(dct_sum*dct_sum)/64;

			// DCT Energy Matrix in RAM schreiben
			dct_energy_matrix[dct_energy_counta][dct_energy_countb]=dct_energy;

			dct_energy_countb++;
		}
		dct_energy_counta++;
		dct_energy_countb=0;
	}
	
	int dct_energy_Xoffset[4]={ 0, 1, 0, 1 };
	int dct_energy_Yoffset[4]={ 0, 0, 1, 1 };
	int dct_energy_transition_sum=0;
	int actual_x=0;
	int actual_y=0;

	for(int count1=0; count1<(height/8)-1; count1++)
	{
		for(int count2=0; count2<(width/8)-1; count2++)
		{
			// Energie im 2x2 Block mitteln
			for(int count=0; count<2; count++)
				dct_energy_transition_sum+=(dct_energy_matrix[count1+dct_energy_Yoffset[count]][count2+dct_energy_Xoffset[count]]);

			// Mittelwert 2x2 DCT
			dct_energy_transition_sum/=4;

			dct_energy_trainlist.push_back(dct_energy_transition_sum);
			
			if(dct_energy_transition_sum==23112)
			{
				for (int i=0; i<64; i++)
				{
					*(image->scanLine(actual_y*8 + Yoffset[i]) + actual_x*8 + Xoffset[i])=255;
				}
			}
			actual_x++;
			dct_energy_transition_sum=0;
		}
		actual_x=0;
		actual_y++;
	}

	// Speicher freigeben
	// Funktioniert nicht - warum? (ToDo)
	//for(int i=0; i<dct_energy_counta; i++)
	//	free(dct_energy_matrix[i]);
	//free(dct_energy_matrix);

	return true;
}

// *****
// Ansatz I a)
// Segmentierung über trainierte DCT Energy Konstanten
// Jan Wieners
// *****
bool TextSegment_DCT_energy_ontrained(QImage *image)
{
	int width, height, baseline;
	int candidate;

	width=image->width();
	height=image->height();

	// To remember: dct_energy_matrix ist Zeiger auf int-Zeiger
	int ** dct_energy_matrix;

	// Speicher reservieren für zweidimensionale DCT Energie Matrix
	dct_energy_matrix=(int **)malloc((height/8) * sizeof(int));

	// Navigation in Pixmap per 8x8 Matrix
	// Muster:
	// . - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	int Xoffset[64] = { 0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						};
	int Yoffset[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
						1, 1, 1, 1, 1, 1, 1, 1,
						2, 2, 2, 2, 2, 2, 2, 2,
						3, 3, 3, 3, 3, 3, 3, 3,
						4, 4, 4, 4, 4, 4, 4, 4,
						5, 5, 5, 5, 5, 5, 5, 5,
						6, 6, 6, 6, 6, 6, 6, 6,
						7, 7, 7, 7, 7, 7, 7, 7,
						};

	// Eingabe- und Ausgabematrix (8x8 Block)
	int input8x8[8][8];
	int output8x8[8][8];

	int counta=0, countb=0;
	// Zaehlvariable Array DCT Energy Matrix
	int dct_energy_counta=0;
	int dct_energy_countb=0;

	// Navigation durch QImage über 8x8 Block (nicht ueberlappend)
	for (int y=1; y<height-8; y+=8)
	{
		dct_energy_matrix[dct_energy_counta]=(int *)malloc((width/8) * sizeof(int));
		for (int x=0; x<width; x+=8)
		{
			baseline=y;
			counta=0;
			countb=0;
			// 8x8 Block bearbeiten
			for (int i=0; i<64; i++)
			{
				candidate=*(image->scanLine(baseline + Yoffset[i]) + x + Xoffset[i]);

				input8x8[counta][countb]=candidate;
				countb++;

				if((i+1)%8==0) { counta++; countb=0; }
			}
			// DCT des Blockes berechnen
			dct(input8x8, output8x8);
			
			int dct_sum=0;
			int dct_mean=0;
			int dct_energy=0;

			// DCT Koeffizienten summieren
			for(int count1=0; count1<8; count1++)
				for(int count2=0; count2<8; count2++)
				{
					dct_sum+=output8x8[count1][count2];
				}

			dct_mean=dct_sum/64;
			dct_energy=(dct_sum*dct_sum)/64;

			// DCT Energy Matrix in RAM schreiben
			dct_energy_matrix[dct_energy_counta][dct_energy_countb]=dct_energy;

			dct_energy_countb++;
		}
		dct_energy_counta++;
		dct_energy_countb=0;
	}
	
	int dct_energy_Xoffset[4]={ 0, 1, 0, 1 };
	int dct_energy_Yoffset[4]={ 0, 0, 1, 1 };
	int dct_energy_transition_sum=0;
	int actual_x=0;
	int actual_y=0;

	for(int count1=0; count1<(height/8)-1; count1++)
	{
		for(int count2=0; count2<(width/8)-1; count2++)
		{
			// Energie im 2x2 Block mitteln
			for(int count=0; count<2; count++)
				dct_energy_transition_sum+=(dct_energy_matrix[count1+dct_energy_Yoffset[count]][count2+dct_energy_Xoffset[count]]);

			// Mittelwert 2x2 DCT
			dct_energy_transition_sum/=4;
			
			// Unterscheidung Text- und Grafikinformation
			if(std::find(dct_energy_trainlist.begin(), dct_energy_trainlist.end(), dct_energy_transition_sum) != dct_energy_trainlist.end())
			{
				for (int i=0; i<64; i++)
				{
					*(image->scanLine(actual_y*8 + Yoffset[i]) + actual_x*8 + Xoffset[i])=0;
				}
			}
			else
				for (int i=0; i<64; i++)
				{
					*(image->scanLine(actual_y*8 + Yoffset[i]) + actual_x*8 + Xoffset[i])=255;
				}
			actual_x++;
			dct_energy_transition_sum=0;
		}
		actual_x=0;
		actual_y++;

		QFile file("dct_energy_trained.txt");
		file.open(QIODevice::WriteOnly);
		QTextStream out(&file);

		 list<int>::iterator iter;

		 for (iter=dct_energy_trainlist.begin(); iter != dct_energy_trainlist.end(); ++iter)
			out << *iter << " ";
	}
	return true;
}


// *****
// Ansatz II: Text extrahieren über Charakteristika der DCT Bitrate
// Jan Wieners
bool TextSegment_DCT_bitrate(QImage *image)
{
	int width, height, baseline;
	int candidate;

	postprocess(image);

	width=image->width();
	height=image->height();

	// To remember: dct_energy_matrix ist Zeiger auf int-Zeiger
	int ** dct_bitrate_matrix;
	// Speicher reservieren für zweidimensionale DCT Bitrate Matrix
	dct_bitrate_matrix=(int **)malloc((height/8) * sizeof(int));

	// Navigation in Pixmap per 8x8 Matrix
	// Muster:
	// . - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	// - - - - - - - -
	int Xoffset[64] = { 0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						0, 1, 2, 3, 4, 5, 6, 7,
						};
	int Yoffset[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
						1, 1, 1, 1, 1, 1, 1, 1,
						2, 2, 2, 2, 2, 2, 2, 2,
						3, 3, 3, 3, 3, 3, 3, 3,
						4, 4, 4, 4, 4, 4, 4, 4,
						5, 5, 5, 5, 5, 5, 5, 5,
						6, 6, 6, 6, 6, 6, 6, 6,
						7, 7, 7, 7, 7, 7, 7, 7,
						};

	// Ausgabe der Pixelwerte in Txt File
	QFile file("pxvalues_raw.txt");
	file.open(QIODevice::WriteOnly);
	QTextStream out(&file);

	// DCT Energy Matrix generieren
	QFile file_dct_bitrate_matrix("dct_bitrate_matrix.txt");
	file_dct_bitrate_matrix.open(QIODevice::WriteOnly);
	QTextStream out_dct_bitrate_matrix(&file_dct_bitrate_matrix);

	// Eingabe- und Ausgabematrix (8x8 Block)
	int input8x8[8][8];
	int output8x8[8][8];

	int counta=0, countb=0;
	// Zaehlvariable Array DCT Bitrate Matrix
	int dct_energy_counta=0;
	int dct_energy_countb=0;

	// Navigation durch QImage über 8x8 Block (nicht ueberlappend)
	for (int y=1; y<height-8; y+=8)
	{
		dct_bitrate_matrix[dct_energy_counta]=(int *)malloc((width/8) * sizeof(int));
		for (int x=0; x<width; x+=8)
		{
			baseline=y;
			counta=0;
			countb=0;
			// 8x8 Block bearbeiten
			for (int i=0; i<64; i++)
			{
				candidate=*(image->scanLine(baseline + Yoffset[i]) + x + Xoffset[i]);

				input8x8[counta][countb]=candidate;
				countb++;

				if((i+1)%8==0) { counta++; countb=0; }
			}
			
			// DCT des Blockes berechnen
			dct(input8x8, output8x8);
			
			// Pixelwerte blockweise in Textdatei schreiben
			out << "Raw PX Data: \n";
			for(int count1=0; count1<8; count1++)
			{
				for(int count2=0; count2<8; count2++)
					out << input8x8[count1][count2] << " ";

				out << "\n";
			}
			
			// DCT Koeffizienten in Textdatei schreiben
			out << "\nDCT-Transformiert: \n";
			for(int count1=0; count1<8; count1++)
			{
				for(int count2=0; count2<8; count2++)
					out << output8x8[count1][count2] << " ";

				out << "\n";
			}
			// dct_bitrate: ungefähre Bitlänge, die es benötigen würde, den DCT-Block per RLE zu komprimieren
			// nach http://www.stanford.edu/class/ee368b/Projects/mkalman/report.html
			int dct_bitrate_sum=0;

			// DCT Koeffizienten summieren
			for(int count1=0; count1<8; count1++)
				for(int count2=0; count2<8; count2++)
				{
					if(output8x8[count1][count2]>0)
						dct_bitrate_sum+=log2((double)output8x8[count1][count2])+4;
					else if(output8x8[count1][count2]<0)
						dct_bitrate_sum+=log2((double)(output8x8[count1][count2])*-1)+4;
				}
			out << "\nBitrate: \n";
			out << dct_bitrate_sum << "\n";

			// DCT Bitrate Matrix in RAM schreiben
			dct_bitrate_matrix[dct_energy_counta][dct_energy_countb]=dct_bitrate_sum;

			dct_energy_countb++;
		}
		dct_energy_counta++;
		dct_energy_countb=0;
	}
	
	// DCT Bitrate Matrix in TXT-Datei schreiben
	out_dct_bitrate_matrix << "DCT-Bitrate Matrix: \n\n";
	for(int count1=0; count1<(height/8)-1; count1++)
	{
		for(int count2=0; count2<(width/8)-1; count2++)
			out_dct_bitrate_matrix << dct_bitrate_matrix[count1][count2] << " ";

		out_dct_bitrate_matrix << "\n\n";
	}

	// *****
	// Arbeitshypothese II: Text ist aufwändiger zu komprimieren (höhere Bitrate bei RLE)
	int dct_current_bitrate=0;
	int actual_x=0;
	int actual_y=0;

	// Höhere Bitraten mit kleineren Blocks ersetzen -> Varianz (f. Iteration) generieren
	int Xoffset_small[4] = { 0, 1, 0, 1 };
	int Yoffset_small[4] = { 0, 0, 1, 1 };
	int Xoffset_medium[9] = { 0, 1, 2, 0, 1, 2, 0, 1, 2 };
	int Yoffset_medium[9] = { 0, 0, 0, 1, 1, 1, 2, 2, 2 };

	for(int count1=0; count1<(height/8)-1; count1++)
	{
		for(int count2=0; count2<(width/8)-1; count2++)
		{
			dct_current_bitrate=(dct_bitrate_matrix[count1][count2]);

			// Niedrige Bitraten == Redundant == Große Blöcke
			if(dct_current_bitrate<=300)
			{
				for (int i=0; i<64; i++)
				{
					*(image->scanLine(actual_y*8 + Yoffset[i]) + actual_x*8 + Xoffset[i])=0;
				}
			}
			else if(dct_current_bitrate<=400)
			{
				for (int i=0; i<9; i++)
				{
					*(image->scanLine(actual_y*8 + Yoffset_medium[i]) + actual_x*8 + Xoffset_medium[i])=0;
				}
			}
			else if(dct_current_bitrate<=450)
			{
				for (int i=0; i<4; i++)
				{
					*(image->scanLine(actual_y*8 + Yoffset_small[i]) + actual_x*8 + Xoffset_small[i])=0;
				}
			}
			else if(dct_current_bitrate>=600)
			{
				for (int i=0; i<64; i++)
				{
					*(image->scanLine(actual_y*8 + Yoffset[i]) + actual_x*8 + Xoffset[i])=0;
				}
			}
			actual_x++;
			dct_current_bitrate=0;
		}
		actual_x=0;
		actual_y++;
	}

	// Speicher freigeben
	// Funktioniert nicht - warum? (ToDo)
	//for(int i=0; i<dct_energy_counta; i++)
	//	free(dct_energy_matrix[i]);
	//free(dct_energy_matrix);

	// Ausgabedatei pxvalues_raw.txt schließen
	file.close();
	// Ausgabedatei dct_energy_matrix.txt schließen
	file_dct_bitrate_matrix.close();

	postprocess(image);

	return true;
}

// *****
// Postprocessing / Bildinformation nachbearbeiten
// Jan Wieners
// *****
void postprocess(QImage *image)
{
	int width, height, baseline;
	int candidate;

	width=image->width();
	height=image->height();

	// Post Processing, basierend auf Mustern unter
	// http://www.mhaji.com/ml2/Project2.html
	// a) 0 0 0    0 0 0
	//    0 1 0 => 0 0 0
	//    0 0 0    0 0 0
	//
	// b) 1 1 1    1 1 1
	//    1 0 1 => 1 1 1
	//    1 1 1    1 1 1
	//
	// c) 0 1   => 1 1
	//    1 1      1 1
	//
	// d) 1 0   => 1 1
	//    1 1      1 1
	//
	// e) 1 1   => 1 1
	//    0 1      1 1
	//
	// f) 1 1   => 1 1
	//    1 0      1 1
	int neighbourcount=0;
	
	// Navigation in Pixmap
	// Muster:
	// - - -
	// - . -
	// - - -
	int Xmuster_offset[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1};
	int Ymuster_offset[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1};

	width=image->width();

	// Musterverarbeitung freilich inperformant, da zu viele Neuaufrufe...
	// a)
	for (int y=1; y<image->height()-1; y++)
		for (int x=0; x<image->width(); x++)
		{
			baseline=y;
			// Pixmap über offsets abgrasen: Weiße Pixel auf Nachbarpixel prüfen
			if(*(image->scanLine(y) + x)==255)
			for (int i=0; i<9; i++)
			{
				candidate=*(image->scanLine(baseline + Ymuster_offset[i]) + x + Xmuster_offset[i]);
				// Weißer Pixel gefunden
				if(candidate==255)
					neighbourcount++;
			}
		// maxneighbours -> Anzahl der Nachbarn konstituiert Pixelwert
		if(neighbourcount==1)
			*(image->scanLine(y) + x)=0;
		neighbourcount=0;
		}

	// b)
	for (int y=1; y<image->height()-1; y++)
		for (int x=0; x<image->width(); x++)
		{
			baseline=y;
			// Pixmap über offsets abgrasen: Schwarze Pixel auf Nachbarpixel prüfen
			if(*(image->scanLine(y) + x)==0)
			for (int i=0; i<9; i++)
			{
				candidate=*(image->scanLine(baseline + Ymuster_offset[i]) + x + Xmuster_offset[i]);
				// Schwarzer Pixel gefunden
				if(candidate==0)
					neighbourcount++;
			}
		// maxneighbours -> Anzahl der Nachbarn konstituiert Pixelwert
		if(neighbourcount==1)
			*(image->scanLine(y) + x)=255;
		neighbourcount=0;
		}
}

// *****
// nextneighbours
// -> erodieren
// Jan Wieners
// *****
bool nextneighbours(QImage *image, unsigned int maxneighbours)
{
	unsigned int width, baseline, neighbourcount=0, candidate;
	
	// Navigation in Pixmap
	// Muster:
	// - - -
	// - . -
	// - - -
	// Schwarze Pixel ohne (schwarze) Nachbarn durch weiße ersetzen.
	int Xoffset[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1};
	int Yoffset[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1};

	width=image->width();

	for (int y=1; y<image->height()-1; y++)
		for (int x=0; x<image->width(); x++)
		{
			baseline=y;
			// Pixmap über offsets abgrasen: Schwarze Pixel auf Nachbarpixel prüfen
			if(*(image->scanLine(y) + x)==0)
			for (int i=0; i<9; i++)
			{
				candidate=*(image->scanLine(baseline + Yoffset[i]) + x + Xoffset[i]);
				// Schwarzer Pixel gefunden
				if(candidate==0)
					neighbourcount++;
			}
		// maxneighbours -> Anzahl der Nachbarn konstituiert Pixelwert
		if(neighbourcount==maxneighbours)
			*(image->scanLine(y) + x)=255;
		neighbourcount=0;
		}
				
	return true;
}	

// Bildinformation / Farbwerte invertieren
bool TInegate(QImage *image)
{
	unsigned char oldVal, newVal;
	unsigned int oldRed, oldGreen, oldBlue, newRed, newGreen, newBlue;
	QRgb *RGB;
	
	if (image->depth() == 8)
	   {
	   for (int y=0;y<image->height();y++)
		   for (int x=0;x<image->width();x++)
			   {
			   oldVal = *(image->scanLine(y) + x);
			   newVal=255-oldVal;
			   *(image->scanLine(y) + x) = newVal;
			   }
	   return true;
	   }

	if (image->depth() == 32)
	   {
	   for (int y=0;y<image->height();y++)
		   for (int x=0;x<image->width();x++)
			   {
			   RGB=(QRgb *)image->scanLine(y) + x;
			   oldRed = qRed(*RGB);
			   newRed=255-oldRed;
			   oldGreen = qGreen(*RGB);
			   newGreen=255-oldGreen;
			   oldBlue = qBlue(*RGB);
			   newBlue=255-oldBlue;
			   *RGB = qRgb(newRed,newGreen,newBlue);
			   }
	   return true;
	   }
	
	return false;
}
