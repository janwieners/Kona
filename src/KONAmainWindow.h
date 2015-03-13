#ifndef __KONAMAINWINDOW_H__
#define __KONAMAINWINDOW_H__

#include <QMainWindow>
#include <QtGui>
#include <QDialog>

#include "KONAlib.h"
#include "edge.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT 

public:
    MainWindow();

private slots:
    void loadImage(void);

	void conv2gray(void)
	{
         if (Convert2Grayscale(&image)==true)
            view->setPixmap(QPixmap::fromImage(image));
         else QMessageBox::warning(this,"Warnung",
              "Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
    }

	void conv2mono(void)
	{
		bool ok;
        int threshold;

        threshold=QInputDialog::getInteger(this,"Schwellenwert",
              "Bitte geben Sie den Schwellenwert an (0-255)",
              155,0,255,1,&ok);
        if (ok==true)
		{
			if (Convert2Mono(&image, threshold)==true)
				view->setPixmap(QPixmap::fromImage(image));
			else QMessageBox::warning(this,"Warnung",
				"Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
		 }
    }

	void startnextneighbours( void )
    {
		bool ok;
		int maxneighbours;

		maxneighbours=QInputDialog::getInteger(this,"Maximale Anzahl benachbarter Pixel",
			  "Bitte geben Sie die maximale Anzahl der benachbarten (3x3 Matrix) Pixel an (1-9)",
				  1,0,9,1,&ok);
			if (ok==true)
			{
				if (nextneighbours(&image, maxneighbours)==true)
					view->setPixmap( QPixmap::fromImage(image) );
				else QMessageBox::warning(this,"Warnung",
				  "Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
			 }
	}
	
	void negate(void)
	{
		if (TInegate(&image)==true)
            view->setPixmap(QPixmap::fromImage(image));
         else QMessageBox::warning(this,"Warnung",
              "Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
	}

	void segmenttext_DCT_energy(void)
	{
		if (TextSegment_DCT_energy(&image)==true)
            view->setPixmap(QPixmap::fromImage(image));
         else QMessageBox::warning(this,"Warnung",
              "Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
	}

	void segmenttext_DCT_bitrate(void)
	{
		if (TextSegment_DCT_bitrate(&image)==true)
            view->setPixmap(QPixmap::fromImage(image));
         else QMessageBox::warning(this,"Warnung",
              "Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
	}

	void segmenttext_DCT_energy_train(void)
	{
		if (TextSegment_DCT_energy_train(&image)==true)
            view->setPixmap(QPixmap::fromImage(image));
         else QMessageBox::warning(this,"Warnung",
              "Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
	}

	void segmenttext_DCT_energy_ontrained(void)
	{
		if (TextSegment_DCT_energy_ontrained(&image)==true)
            view->setPixmap(QPixmap::fromImage(image));
         else QMessageBox::warning(this,"Warnung",
              "Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
	}
         
         // *********************************
         // > Filter (Uebergabe enum): siehe edge.h/.cpp
         void Sobel()
         {
         	if ((image=Filter(image,FSobel)).isNull()==false)
            	view->setPixmap( QPixmap::fromImage(image) );
         	else QMessageBox::warning(this,"Warnung",
              	"Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
         }

		 void Sobel45()
         {
         	if ((image=Filter(image,FSobel45)).isNull()==false)
            	view->setPixmap( QPixmap::fromImage(image) );
         	else QMessageBox::warning(this,"Warnung",
              	"Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
         }

         // *******
         void Laplace1()
         {
         	if ((image=Filter(image,FLaplace1)).isNull()==false)
            	view->setPixmap( QPixmap::fromImage(image) );
         	else QMessageBox::warning(this,"Warnung",
              	"Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
         }
         
         void Laplace2()
         {
         	if ((image=Filter(image,FLaplace2)).isNull()==false)
            	view->setPixmap( QPixmap::fromImage(image) );
         	else QMessageBox::warning(this,"Warnung",
              	"Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
         }
         
         void Laplace3()
         {
         	if ((image=Filter(image,FLaplace3)).isNull()==false)
            	view->setPixmap( QPixmap::fromImage(image) );
         	else QMessageBox::warning(this,"Warnung",
              	"Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
         }
         // *******
         void Prewitt()
         {
         	if ((image=Filter(image,FPrewitt)).isNull()==false)
            	view->setPixmap( QPixmap::fromImage(image) );
         	else QMessageBox::warning(this,"Warnung",
              	"Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
         }
         // *******
         void Roberts()
         {
         	if ((image=Filter(image,FRoberts)).isNull()==false)
            	view->setPixmap( QPixmap::fromImage(image) );
         	else QMessageBox::warning(this,"Warnung",
              	"Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
         }
         // *******
         void WMean1()
         {
         	if ((image=Filter(image,FWMean1)).isNull()==false)
            	view->setPixmap( QPixmap::fromImage(image) );
         	else QMessageBox::warning(this,"Warnung",
              	"Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
         }
         
         void WMean2()
         {
         	if ((image=Filter(image,FWMean2)).isNull()==false)
            	view->setPixmap( QPixmap::fromImage(image) );
         	else QMessageBox::warning(this,"Warnung",
              	"Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
         }
         // *******
         void Highpass1()
         {
         	if ((image=Filter(image,FHighpass1)).isNull()==false)
            	view->setPixmap( QPixmap::fromImage(image) );
         	else QMessageBox::warning(this,"Warnung",
              	"Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
         }
         
         void Highpass2()
         {
         	if ((image=Filter(image,FHighpass2)).isNull()==false)
            	view->setPixmap( QPixmap::fromImage(image) );
         	else QMessageBox::warning(this,"Warnung",
              	"Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
         }
          // *******
         void Lowpass1()
         {
         	if ((image=Filter(image,FLowpass1)).isNull()==false)
            	view->setPixmap( QPixmap::fromImage(image) );
         	else QMessageBox::warning(this,"Warnung",
              	"Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
         }
         
         void Lowpass2()
         {
         	if ((image=Filter(image,FLowpass2)).isNull()==false)
            	view->setPixmap( QPixmap::fromImage(image) );
         	else QMessageBox::warning(this,"Warnung",
              	"Bild wegen falscher Farbtiefe nicht verarbeitbar", "Weiter");
         }
    

private:
    void createActions(void);  
    void createMenus(void);  
    void createStatusBar(void);  

private:
    QMenu          *fileMenu;
    QMenu          *testbed;
    QMenu          *edgeMenue;
   
    QAction        *loadAct;  
    QAction        *exitAct;

	QAction *conv2monoAct;
	QAction *conv2grayact;
	QAction *nextneighboursAct;
	QAction *negateAct;
	QAction *segmentText_DCT_Energy_Act;
	QAction *segmentText_DCT_Bitrate_Act;
	QAction *segmentText_DCT_Energy_Train_Act;
	QAction *segmentText_DCT_Energy_BasedonTrained_Act;

	// Filter Actor
    QAction *sobelAct;
	QAction *sobel45Act;
    QAction *laplace1Act;
    QAction *laplace2Act;
    QAction *laplace3Act;
    QAction *prewittAct;
    QAction *robertsAct;
    QAction *wmean1Act;
    QAction *wmean2Act;
    QAction *highpass1Act;
    QAction *highpass2Act;
    QAction *lowpass1Act;
    QAction *lowpass2Act;
  
    QScrollArea    *imageView;
    QLabel         *view;

    QLabel         *currentFile;

    QString         fileName;
    QImage          image;
};

#endif
