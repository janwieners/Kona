enum FilterType   { FLowpass1,	FLowpass2, 	FHighpass1, 	FHighpass2,
                    FWMean1, 	FWMean2, 	FLaplace1, 		FLaplace2, 		FLaplace3,
                    FPrewitt, 	FRoberts, 	FSobel, FSobel45 
                  };

QImage Filter(QImage, FilterType);
