
#include "stats.h"

stats::stats(PNG & im){

/* your code here! */
	sumHueX.clear();
	sumHueX.resize(im.width(), vector<double> (im.height(), 0));

	sumHueY.clear();
	sumHueY.resize(im.width(), vector<double> (im.height(), 0));

	sumSat.clear();
	sumSat.resize(im.width(), vector<double> (im.height(), 0));

	sumLum.clear();
	sumLum.resize(im.width(), vector<double> (im.height(), 0));

	hist.clear();
	hist.resize(im.width(), vector<vector<int>> (im.height(), vector<int> (36, 0)));

	for (unsigned int i = 0; i < im.width(); i++) {
		for(unsigned int j = 0; j < im.height(); j++) {
			HSLAPixel* pixel = im.getPixel(i,j);
			if (i == 0 && j ==0) {

				sumHueX[i][j] = cos(pixel->h * PI / 180);
				sumHueY[i][j]= sin(pixel->h * PI / 180);
				sumSat[i][j] = pixel->s;
				sumLum[i][j] = pixel->l;
				hist[i][j][pixel->h / 10]++;
			} else if (i == 0) {
				sumHueX[i][j] = sumHueX[i][j - 1] + cos(pixel->h * PI / 180);
				sumHueY[i][j] = sumHueY[i][j - 1] + sin(pixel->h * PI / 180);
				sumSat[i][j] = sumSat[i][j - 1] + pixel->s;
				sumLum[i][j] = sumLum[i][j - 1] + pixel->l;

				createHistogram(i,j);

				hist[i][j][pixel->h / 10]++;
			} else if (j == 0) {
				sumHueX[i][j] = sumHueX[i - 1][j] + cos(pixel->h * PI / 180);
				sumHueY[i][j] = sumHueY[i - 1][j] + sin(pixel->h * PI / 180);
				sumSat[i][j] = sumSat[i - 1][j] + pixel->s;
				sumLum[i][j] = sumLum[i - 1][j] + pixel->l;

				createHistogram(i,j);

				hist[i][j][pixel->h / 10]++;

			} else {
				sumHueX[i][j] = sumHueX[i][j - 1] + sumHueX[i - 1][j] - sumHueX[i - 1][j - 1] + cos(pixel->h * PI / 180);
				sumHueY[i][j] = sumHueY[i][j - 1] + sumHueY[i - 1][j] - sumHueY[i - 1][j - 1] + sin(pixel->h * PI / 180);
				sumSat[i][j] = sumSat[i][j - 1] + sumSat[i - 1][j] - sumSat[i - 1][j - 1] + pixel->s;
				sumLum[i][j] = sumLum[i][j - 1] + sumLum[i - 1][j] - sumLum[i - 1][j - 1] + pixel->l;

				createHistogram(i,j);

				hist[i][j][pixel->h / 10]++;
			}


		}
	}

}

void stats::createHistogram(int x, int y){
	int rangeOfBins = 35;
	for(int k = 0; k < rangeOfBins; k++){
		if (x ==0){
			hist[x][y][k] = hist[x][y-1][k];
		}else if(y == 0){
			hist[x][y][k] = hist[x-1][y][k];
		}else {
			hist[x][y][k] = hist[x-1][y][k]+hist[x][y-1][k]-hist[x-1][y-1][k];
		}
	}
}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){

/* your code here */
	long area = (lr.first - ul.first + 1) * (lr.second - ul.second + 1);
	return area;

}

HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){

/* your code here */
	HSLAPixel avg;
	long area = rectArea(ul,lr);
	int ui = ul.first;
	int uj = ul.second;
	int li = lr.first;
	int lj = lr.second;
	double avgSat, avgLum, avgHue;

	if(ui == 0 && uj == 0) {

		avgHue = atan2((sumHueY[li][lj]), (sumHueX[li][lj])) * 180 /PI;
		avgSat = sumSat[li][lj] / area;
		avgLum = sumLum[li][lj] / area;

	} else if (ui == 0) {

		avgHue = atan2((sumHueY[li][lj] - sumHueY[ui][uj - 1]), (sumHueX[li][lj] - sumHueX[ui][uj - 1])) * 180 / PI;
		avgSat = (sumSat[li][lj] - sumSat[ui][uj - 1]) / area;
		avgLum = (sumLum[li][lj] - sumLum[ui][uj - 1]) / area;

	} else if (uj == 0) {

		avgHue = atan2((sumHueY[li][lj] - sumHueY[ui - 1][uj]), (sumHueX[li][lj] - sumHueX[ui - 1][uj])) * 180 / PI;
		avgSat = (sumSat[li][lj] - sumSat[ui - 1][uj]) / area;
		avgLum = (sumLum[li][lj] - sumLum[ui - 1][uj]) / area;

	} else {

		avgHue = atan2((sumHueY[li][lj] - sumHueY[ui - 1][uj] - sumHueY[ui][uj - 1]), (sumHueX[li][lj] - sumHueX[ui - 1][uj] - sumHueX[ui][uj - 1])) * 180 / PI;
		avgSat = (sumSat[li][lj] - sumSat[ui - 1][uj] - sumSat[ui][uj - 1]) / area;
		avgLum = (sumLum[li][lj] - sumLum[ui - 1][uj] - sumLum[ui][uj - 1]) / area;

	}

	avg = HSLAPixel(avgHue, avgSat, avgLum);


	return avg;


}

double stats::entropy(pair<int,int> ul, pair<int,int> lr){

	vector<int> distn(36, 0);

    /* using private member hist, assemble the distribution over the
    *  given rectangle defined by points ul, and lr into variable distn.
    *  You will use distn to compute the entropy over the rectangle.
    *  if any bin in the distn has frequency 0, then do not add that 
    *  term to the entropy total. see .h file for more details.
    */

    /* my code includes the following lines:
        if (distn[i] > 0 ) 
            entropy += ((double) distn[i]/(double) area) 
                                    * log2((double) distn[i]/(double) area);
    */

	for (int i = 0; i < 36; i++) {
		if (ul.first == 0 && ul.second == 0) {
			distn[i] = hist[lr.first][lr.second][i];
		} else if (ul.second == 0) {
			distn[i] = hist[lr.first][lr.second][i] - hist[ul.first - 1][lr.second][i];
		} else if (ul.first == 0) {
			distn[i] = hist[lr.first][lr.second][i] - hist[lr.first][ul.second - 1][i];
		} else {
			distn[i] = hist[lr.first][lr.second][i] - hist[ul.first - 1][lr.second][i] -
			hist[lr.first][ul.second - 1][i] + hist[ul.first - 1][ul.second - 1][i];
		}
	}

	long area = rectArea(ul, lr);
	double entropy = 0;
	for (int i = 0; i < 36; i++) {
		if (distn[i] > 0 )
			entropy += ((double) distn[i]/(double) area)
		* log2((double) distn[i]/(double) area);
	}

	return  -1 * entropy;
}










