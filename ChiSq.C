#include "Riostream.h"
#include "TMath.h"
#include "TF1.h"

const int kIter = 1e4; //divide il range per integrale

void ChiSq () {

    double alpha;
    double dof;
    double chisqcrit;

    cout<<" \n --------------------- \n CALCOLO CHI QUADRATO CRITICO \n --------------------- \n TEST A UNA CODA (DX) \n";
    do{
        cout<<" - Inserisci Alpha: ";
        cin>>alpha;
    } while (alpha<0.);

    do{
        cout<<" - Inserisci DOF: ";
        cin>>dof;
    } while (dof<0.);

    double rangemax = dof*2;
    double rangemin = dof*0.5;

    TF1 *pears = new TF1("pears","ROOT::Math::chisquared_pdf(x,[0])",rangemin,rangemax); 
    pears->SetParameter(0,dof);

    double x = rangemin;
    double step = (rangemax-rangemin)/kIter;
    double integ = 10.;

    do {
        integ = pears->Integral(x,rangemax);
        x=x+step;
    } while (x<rangemax && integ>alpha);

    if(x<rangemax) chisqcrit = x-step;

    cout<<" --------------------- \n CHI2 CRITICO: "<<chisqcrit;
    cout<<" \n --------------------- \n CHI2 RIDOTTO: "<<chisqcrit/dof;
    cout<<" \n --------------------- \n INTEGRALE: "<<pears->Integral(chisqcrit,rangemax);
    cout<<" \n --------------------- \n ";


}