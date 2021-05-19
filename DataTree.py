# -*- coding: utf-8 -*-
#LIBRERIE
import os.path
import sys  
import ctypes
import ROOT
from ROOT import TFile, TTree, gROOT, addressof

#MAIN
cwd = os.getcwd()+'/' #Current Working Directory
filename = 'pbard.dat' #nome del file dati
filename = cwd + filename #inserisce il path
error = 'Il file ' + filename + ' non esiste'
if (not os.path.isfile(filename)): exit(error) #esce se non trova il file

ncol = 14 #numero di colonne nel file da leggere

#Bisogna allocare memoria con C++ -> Definisco solo strutture 
gROOT.ProcessLine(
    "typedef struct {\
        double pmed;\
        double pmin;\
        double pmax;\
    } p;\
    typedef struct {\
        double sig;\
        double statpos;\
        double statneg;\
        double sys;\
    } sigma;\
    "    
)

#Creo oggetti su python
evid = ctypes.c_int() #per interi o double singoli basta chiamare ctype
plab = ROOT.p()
xs = ROOT.sigma()

f = TFile('hfile.root','recreate') #file su cui vado a scrivere il tree
tree = TTree('T','tree con dati pbard') #dichiarazione del tree

#dichiarazione branch in C++
#tree->Branch("nome branch",&oggetto) dove & prende l'indirizzo
#tree->Branch("nome branch",&nomestruct.primoelemento,"elementodouble/D:elementointero/I:elementochar/C")

#dichiarazione branch in python
tree.Branch('eventID',addressof(evid),'evid/I')
tree.Branch('plab[GeV/c]',addressof(plab,'pmed'),'pmed/D:pmin/D:pmax/D')
tree.Branch('CrossSection[mb]',addressof(xs,'sig'),'sig/D:statpos/D:statneg/D:sys/D')

#leggiamo il file
for line in open(filename,'r'):
    words = line.split()
    if (len(words)!=ncol): continue
    else:
        evid = ctypes.c_int(int(words[0])) #per chiamare addressof serve un ctype che però richiede un intero per la conversione
        plab.pmed = float(words[1])
        plab.pmin = float(words[2])
        plab.pmax = float(words[3])
        xs.sig = float(words[4])
        xs.statpos = float(words[5])
        xs.statneg = float(words[6])
        xs.sys = float(words[7])
    tree.Fill() #fill evento
#fine lettura file

tree.Write() #scriviamo il tree nel file