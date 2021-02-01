#include "Riostream.h"
#include "TH1D.h"

void excelplot3 () {

    std::vector<double> xdata;
    double x;
    double xmax;
    double xmin;
    int cnt=0;

    ifstream data("file.dat"); //data file -> xdata

    if (!data) {
        cout<<"Attenzione: il file dati non esiste \n";
        return;
    }

    while(!data.eof()) {
        data>>x;
        xdata.push_back(x);
        if (cnt==0) {
            xmax=x;
            xmin=x;
        }
        if(x>xmax) xmax = x;
        if(x<xmin) xmin = x; 
        cnt++;
    }

    int size = (int)xdata.size();
    int bins = (int)TMath::Sqrt(size);
    cout<<" Numero di dati: "<<size<<"\n";
    TH1D *h1 = new TH1D("h1"," Plot Dati ",bins,xmin-0.1,xmax+0.1);

    for(int i=0;i<size;i++) h1->Fill(xdata[i]);

    h1->Draw();

} 