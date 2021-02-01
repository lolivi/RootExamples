#include "Riostream.h"
#include "TGraphErrors.h"

void swap(double &a,double&b);
void orderdata (std::vector<double> &xdata, std::vector<double> &ydata); //xdata in ascending order
void edata (std::vector<double> xdata, std::vector<double> ydata,std::vector<double> &exdata, std::vector<double> &eydata); //errors implementation
void printvec (std::vector<double> data);

void excelplot () {

    std::vector<double> xdata;
    std::vector<double> ydata;
    std::vector<double> exdata;
    std::vector<double> eydata;
    double x,y;

    ifstream data("file.dat"); //data file -> xdata + blank space + ydata

    if (!data) {
        cout<<"Attenzione: il file dati non esiste \n";
        return;
    }
    while(!data.eof()) {
        data>>x>>y;
        xdata.push_back(x);
        ydata.push_back(y);
    }
    if(xdata.size()!=ydata.size()) {
        cout<<" Attenzione: diverse dimensioni fra dati su x e dati su y\n";
        return;
    }

    orderdata(xdata,ydata);
    edata(xdata,ydata,exdata,eydata);
    int size = (int)xdata.size();
    cout<<" Numero di dati: "<<size<<"\n";

    TGraphErrors *g1 = new TGraphErrors(size,&xdata[0],&ydata[0],&exdata[0],&eydata[0]);
    g1->Draw();

} 

void swap(double &a,double&b) {
    double t;
    t = b;
    b = a;
    a = t;
}

void orderdata (std::vector<double> &xdata, std::vector<double> &ydata) {
    for(int i=0;i<(int)xdata.size();i++) {
        for(int j=i+1; j<(int)xdata.size(); j++) {
            if (xdata[j]<xdata[i]) {
                swap(xdata[i],xdata[j]);
                swap(ydata[i],ydata[j]);
            }
        }
    }
}

void edata (std::vector<double> xdata, std::vector<double> ydata,std::vector<double> &exdata, std::vector<double> &eydata) {
    for (int i=0;i<(int)xdata.size();i++){
        exdata.push_back(0.1);
        eydata.push_back(0.5);
    }
}

void printvec (std::vector<double> data) {
    cout<<" v = {";
    for(int i=0;i<(int)data.size();i++) {
        cout<<data[i]<<" ";
    }
    cout<<"}\n";
}

