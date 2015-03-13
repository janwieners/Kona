#include <qimage.h>
#include <qvector.h>
#include <math.h>
#include <iostream>
using namespace std;
#include "edge.h"


static unsigned int Filter8(QImage, int, int *, int, int ,unsigned char *);
static unsigned int Filter24(QImage, int, int *, int, int, QRgb *);
static QImage ReFilter(QImage, QImage, enum FilterType);




// ************************************
// 
QImage Filter(QImage image, enum FilterType type)
{
	static int filter[15][9]=  { 	{ 1, 1, 1, 1, 1, 1, 1, 1, 1}, //  0 = low pass 1 
                            		{ 0, 1, 0, 1, 1, 1, 0, 1, 0}, //  1 = low pass 2 
                            		{ 0,-1, 0,-1, 5,-1, 0,-1, 0}, //  2 = high pass 1 
                           			{-1,-1,-1,-1, 9,-1,-1,-1,-1}, //  3 = high pass 2 
                            		{ 1, 2, 1, 2, 4, 2, 1, 2, 1}, //  4 = W.Mean 1 
                            		{ 0, 1, 0, 1, 2, 1, 0, 1, 0}, //  5 = W.Mean 2 
                            		{ 0, 1, 0, 1,-4, 1, 0, 1, 0}, //  6 = Laplace 1 
                           			{-1,-1,-1,-1, 8,-1,-1,-1,-1}, //  7 = Laplace 2 
                            		{ 0,-1, 0,-1, 7,-1, 0,-1, 0}, //  8 = Laplace 3 
                           			{-1,-1,-1, 0, 0, 0, 1, 1, 1}, //  9 = Prewitt A 
                           			{-1, 0, 0, 0, 0, 0, 0, 0, 1}, // 10 = Roberts A 
                           			{-1,-2,-1, 0, 0, 0, 1, 2, 1}, // 11 = Sobel  A 
                            		{ 1, 0,-1, 1, 0,-1, 1, 0,-1}, // 12 = Prewitt B 
                            		{ 0, 0,-1, 0, 0, 0, 1, 0, 0}, // 13 = Roberts B 
                           			{-1, 0, 1,-2, 0, 2,-1, 0, 1}  // 14 = Sobel B  
                          		};
                          		
	static int divisor[15] = 	{9,5,1,1,16,6,1,1,3,1,1,1,1,1,1};
	static int absolute[15] = 	{0,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1};
	
	unsigned char *baseline;
	QRgb *basergbline, *resultline;

	double sobelx, sobely;
	double at, at2;

	QImage result=QImage(image.width(),image.height(),QImage::Format_RGB32);

	if (image.depth() == 8)
   	{
		if(type == FSobel45)
		{
			for (int y=1;y<result.height()-1;y++)
       		{
       			baseline=image.scanLine(y);
       			resultline=(QRgb *)result.scanLine(y);
       			for (int x=1;x<result.width()-1;x++)
				{
					// Handmade Sobel:
					// Für die Weiterverarbeitung sollen nur ganzzahlige Vielfache von 45° relevant sein.
           			sobelx = Filter8(image,x,filter[FSobel],divisor[FSobel],absolute[FSobel],baseline);
					sobely = Filter8(image,x,filter[FSobel+3],divisor[FSobel+3],absolute[FSobel+3],baseline);

					at=atan2(sobelx, sobely);
					at2=at/(2*3.1415)*360;

					if((int)at2%45==0)
						*(resultline + x) = Filter8(image,x,filter[type],divisor[type],absolute[type],baseline);
					else *(resultline + x) = 0;
				}
       		}
			return result;
		}
		else
		{
   		for (int y=1;y<result.height()-1;y++)
       	{
       		baseline=image.scanLine(y);
       		resultline=(QRgb *)result.scanLine(y);
       		for (int x=1;x<result.width()-1;x++)
           		*(resultline + x) = Filter8(image,x,filter[type],divisor[type],absolute[type],baseline);
       	}

   		if (type < FPrewitt || type > FSobel) 
   			return result;
   			
   		return ReFilter(image,result,(enum FilterType)(type+3));
		}
   	}

	if (image.depth() == 32)
   	{
   		for (int y=1;y<result.height()-1;y++)
       	{
       		basergbline	= (QRgb *)image.scanLine(y);
       		resultline	= (QRgb *)result.scanLine(y);
       
       		for (int x=1;x<result.width()-1;x++)
           		*(resultline + x) = Filter24(image,x,filter[type],divisor[type],absolute[type],basergbline);
       	}

   	if (type < FPrewitt || type > FSobel) 
   		return result;
   
   	return ReFilter(image,result,(enum FilterType)(type+3));
   	}

	return result;
}

// ************************************
// 
static unsigned int Filter8(QImage image, int x, int *filter, int divisor, int absolute, unsigned char *baseline)
{
	unsigned int result, candidate, width;
	const unsigned int *table;
	int collectRed, collectGreen, collectBlue;

	int Xoffset[] = { -1, 0, 1, -1, 0, 1, -1, 0, 1};
	int Yoffset[] = { -1, -1, -1, 0, 0, 0, 1, 1, 1};

	width=image.width();
	table=image.colorTable().constData();
	collectRed=collectGreen=collectBlue=0;

	for (int i=0;i < 9; i++)
    {
    	//candidate = image.colorTable()[*(baseline + (width*Yoffset[i]) + x + Xoffset[i])];
    	candidate 		 = table[*(baseline + (width*Yoffset[i]) + x + Xoffset[i])];
    	collectRed 		+= (int)(qRed(candidate)) * filter[i];
    	collectGreen 	+= (int)(qGreen(candidate)) * filter[i];
    	collectBlue 	+= (int)(qBlue(candidate)) * filter[i];
    }
    
	collectRed /= divisor;
	if (collectRed < 0) 
		collectRed *= absolute;
	else if (collectRed > 255) 
		collectRed = 255;
		
	collectGreen /= divisor;
	if (collectGreen < 0) 
		collectGreen *= absolute;
	else if (collectGreen > 255) 
		collectGreen = 255;
		
	collectBlue /= divisor;
	if (collectBlue < 0) 
		collectBlue *= absolute;
	else if (collectBlue > 255) 
		collectBlue = 255;

	result=qRgb(collectRed,collectGreen,collectBlue);

	return result;
}

// ************************************
// 
static unsigned int Filter24(QImage image, int x, int *filter, int divisor, int absolute, QRgb *basergbline)
{
	unsigned int result, candidate, width;
	int collectRed, collectGreen, collectBlue;

	int Xoffset[] = { -1, 0, 1, -1, 0, 1, -1, 0, 1};
	int Yoffset[] = { -1, -1, -1, 0, 0, 0, 1, 1, 1};

	width=image.width();
	
	collectRed=collectGreen=collectBlue=0;
	
	for (int i=0;i < 9; i++)
    {
    	candidate = *(basergbline + (width*Yoffset[i]) + x + Xoffset[i]);
    	collectRed 		+= (int)(qRed(candidate)) * filter[i];
    	collectGreen 	+= (int)(qGreen(candidate)) * filter[i];
    	collectBlue 	+= (int)(qBlue(candidate)) * filter[i];
    }
    
	collectRed /= divisor;
	if (collectRed < 0) 
		collectRed *= absolute;
	else if (collectRed > 255) 
		collectRed = 255;

	collectGreen /= divisor;
	if (collectGreen < 0) 
		collectGreen *= absolute;
	else if (collectGreen > 255) 
		collectGreen = 255;
	
	collectBlue /= divisor;
	if (collectBlue < 0) 
		collectBlue *= absolute;
	else if (collectBlue > 255) 
		collectBlue = 255;

	result=qRgb(collectRed,collectGreen,collectBlue);

	return result;
}

// ************************************
//
static QImage ReFilter(QImage image, QImage result, enum FilterType type)
{
	QRgb a,b;
	int collectRed, collectGreen, collectBlue;
	QImage part2 = Filter(image,type);
	QRgb *resultline, *part2line;

	for (int y=0;y<result.height();y++)
    {
    	resultline	= (QRgb *)result.scanLine(y);
    	part2line	= (QRgb *)part2.scanLine(y);
    	
    	for (int x=0;x<result.width();x++)
        {
        	a=*(resultline + x);
        	b=*(part2line + x);
        
        	if ((collectRed = qRed(a) + qRed(b)) > 255) 
        		collectRed = 255;
        	if ((collectGreen = qGreen(a) + qGreen(b)) > 255) 
        		collectGreen = 255;
        	if ((collectBlue = qBlue(a) + qBlue(b)) > 255) 
        		collectBlue = 255;
        		
        	*(resultline + x) = qRgb(collectRed,collectGreen,collectBlue);
        }
    }

	return result;
}

