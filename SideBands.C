#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TMath.h"
#include "iomanip"
#include "TCanvas.h"

const double psigma[10]={3/5.,1/3.,1/7.,1/22.,1/81.,1./370.,1./2149,1./15787,1./147160,1./1744278};
const double sigma[10]={0.5,1.,1.5,2.,2.5,3,3.5,4.,4.5,5.};
const double thr = psigma[7]/2.; //4 sigma di soglia 

double chi2comp (TH1D *h1, TF1 *f1); //chi2 computing, f1 è la funzione attesa
double chi2maxcomp (TH1D *h1, TF1 *f1); //chi2 max computing
double chi2mincomp (TH1D *h1, TF1 *f1); //chi2 min computing 
double wmean (double x1, double x2, double p1, double p2); //media pesata per x1 e x2 di pesi p1 e p2
double swmean (double p1, double p2); //incertezza media pesata
double fmax (TF1 *f1, double x); //trova il massimo valore possibile nel range di incertezze
double fmin (TF1 *f1, double x); //trova il minimo valore possibile nel range di incertezze
void swap (double &x, double &y); //scambia x e y
double ptosigma(double p); //converte p value in sigma

void SideBands () {

    //lettura file
    TFile *f = new TFile("hgaus.root");
    TH1D *hdata = (TH1D*)f->Get("h1");

    hdata->SetMarkerStyle(21);
   	hdata->SetMarkerSize(0.6);
    hdata->GetXaxis()->SetTitle(" E [MeV] ");
    hdata->GetYaxis()->SetTitle("Counts");
    hdata->SetTitle(" Dati ");

    //funzioni fit
    TF1 *bkgl = new TF1("bkgl","expo",0.,9.); //background sinistro
    TF1 *bkgr = new TF1("bkgr","expo",41.,50.); //background destro
    TF1 *fit = new TF1("fit","expo(0)+gaus(2)",0.,50.); //fit totale con segnale

    //inizializzare manualmente i parametri per il fondo
    bkgl->SetParameter(1,1./20);
    bkgr->SetParameter(1,1./20);

    //fissare i limiti per il fondo manualmente
    //bkgl->SetParLimits(1,15,25);
    //bkgr->SetParLimit(1,15,25);

    //fit nelle sideband - regioni del fondo
    hdata->Fit("bkgl","R0");
    hdata->Fit("bkgr","R0");

    //calcolo significance con ipotesi che ci sia solo il fondo
    TF1 *bkg = new TF1("bkgt","expo",0.,50.); //background totale

    //media pesata dei parametri del fondo
    bkg->SetParameter(0,wmean(bkgl->GetParameter(0),bkgr->GetParameter(0),bkgl->GetParError(0),bkgr->GetParError(0)));
    bkg->SetParameter(1,wmean(bkgl->GetParameter(1),bkgr->GetParameter(1),bkgl->GetParError(1),bkgr->GetParError(1)));
    bkg->SetParError(0,swmean(bkgl->GetParError(0),bkgr->GetParError(0)));
    bkg->SetParError(1,swmean(bkgl->GetParError(1),bkgr->GetParError(1)));

    double chi2 = chi2comp(hdata,bkg); 
    double chi2max = chi2maxcomp(hdata,bkg); //non è il chi quadro massimo -> è quello corrispondente alla funzione massimizzata nel range di incertezze
    double chi2min = chi2mincomp(hdata,bkg); //non è il minimo per lo stesso motivo

    cout<<" \n ------------------------- \n CHI QUADRO OSSERVATO (BKG ONLY) = "<<chi2<<" (+ "<<chi2max<<")(- "<<chi2min<<") \n ------------------------- \n ";
    
    int dof = hdata->GetNbinsX();

    double rangemax = dof*2;
    double rangemin = dof*0.5;
    TF1 *pears = new TF1("pears","ROOT::Math::chisquared_pdf(x,[0])",rangemin,rangemax); 
    pears->SetParameter(0,dof);
    pears->Draw(); 

    double pval = 0.;
    double pvalmin = 0.;
    double pvalmax = 0.; 

    if (chi2<rangemax) pval = pears->Integral(chi2,rangemax);
    if (chi2max<rangemax) pvalmin = pears->Integral(chi2max,rangemax);
    if (chi2min<rangemax) pvalmax = pears->Integral(chi2min,rangemax);

    cout<<" \n ------------------------- \n SIGNIFICATIVITA OSSERVATA (BKG ONLY) = "<<pval<<" (+ "<<pvalmax<<")(- "<<pvalmin<<") \n SIGNIFICATIVITA OSSERVATA (BKG ONLY) = "<<ptosigma(pval)<<" (+ "<<ptosigma(pvalmax)<<")(- "<<ptosigma(pvalmin)<<") SIGMA \n ------------------------- \n ";

    if (pval<thr) cout<<" \n ------------------------- \n L'ECCESSO DI EVENTI DEPONE PER L'EFFETTIVA OSSERVAZIONE DI UN SEGNALE - SOGLIA = "<<thr<<" \n ------------------------- \n";

    //inzializzare manualmente i parametri per il segnale
    fit->SetParameter(3,25); //media
    fit->SetParameter(4,5); //sigma

    //fissare manualmente i limiti per il segnale
    fit->SetParLimits(3,20,30); //media
    fit->SetParLimits(4,2,8); //sigma

    //fissare automaticamente i parametri per il fondo totale
    for(int i=0;i<=1;i++) {
        fit->SetParameter(i,bkg->GetParameter(i));
        //fit->SetParLimits(i,bkg->GetParameter(i)-0.2*bkg->GetParameter(i),bkg->GetParameter(i)+0.2*bkg->GetParameter(i));
    }

    //fit totale
    hdata->Fit("fit","R0");

    double gmean = fit->GetParameter(3);
    double gsigma = fit->GetParameter(4);

    //funzioni risultanti
    TF1 *sig = new TF1("sig","gaus",gmean-3.5*gsigma,gmean+3.5*gsigma); //nuovo segnale

    //disegno contributi
    gStyle->SetOptFit(1111);
    gStyle->SetOptStat(0);

    for(int i=0;i<=2;i++) sig->SetParameter(i,fit->GetParameter(i+2));
    for(int i=0;i<=1;i++) bkg->SetParameter(i,fit->GetParameter(i));

    fit->SetLineColor(kRed);
    fit->SetTitle(" Fit Totale ");
    bkg->SetLineColor(kOrange);
    bkg->SetTitle(" Background ");
    sig->SetLineColor(kGreen);
    sig->SetTitle(" Signal ");

    TCanvas *c = new TCanvas();
    hdata->Draw("e1");
    fit->Draw("same");
    sig->Draw("same");
    bkg->Draw("same");
    c->BuildLegend();

}

double chi2comp (TH1D *h1,TF1 *f1) { //chi2 computing

    int hbins = h1->GetNbinsX(); //numero di bins non noto a priori
    double chi2 = 0.; //calcolo chi2
    for(int i=1;i<=hbins;i++) {
        double newterm = h1->GetBinContent(i)-f1->Eval(h1->GetBinCenter(i));
        newterm = newterm*newterm;
        newterm = newterm/f1->Eval(h1->GetBinCenter(i));
        chi2 = chi2 + newterm;
    }

    return chi2;

}

double chi2maxcomp (TH1D *h1,TF1 *f1) { //chi2 max computing

    int hbins = h1->GetNbinsX(); //numero di bins non noto a priori
    double chi2 = 0.; //calcolo chi2
    for(int i=1;i<=hbins;i++) {
        double s = fmax(f1,h1->GetBinCenter(i));
        double newterm = h1->GetBinContent(i)-s;
        newterm = newterm*newterm;
        newterm = newterm/s;
        chi2 = chi2 + newterm;
    }
    return chi2;

}

double chi2mincomp (TH1D *h1,TF1 *f1) { //chi2 min computing

    int hbins = h1->GetNbinsX(); //numero di bins non noto a priori
    double chi2 = 0.; //calcolo chi2
    for(int i=1;i<=hbins;i++) {
        double s = fmin(f1,h1->GetBinCenter(i));
        double newterm = h1->GetBinContent(i)-s;
        newterm = newterm*newterm;
        newterm = newterm/s;
        chi2 = chi2 + newterm;
    }
    return chi2;

}

double wmean (double x1, double x2, double p1, double p2) {

    p1 = 1./(p1*p1);
    p2 = 1./(p2*p2);
    double n = x1*p1 + x2*p2;
    double d = p1+p2;
    return n/d;

}

double swmean (double p1, double p2) {
    p1 = 1./(p1*p1);
    p2 = 1./(p2*p2);
    double s = TMath::Sqrt(p1+p2);
    return 1/s;
}

double fmax (TF1 *f1, double x) { 
    
    int iter = 100; //numero di iterazioni nel ciclo
    double r = f1->Eval(x); //iniziamo da x
    double par0 = f1->GetParameter(0);
    double spar0 = f1->GetParError(0);
    double par1 = f1->GetParameter(1);
    double spar1 = f1->GetParError(1);
    double step0 = (par0+2*spar0)/iter;
    double step1 = (par1+2*spar1)/iter;
    if (step0<0) step0=-step0;
    if (step1<0) step1=-step1;

    for(double i = par0-spar0;i<=(par0+spar0);i=i+step0){
        for(double j=par1-spar1;j<=(par1+spar1);j=j+step1){
            if (i>0) f1->SetParameter(0,i);
            if (j>0) f1->SetParameter(1,j);
            double s = f1->Eval(x);
            if (s>r) r=s;
        }
    }

    return r;

}

double fmin (TF1 *f1, double x) { 
    
    int iter = 100; //numero di iterazioni nel ciclo
    double r = f1->Eval(x); //iniziamo da x
    double par0 = f1->GetParameter(0);
    double spar0 = f1->GetParError(0);
    double par1 = f1->GetParameter(1);
    double spar1 = f1->GetParError(1);
    double step0 = (par0+2*spar0)/iter;
    double step1 = (par1+2*spar1)/iter;

    if (step0<0) step0=-step0;
    if (step1<0) step1=-step1;

    for(double i = par0-spar0;i<=par0+spar0;i=i+step0){
        for(double j=par1-spar1;j<=par1+spar1;j=j+step1){
            if (i>0) f1->SetParameter(0,i);
            if (j>0) f1->SetParameter(1,j);
            double s = f1->Eval(x);
            if (s<r) r=s;
        }
    }

    return r;
    
}

void swap (double &x, double &y) {

    double t = x;
    x = y;
    y = t;

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