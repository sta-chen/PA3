
#include "stats.h"

stats::stats(PNG & im){

/* your code here! */
    sumHueX.clear();
    sumHueX.resize(im.height(), vector<double> (im.width(), 0));

    sumHueY.clear();
    sumHueY.resize(im.height(), vector<double> (im.width(), 0));

    sumSat.clear();
    sumSat.resize(im.height(), vector<double> (im.width(), 0));

    sumLum.clear();
    sumLum.resize(im.height(), vector<double> (im.width(), 0));

    hist.clear();
    hist.resize(im.height(), vector<vector<int>> (im.width(), vector<int> (36, 0)));

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

                for (int k = 0; k < 36; k++) {
                    hist[i][j][k] = hist[i][j - 1][k];
                }

                hist[i][j][pixel->h / 10]++;
            } else if (j == 0) {
                sumHueX[i][j] = sumHueX[i - 1][j] + cos(pixel->h * PI / 180);
                sumHueY[i][j] = sumHueY[i - 1][j] + sin(pixel->h * PI / 180);
                sumSat[i][j] = sumSat[i - 1][j] + pixel->s;
                sumLum[i][j] = sumLum[i - 1][j] + pixel->l;

                for (int k = 0; k < 36; k++) {
                    hist[i][j][k] = hist[i - 1][j][k];
                }

                hist[i][j][pixel->h / 10]++;

            } else {
                sumHueX[i][j] = sumHueX[i][j - 1] + sumHueX[i - 1][j] - sumHueX[i - 1][j - 1] + cos(pixel->h * PI / 180);
                sumHueY[i][j] = sumHueY[i][j - 1] + sumHueY[i - 1][j] - sumHueY[i - 1][j - 1] + sin(pixel->h * PI / 180);
                sumSat[i][j] = sumSat[i][j - 1] + sumSat[i - 1][j] - sumSat[i - 1][j - 1] + pixel->s;
                sumLum[i][j] = sumLum[i][j - 1] + sumLum[i - 1][j] - sumLum[i - 1][j - 1] + pixel->l;

                for (int k = 0; k < 36; k++) {
                    hist[i][j][k] = hist[i][j - 1][k] + hist[i - 1][j][k] + hist[i - 1][j - 1][k];
                }

                hist[i][j][pixel->h / 10]++;
            }

            
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

    vector<int> distn;

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
    double entropy = 0;
    long area = rectArea(ul,lr);
    int ui = ul.first;
    int uj = ul.second;
    int li = lr.first;
    int lj = lr.second;
    distn = hist[li][lj];

    if (ui == 0 && uj == 0) {
        for (int k = 0; k < 36; k++) {
            if (distn[k] > 0) {
                entropy += ((double) distn[k] / (double) area) * log2((double) distn[k] / (double) area);
            }
        }
    } else {
        vector<int> temp1 = hist[ui - 1][lj]; //left rectangle
        vector<int> temp2 = hist[li][uj - 1]; // upper rectangle
        vector<int> temp3 = hist[ui - 1][uj - 1]; // upper left rectangle
        for (int k = 0; k < 36; k++) {
            distn[k] = distn[k] - temp1[k] - temp2[k] + temp3[k];
            if (distn[k] > 0) {
                entropy += ((double) distn[k] / (double) area) * log2((double) distn[k] / (double) area);
            }
        }

    }
    return  -1 * entropy;
}
