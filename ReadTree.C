#include "Riostream.h"
#include "TH1D.h"
#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"
#include "TMath.h"
#include "TLatex.h"

double sqsum (double a, double b) {return TMath::Sqrt(a*a+b*b);}

void ReadTree () {

    int eventID;

    typedef struct {
        double pmed;
        double pmin;
        double pmax;
    } p;
    static p plab; //plab  

    typedef struct {
        double sig;
        double statpos;
        double statneg;
        double sys;
    } sigma;
    static sigma xs; //cross section

    double step = 0.001; //step
    double rngmax = 2; //fino a 2 GeV
    double rngmin = 0.; //da 0 GeV
    int nbins = (int)((rngmax-rngmin)/step);
    TH1D *h1 = new TH1D("h1","h1",nbins,rngmin,rngmax);

    TFile f("hfile.root"); //lettura file
    TTree *tree = (TTree*)f.Get("T");
    TBranch *b1 = tree->GetBranch("eventID");
    TBranch *b2 = tree->GetBranch("plab[GeV/c]");
    TBranch *b3 = tree->GetBranch("CrossSection[mb]");

    b1->SetAddress(&eventID);
    b2->SetAddress(&plab.pmed);
    b3->SetAddress(&xs.sig);

    int entries = (int)tree->GetEntries();

    for (int i=0;i<entries;i++) {
        tree->GetEvent(i);
        h1->Fill(plab.pmed,xs.sig);
        //h1->SetBinError( h1->GetXaxis()->FindBin(plab.pmed) , sqsum((xs.statneg+xs.statpos)/2.,xs.sys) );
    }

    h1->SetTitle(" Sezione d'urto #bar{p} - d ");
    h1->GetYaxis()->SetTitle(" #sigma [mbar] ");
    h1->GetXaxis()->SetTitle(" pLAB [GeV/c] ");
    //h1->GetYaxis()->SetRangeUser(0.,h1->GetMaximum(rngmin,rngmax)+5.);
    h1->SetMarkerStyle(20);
    h1->SetMarkerColor(kRed);
    h1->Draw("PE1");


}