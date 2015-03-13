bool Convert2Grayscale					(QImage *image);
bool Convert2Mono						(QImage *image, int threshold);
bool nextneighbours						(QImage *image, unsigned int maxneighbours);
bool TInegate							(QImage *image);
bool TextSegment_DCT_energy				(QImage *image);
bool TextSegment_DCT_bitrate			(QImage *image);
bool TextSegment_DCT_energy_train		(QImage *image);
bool TextSegment_DCT_energy_ontrained	(QImage *image);
void postprocess						(QImage *image);

