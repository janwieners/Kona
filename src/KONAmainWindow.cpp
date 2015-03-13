#include <QtGui>

 
#include "KONAmainWindow.h"

MainWindow::MainWindow()
{ 
	view = new QLabel;
	imageView = new QScrollArea(this);
	imageView->setBackgroundRole( QPalette::Light );
	imageView->setWidget(view);
	
	setCentralWidget(imageView);   
	createActions();  
	createMenus();   
	createStatusBar();
} 

//################################################################



void MainWindow::createActions()
{
	loadAct = new QAction(tr("&Laden"), this);
	loadAct->setShortcut(tr("Ctrl+N"));
	connect(loadAct, SIGNAL(triggered()), this, SLOT(loadImage()));
	
	exitAct = new QAction(tr("&Beenden"), this); 
	exitAct->setShortcut(tr("Ctrl+B")); 
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
	
	//================================================ actions
	conv2grayact=new QAction(tr("Graustufen"), this);
	conv2grayact->setShortcut(tr("Ctrl+G"));
	connect(conv2grayact, SIGNAL(triggered()), this, SLOT(conv2gray()));

	conv2monoAct=new QAction(tr("In s/w konvertieren"), this);
	conv2monoAct->setShortcut(tr("Ctrl+C"));
	connect(conv2monoAct, SIGNAL(triggered()), this, SLOT(conv2mono()));
	
	nextneighboursAct=new QAction(tr("Next Neighbours"), this);
	nextneighboursAct->setShortcut(tr("Ctrl+I"));
	connect(nextneighboursAct, SIGNAL(triggered()), this, SLOT(startnextneighbours()));
	
	negateAct=new QAction(tr("Negieren"), this);
	negateAct->setShortcut(tr("Ctrl+N"));
	connect(negateAct, SIGNAL(triggered()), this, SLOT(negate()));

	segmentText_DCT_Energy_Act=new QAction(tr("Text Segmentieren: DCT Energy (Wieners)"), this);
	segmentText_DCT_Energy_Act->setShortcut(tr("Ctrl+E"));
	connect(segmentText_DCT_Energy_Act, SIGNAL(triggered()), this, SLOT(segmenttext_DCT_energy()));

	segmentText_DCT_Bitrate_Act=new QAction(tr("Text Segmentieren: DCT Bitrate (Wieners)"), this);
	segmentText_DCT_Bitrate_Act->setShortcut(tr("Ctrl+B"));
	connect(segmentText_DCT_Bitrate_Act, SIGNAL(triggered()), this, SLOT(segmenttext_DCT_bitrate()));

	segmentText_DCT_Energy_Train_Act=new QAction(tr("Text Segmentieren: DCT Energy Train (Wieners)"), this);
	segmentText_DCT_Energy_Train_Act->setShortcut(tr("Ctrl+T"));
	connect(segmentText_DCT_Energy_Train_Act, SIGNAL(triggered()), this, SLOT(segmenttext_DCT_energy_train()));

	segmentText_DCT_Energy_BasedonTrained_Act=new QAction(tr("Text Segmentieren: DCT Energy Train (Trainiert) (Wieners)"), this);
	segmentText_DCT_Energy_BasedonTrained_Act->setShortcut(tr("Ctrl+O"));
	connect(segmentText_DCT_Energy_BasedonTrained_Act, SIGNAL(triggered()), this, SLOT(segmenttext_DCT_energy_ontrained()));
	
	// **********************************
	// Filter
	sobelAct = new QAction(tr("Sobel"), this);
	connect(sobelAct, SIGNAL(triggered()), this, SLOT(Sobel()));
	// **
	sobel45Act = new QAction(tr("Sobel 45°"), this);
	connect(sobel45Act, SIGNAL(triggered()), this, SLOT(Sobel45()));

	laplace1Act = new QAction(tr("Laplace 1"), this);
	connect(laplace1Act, SIGNAL(triggered()), this, SLOT(Laplace1()));
	
	laplace2Act = new QAction(tr("Laplace 2"), this);
	connect(laplace2Act, SIGNAL(triggered()), this, SLOT(Laplace2()));
	
	laplace3Act = new QAction(tr("Laplace 3"), this);
	connect(laplace3Act, SIGNAL(triggered()), this, SLOT(Laplace3()));
	// **
	prewittAct = new QAction(tr("Prewitt"), this);
	connect(prewittAct, SIGNAL(triggered()), this, SLOT(Prewitt()));
	// **
	robertsAct = new QAction(tr("Roberts"), this);
	connect(robertsAct, SIGNAL(triggered()), this, SLOT(Roberts()));
	// **
	wmean1Act = new QAction(tr("WMean 1"), this);
	connect(wmean1Act, SIGNAL(triggered()), this, SLOT(WMean1()));

	wmean2Act = new QAction(tr("WMean 2"), this);
	connect(wmean2Act, SIGNAL(triggered()), this, SLOT(WMean2()));
	// **
	highpass1Act = new QAction(tr("Highpass 1"), this);
	connect(highpass1Act, SIGNAL(triggered()), this, SLOT(Highpass1()));
	
	highpass2Act = new QAction(tr("Highpass 2"), this);
	connect(highpass2Act, SIGNAL(triggered()), this, SLOT(Highpass2()));
	// **
	lowpass1Act = new QAction(tr("Lowpass 1"), this);
	connect(lowpass1Act, SIGNAL(triggered()), this, SLOT(Lowpass1()));
	
	lowpass2Act = new QAction(tr("Lowpass 2"), this);
	connect(lowpass2Act, SIGNAL(triggered()), this, SLOT(Lowpass2()));
} 

void MainWindow::createMenus()   
{
	fileMenu = menuBar()->addMenu(tr("&Datei"));
	fileMenu->addAction(loadAct);
	fileMenu->addSeparator();  
	fileMenu->addAction(exitAct);
	
	testbed=menuBar()->addMenu(tr("Vorverarbeitung"));
	testbed->addAction(conv2grayact);
	testbed->addAction(conv2monoAct);
	testbed->addAction(nextneighboursAct);
	testbed->addAction(negateAct);
	testbed->addSeparator();
	testbed->addAction(segmentText_DCT_Energy_Act);
	testbed->addAction(segmentText_DCT_Bitrate_Act);
	testbed->addSeparator();
	testbed->addAction(segmentText_DCT_Energy_Train_Act);
	testbed->addAction(segmentText_DCT_Energy_BasedonTrained_Act);
	
	// Filter
	edgeMenue=menuBar()->addMenu(tr("Kanten finden"));
	edgeMenue->addAction(sobel45Act);
	edgeMenue->addSeparator();
	edgeMenue->addAction(sobelAct);
	edgeMenue->addSeparator();
	edgeMenue->addAction(laplace1Act);
	edgeMenue->addAction(laplace2Act);
	edgeMenue->addAction(laplace3Act);
	edgeMenue->addSeparator(); 
	edgeMenue->addAction(prewittAct);
	edgeMenue->addSeparator();
	edgeMenue->addAction(robertsAct);
	edgeMenue->addSeparator();
	edgeMenue->addAction(wmean1Act);
	edgeMenue->addAction(wmean2Act);
	edgeMenue->addSeparator();
	edgeMenue->addAction(highpass1Act);
	edgeMenue->addAction(highpass2Act);
	edgeMenue->addSeparator();
	edgeMenue->addAction(lowpass1Act);
	edgeMenue->addAction(lowpass2Act);
}

void MainWindow::createStatusBar()
{
	QStatusBar *statusBar;
	
	statusBar = new QStatusBar; 
	QLabel *l = new QLabel("Datei: ");
	statusBar->addPermanentWidget(l);
	currentFile = new QLabel(" ");
	statusBar->addPermanentWidget(currentFile);
	
	setStatusBar(statusBar);
} 

void MainWindow::loadImage(void)
{
	QPixmap pixmap;
		
	fileName = QFileDialog::getOpenFileName(this,
	                         "Wählen Sie ein Bild",
	                         ".",
	                         "Images (*.png *.xpm *.jpg)");
	
	if (fileName.isEmpty() == true)
	   {
	   QMessageBox::warning(this,"Warnung",
	                             "Sie haben kein Bild gewählt",
	                             "Weiter");
	   }

	if (image.load( fileName ))
	   {
		   pixmap=pixmap.fromImage(image, 0);
		   view->setPixmap(pixmap);
		   view->resize( view->sizeHint() );
		   view->setLineWidth( 4 );
		   currentFile->setText(fileName);
	   }
	else QMessageBox::warning(this,"Warnung",
	                               "Dieses Bild ist beschädigt",
	                               "Weiter");
}

