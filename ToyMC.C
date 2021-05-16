#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"
#include "TAxis.h"
#include "TRandom3.h"

const int kIter = 1e6; //numero iterazioni del toy MC
double expbkg (double x); //expected background
double chi2comp (TH1D *h1); //chi2 computing
double chi2sim (TH1D *h1); //chi2 computing
double ptosigma (double p); //pvalue to sigma

void ToyMC () {

    //lettura istogramma da file
    TFile *file = new TFile("hfile.root");
    TH1D *hdata = (TH1D*)file->Get("h1"); 

    //calcolo chi quadro osservato
    double chi2obs = chi2comp(hdata);

    cout<<" \n ------------------------- \n CHI QUADRO OSSERVATO = "<<chi2obs<<" \n ------------------------- ";

    //estrapolo distribuzione dal toy MC

    //definisco l'istogramma fino a 2 DOF
    double step = 0.5;
    double stepmin = 0.;
    double stepmax = 3*hdata->GetNbinsX(); //3 DOF
    int hsimbins = (int)((stepmax-stepmin)/step);
    TH1D *hsim = new TH1D("hsim","Distribuzione Simulata Toy MC",hsimbins,stepmin,stepmax);

    cout<<" \n ------------------------- \n SIMULAZIONE TOY MC \n ------------------------- \n";

    //simulazione della distribuzione
    for(int i=0;i<kIter;i++) {
        if(i%(kIter/10)==0) cout<<"  - ESTRAZIONE # "<<i<<endl;
        double chi2=chi2sim(hdata);
        hsim->Fill(chi2);
    }
    hsim->Scale(1./hsim->Integral()); //normalizzato a 1
    //hsim->Draw("histo");

    //calcolo il p-value nella distribuzione simulata
    double pval = hsim->Integral(hsim->GetXaxis()->FindBin(chi2obs),hsimbins,"width");
    cout<<" \n ------------------------- \n SIGNIFICATIVITA OSSERVATA = "<<pval<<" = "<<ptosigma(pval)<<" SIGMA  \n ------------------------- \n ";

    //traduzione in sigma della gaussiana a una coda
    //double psigma[10]={3/5.,1/3.,1/7.,1/22.,1/81.,1./370.,1./2149,1./15787,1./147160,1./1744278};
    //double sigma[10]={0.5,1.,1.5,2.,2.5,3,3.5,4.,4.5,5.};

}

//------------------------
//IMPLEMENTAZIONI FUNZIONI
//------------------------

double expbkg (double x) { //fondo atteso senza parametri
    return 0.169*(10-0.01*x);
    } 

double chi2comp (TH1D *h1) { //chi2 computing

    int hbins = h1->GetNbinsX(); //numero di bins non noto a priori
    double chi2 = 0.; //calcolo chi2
    for(int i=1;i<=hbins;i++) {
        double newterm = h1->GetBinContent(i)-expbkg(h1->GetBinCenter(i));
        newterm = newterm*newterm;
        newterm = newterm/expbkg(h1->GetBinCenter(i));
        chi2 = chi2 + newterm;
    }
    return chi2;

}

double chi2sim (TH1D *h1) { //chi2 simulation

    int hbins = h1->GetNbinsX(); //numero di bins non noto a priori
    double chi2 = 0.; //calcolo chi2
    for(int i=1;i<=hbins;i++) {
        double newterm = gRandom->Poisson(expbkg(h1->GetBinCenter(i)))-expbkg(h1->GetBinCenter(i));
        newterm = newterm*newterm;
        newterm = newterm/expbkg(h1->GetBinCenter(i));
        chi2 = chi2 + newterm;
    }
    return chi2;
    
}

double ptosigma(double p) {

    double rng = 50;
    if (p==0) return rng; //non è apprezzabile
    TF1 *f1 = new TF1("f1","gaus",-rng,rng);
    double step = 0.001; //step della sigma
    f1->SetParameters(1,0,1); //gaussiana centrata in 0 e con 1 dev std
    double norm = f1->Integral(-rng,rng);
    double x = 0.;
    double prob = 1.;
    while(prob>p) {
        x = x + step;
        prob = 1-f1->Integral(-x,x)/norm;
        prob = prob/2.;
    }
    delete f1;
    return x;

}