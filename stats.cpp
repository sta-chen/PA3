
#include "stats.h"

stats::stats(PNG & im){

for (unsigned int i = 0; i < im.width(); i++) {
        vector<vector<int>> iHist;
        for (unsigned int j = 0; j < im.height(); j++) {
            vector<int> jHist;
            for (unsigned int k = 0; k < 36; k++) {
                jHist.push_back(0);
            }
            iHist.push_back(jHist);
        }
        hist.push_back(iHist);
    }

    for (unsigned int x = 0; x < im.width(); x++) {
        vector<double> xHueX;
        vector<double> xHueY;
        vector<double> xSat;
        vector<double> xLum;

        for (unsigned int y = 0; y < im.height(); y++) {

            HSLAPixel * curr = im.getPixel(x,y);

            int k = curr->h/10;


            if ((y == 0) && (x == 0)) {
                xHueX.push_back(cos((PI/180)*(curr->h)));
                xHueY.push_back(sin((PI/180)*(curr->h)));
                xSat.push_back(curr->s);
                xLum.push_back(curr->l);
                hist[x][y][k]++;
            }

            else if (x == 0) {
                xHueX.push_back(xHueX[y-1] + cos((PI/180)*(curr->h)));
                xHueY.push_back(xHueY[y-1] + sin((PI/180)*(curr->h)));
                xSat.push_back(xSat[y-1] + curr->s);
                xLum.push_back(xLum[y-1] + curr->l);
                for (int i = 0; i < 36; i++)
                    hist[x][y][i] = hist[x][y-1][i];
                hist[x][y][k] = hist[x][y-1][k] + 1;
            }

            else if (y == 0) {
                xHueX.push_back(sumHueX[x-1][y] + cos((PI/180)*(curr->h)));
                xHueY.push_back(sumHueY[x-1][y] + sin((PI/180)*(curr->h)));
                xSat.push_back(sumSat[x-1][y] + curr->s);
                xLum.push_back(sumLum[x-1][y] + curr->l);
                for (int i = 0; i < 36; i++)
                    hist[x][y][i] = hist[x-1][y][i];
                hist[x][y][k] = hist[x-1][y][k] + 1;   
            } else {
                xHueX.push_back(xHueX[y-1] + sumHueX[x-1][y] -
                sumHueX[x-1][y-1] + cos((PI/180)*(curr->h)));
                xHueY.push_back(xHueY[y-1] + sumHueY[x-1][y] -
                sumHueY[x-1][y-1] + sin((PI/180)*(curr->h)));
                xSat.push_back(sumSat[x-1][y] + xSat[y-1] -
                sumSat[x-1][y-1] + curr->s);
                xLum.push_back(sumLum[x-1][y] + xLum[y-1] -
                sumLum[x-1][y-1] + curr->l);
                for (int i = 0; i < 36; i++)
                    hist[x][y][i] = hist[x-1][y][i] + hist[x][y-1][i] - hist[x-1][y-1][i];
                hist[x][y][k] = hist[x-1][y][k] + hist[x][y-1][k] - hist[x-1][y-1][k] + 1;            
            }

        }
        sumHueX.push_back(xHueX);
        sumHueY.push_back(xHueY);
        sumSat.push_back(xSat);
        sumLum.push_back(xLum);
        
    }

}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){
    return (lr.first - ul.first + 1) * (lr.second - ul.second + 1);
}

HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){
    double X;
    double Y;
    double S;
    double L;

    if ((ul.first == 0) && (ul.second == 0)) {
        X = sumHueX[lr.first][lr.second];
        Y = sumHueY[lr.first][lr.second];
        S = sumSat[lr.first][lr.second];
        L = sumLum[lr.first][lr.second];
    }

    else if (ul.first == 0) {
        X = sumHueX[lr.first][lr.second] - sumHueX[lr.first][ul.second - 1];
        Y = sumHueY[lr.first][lr.second] - sumHueY[lr.first][ul.second - 1];
        S = sumSat[lr.first][lr.second] - sumSat[lr.first][ul.second - 1];
        L = sumLum[lr.first][lr.second] - sumLum[lr.first][ul.second - 1];
    }

    else if (ul.second == 0) {
        X = sumHueX[lr.first][lr.second] - sumHueX[ul.first - 1][lr.second];
        Y = sumHueY[lr.first][lr.second] - sumHueY[ul.first - 1][lr.second];
        S = sumSat[lr.first][lr.second] - sumSat[ul.first - 1][lr.second];
        L = sumLum[lr.first][lr.second] - sumLum[ul.first - 1][lr.second];
    } else {
        X = sumHueX[lr.first][lr.second] + sumHueX[ul.first - 1][ul.second - 1] - sumHueX[lr.first][ul.second - 1] - sumHueX[ul.first - 1][lr.second];
        Y = sumHueY[lr.first][lr.second] + sumHueY[ul.first - 1][ul.second - 1] - sumHueY[lr.first][ul.second - 1] - sumHueY[ul.first - 1][lr.second];
        S = sumSat[lr.first][lr.second] + sumSat[ul.first - 1][ul.second - 1] - sumSat[lr.first][ul.second - 1] - sumSat[ul.first - 1][lr.second];
        L = sumLum[lr.first][lr.second] + sumLum[ul.first - 1][ul.second - 1] - sumLum[lr.first][ul.second - 1] - sumLum[ul.first - 1][lr.second];
    }
    
    double avgHue = atan2(Y,X) * 180 / PI;
    if(avgHue < 0) avgHue += 360;

    double avgSat = S/rectArea(ul,lr);
    double avgLum = L/rectArea(ul,lr);
    return HSLAPixel(avgHue, avgSat, avgLum);
}

double stats::entropy(pair<int,int> ul, pair<int,int> lr){
    double entropy = 0.0;
    vector<int> distn;
    for (int k = 0; k < 36; k++) {
        if ((ul.first == 0) && (ul.second == 0))
            distn.push_back(hist[lr.first][lr.second][k]);
       
        else if (ul.first == 0)
            distn.push_back(hist[lr.first][lr.second][k] - hist[lr.first][ul.second - 1][k]);

        else if (ul.second == 0)
            distn.push_back(hist[lr.first][lr.second][k] - hist[ul.first - 1][lr.second][k]);

        else
            distn.push_back(hist[lr.first][lr.second][k] + hist[ul.first - 1][ul.second - 1][k] - hist[lr.first][ul.second - 1][k] - hist[ul.first - 1][lr.second][k]);

        if (distn[k] > 0 ) 
            entropy += ((double) distn[k]/(double) rectArea(ul,lr)) * log2((double) distn[k]/ (double) rectArea(ul,lr));
    }

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
    
    return 0-entropy;
}
