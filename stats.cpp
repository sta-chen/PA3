
#include "stats.h"

stats::stats(PNG & im){

/* your code here! */
	for (unsigned int i = 0; i < im.width(); i++) {
        for(unsigned int j = 0; j < im.height(); j++) {
            HSLAPixel* pixel = im.getPixel(i,j);
            if (i == 0 && j ==0) {

                sumHueX[i][j] = cos(pixel->h * PI / 180);
                sumHueY[i][j]= sin(pixel->h * PI / 180);
                sumSat[i][j] = pixel->s;
                sumLum[i][j] = pixel->l;
            } else if (i == 0) {
                sumHueX[i][j] = sumHueX[i][j - 1] + cos(pixel->h * PI / 180);
                sumHueY[i][j] = sumHueY[i][j - 1] + sin(pixel->h * PI / 180);
                sumSat[i][j] = sumSat[i][j - 1] + pixel->s;
                sumLum[i][j] = sumLum[i][j - 1] + pixel->l;
            } else if (j == 0) {
                sumHueX[i][j] = sumHueX[i - 1][j] + cos(pixel->h * PI / 180);
                sumHueY[i][j] = sumHueY[i - 1][j] + sin(pixel->h * PI / 180);
                sumSat[i][j] = sumSat[i - 1][j] + pixel->s;
                sumLum[i][j] = sumLum[i - 1][j] + pixel->l;
            } else {
                sumHueX[i][j] = sumHueX[i][j - 1] + sumHueX[i - 1][j] - sumHueX[i - 1][j - 1] + cos(pixel->h * PI / 180);
                sumHueY[i][j] = sumHueY[i][j - 1] + sumHueY[i - 1][j] - sumHueY[i - 1][j - 1] + sin(pixel->h * PI / 180);
                sumSat[i][j] = sumSat[i][j - 1] + sumSat[i - 1][j] - sumSat[i - 1][j - 1] + pixel->s;
                sumLum[i][j] = sumLum[i][j - 1] + sumLum[i - 1][j] - sumLum[i - 1][j - 1] + pixel->l;
            }

            for (int k = 0; k < 36; k++) {
                int count = 0;
                for (unsigned int m = 0; m < i + 1; m++) {
                    for (unsigned int n = 0; n < j + 1; n++){
                        HSLAPixel* currP = im.getPixel(m,n);
                        double currH = currP->h;
                        if (k == 0 && currH / 10 == 36) {
                            count++;
                        } else if (currH / 10 == k) {
                            count++;
                        }
                    }
                }
                hist[i][j][k] = count;
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
    if (ui == 0 && uj == 0) {
        double avgHueX = sumHueX[li][lj];
        double avgHueY = sumHueY[li][lj];
        double avgSat = sumSat[li][lj] / area;
        double avgLum = sumLum[li][lj] / area;

        double avgHue = atan(avgHueY / avgHueX) * 180 / PI;
        avg.h = avgHue;
        avg.s = avgSat;
        avg.l = avgLum;
        avg.a = 1.0;
        
    } else {
        double avgHueX = (sumHueX[li][lj] - sumHueX[ui - 1][lj] - sumHueX[li][uj - 1] + sumHueX[ui - 1][uj - 1]);
        double avgHueY = (sumHueY[li][lj] - sumHueY[ui - 1][lj] - sumHueY[li][uj - 1] + sumHueY[ui - 1][uj - 1]);
        double avgSat = (sumSat[li][lj] - sumSat[ui - 1][lj] - sumSat[li][uj - 1] + sumSat[ui - 1][uj - 1]) / area;
        double avgLum = (sumLum[li][lj] - sumLum[ui - 1][lj] - sumLum[li][uj - 1] + sumLum[ui - 1][uj - 1]) / area;

        double avgHue = atan(avgHueY / avgHueX) * 180 / PI;
        avg.h = avgHue;
        avg.s = avgSat;
        avg.l = avgLum;
        avg.a = 1.0;
    }

    

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
