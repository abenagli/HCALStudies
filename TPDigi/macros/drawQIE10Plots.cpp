// g++ -Wall -o drawQIE10Plots.exe `root-config --cflags --glibs` setTDRStyle.cc drawQIE10Plots.cpp

#include "setTDRStyle.h"

#include <iostream>

#include "TROOT.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TProfile.h"



int main(int argc, char** argv)
{
  setTDRStyle();
  
  int run = atoi(argv[1]);
  
  TFile* inFile = TFile::Open(Form("../test/QIE10Ntuple_%d.root",run));
  
  TH1F* h1;
  TH1F* h2;
  TProfile* p1;
  TCanvas* c1;
  
  TF1* f_bis = new TF1("f_bis","x",0.,1000.);
  f_bis -> SetLineColor(kRed);
  
  std::string baseDir(Form("/afs/cern.ch/user/a/abenagli/www/HCAL_QIE10/run%d",run));
  
  gROOT->ProcessLine(Form(".! rm -rf %s",baseDir.c_str()));
  gROOT->ProcessLine(Form(".! mkdir %s",baseDir.c_str()));
  gROOT->ProcessLine(Form(".! cp index.php %s/",baseDir.c_str()));
  
  
  
  c1 = new TCanvas("c1","p1_shape_10",700,600);
  p1 = (TProfile*)( inFile->Get("qie10Analysis/p1_shape_10") );
  p1 -> Draw("P");
  p1 -> SetTitle(";TS;#LT ADC #GT");
  c1 -> Print(Form("%s/shape_10.png",baseDir.c_str()));
  delete c1;
  
  
  c1 = new TCanvas("c1","p1_shape_8",700,600);
  p1 = (TProfile*)( inFile->Get("qie10Analysis/p1_shape_8") );
  p1 -> Draw("P");
  p1 -> SetTitle(";TS;#LT ADC #GT");
  c1 -> Print(Form("%s/shape_8.png",baseDir.c_str()));
  delete c1;
  
  
  c1 = new TCanvas("c1","h1_ADC_10",700,600);
  h1 = (TH1F*)( inFile->Get("qie10Analysis/h1_ADC_10") );
  h1 -> Draw();
  c1 -> SetLogy();
  h1 -> SetTitle(";QIE10 ADC;entries");
  c1 -> Print(Form("%s/ADC_10.png",baseDir.c_str()));
  delete c1;
  
  
  c1 = new TCanvas("c1","h1_ADC_8",700,600);
  h1 = (TH1F*)( inFile->Get("qie10Analysis/h1_ADC_8") );
  h1 -> Draw();
  c1 -> SetLogy();
  h1 -> SetTitle(";QIE8 ADC;entries");
  c1 -> Print(Form("%s/ADC_8.png",baseDir.c_str()));
  delete c1;
  
  
  c1 = new TCanvas("c1","h1_maxADC_10",700,600);
  h1 = (TH1F*)( inFile->Get("qie10Analysis/h1_maxADC_10") );
  h1 -> Draw();
  c1 -> SetLogy();
  h1 -> SetTitle(";max QIE10 ADC;entries");
  c1 -> Print(Form("%s/maxADC_10.png",baseDir.c_str()));
  delete c1;
  
  
  c1 = new TCanvas("c1","h1_maxADC_8",700,600);
  h1 = (TH1F*)( inFile->Get("qie10Analysis/h1_maxADC_8") );
  h1 -> Draw();
  c1 -> SetLogy();
  h1 -> SetTitle(";max QIE8 ADC;entries");
  c1 -> Print(Form("%s/maxADC_8.png",baseDir.c_str()));
  delete c1;
  
  
  c1 = new TCanvas("c1","h1_LETDC",700,600);
  h1 = (TH1F*)( inFile->Get("qie10Analysis/h1_LETDC") );
  h1 -> Draw();
  c1 -> SetLogy();
  h1 -> SetTitle(";QIE10 LE TDC;entries");
  c1 -> Print(Form("%s/LETDC.png",baseDir.c_str()));
  delete c1;
  
  
  c1 = new TCanvas("c1","h1_LETDC_vs_ADC",700,600);
  h2 = (TH1F*)( inFile->Get("qie10Analysis/h2_LETDC_vs_ADC") );
  h2 -> Draw("COLZ");
  c1 -> SetLogz();
  h2 -> SetTitle(";QIE10 ADC;QIE10 LE TDC");
  c1 -> Print(Form("%s/LETDC_vs_ADC.png",baseDir.c_str()));
  delete c1;
  
  
  c1 = new TCanvas("c1","h2_ADCSumCorr_10_vs_8",700,600);
  h2 = (TH1F*)( inFile->Get("qie10Analysis/h2_ADCSumCorr_10_vs_8") );
  h2 -> Draw("COLZ");
  c1 -> SetLogz();
  h2 -> SetTitle(";QIE8 ADC;QIE10 ADC");
  f_bis -> Draw("same");
  c1 -> Print(Form("%s/h2_ADCSumCorr_10_vs_8.png",baseDir.c_str()));
  delete c1;
  
  
  c1 = new TCanvas("c1","h2_ADCCorr_10_vs_8",700,600);
  h2 = (TH1F*)( inFile->Get("qie10Analysis/h2_ADCCorr_10_vs_8") );
  h2 -> Draw("COLZ");
  c1 -> SetLogz();
  h2 -> SetTitle(";QIE8 ADC;QIE10 ADC");
  f_bis -> Draw("same");
  c1 -> Print(Form("%s/h2_ADCCorr_10_vs_8.png",baseDir.c_str()));
  delete c1;
  

  c1 = new TCanvas("c1","h2_ADCCorr_10_vs_10",700,600);
  h2 = (TH1F*)( inFile->Get("qie10Analysis/h2_ADCCorr_10_vs_10") );
  h2 -> Draw("COLZ");
  c1 -> SetLogz();
  h2 -> SetTitle(";QIE10 ADC;QIE10 ADC");
  f_bis -> Draw("same");
  c1 -> Print(Form("%s/h2_ADCCorr_10_vs_10.png",baseDir.c_str()));
  delete c1;
  
  
  c1 = new TCanvas("c1","h2_LETDCCorr_10_vs_10",700,600);
  h2 = (TH1F*)( inFile->Get("qie10Analysis/h2_LETDCCorr_10_vs_10") );
  h2 -> Draw("COLZ");
  c1 -> SetLogz();
  h2 -> SetTitle(";QIE10 LE TDC;QIE10 LE TDC");
  f_bis -> Draw("same");
  c1 -> Print(Form("%s/h2_LETDCCorr_10_vs_10.png",baseDir.c_str()));
  delete c1;
  
  
  
  for(int TSIt = 0; TSIt < 10; ++TSIt)
  {
    std::string TSDir(Form("%s/TS%02d",baseDir.c_str(),TSIt));
    gROOT->ProcessLine(Form(".! mkdir %s",TSDir.c_str()));
    gROOT->ProcessLine(Form(".! cp index.php %s/",TSDir.c_str()));
    
    c1 = new TCanvas("c1",Form("h1_ADC_10_TS%02d",TSIt),700,600);
    h1 = (TH1F*)( inFile->Get(Form("qie10Analysis/h1_ADC_10_TS%02d",TSIt)) );
    h1 -> Draw();
    c1 -> SetLogy();
    h1 -> SetTitle(";QIE10 ADC;entries");
    c1 -> Print(Form("%s/ADC_10_TS%02d.png",TSDir.c_str(),TSIt));
    delete c1;
    
    
    c1 = new TCanvas("c1",Form("h1_ADC_8_TS%02d",TSIt),700,600);
    h1 = (TH1F*)( inFile->Get(Form("qie10Analysis/h1_ADC_8_TS%02d",TSIt)) );
    h1 -> Draw();
    c1 -> SetLogy();
    h1 -> SetTitle(";QIE8 ADC;entries");
    c1 -> Print(Form("%s/ADC_8_TS%02d.png",TSDir.c_str(),TSIt));
    delete c1;
    
    
    c1 = new TCanvas("c1",Form("h1_LETDC_TS%02d",TSIt),700,600);
    h1 = (TH1F*)( inFile->Get(Form("qie10Analysis/h1_LETDC_TS%02d",TSIt)) );
    h1 -> Draw();
    c1 -> SetLogy();
    h1 -> SetTitle(";QIE10 LE TDC;entries");
    c1 -> Print(Form("%s/LETDC_TS%02d.png",TSDir.c_str(),TSIt));
    delete c1;
    
    
    c1 = new TCanvas("c1",Form("h1_LETDC_vs_ADC_TS%02d",TSIt),700,600);
    h2 = (TH1F*)( inFile->Get(Form("qie10Analysis/h2_LETDC_vs_ADC_TS%02d",TSIt)) );
    h2 -> Draw("COLZ");
    c1 -> SetLogz();
    h2 -> SetTitle(";QIE10 ADC;QIE10 LE TDC");
    c1 -> Print(Form("%s/LETDC_vs_ADC_TS%02d.png",TSDir.c_str(),TSIt));
    delete c1;
    
    
    c1 = new TCanvas("c1",Form("h2_ADCCorr_10_vs_8_TS%02d",TSIt),700,600);
    h2 = (TH1F*)( inFile->Get(Form("qie10Analysis/h2_ADCCorr_10_vs_8_TS%02d",TSIt)) );
    h2 -> Draw("COLZ");
    c1 -> SetLogz();
    h2 -> SetTitle(";QIE8 ADC;QIE10 ADC");
    f_bis -> Draw("same");
    c1 -> Print(Form("%s/h2_ADCCorr_10_vs_8_TS%02d.png",TSDir.c_str(),TSIt));
    delete c1;
    
    
    c1 = new TCanvas("c1",Form("h2_ADCCorr_10_vs_10_TS%02d",TSIt),700,600);
    h2 = (TH1F*)( inFile->Get(Form("qie10Analysis/h2_ADCCorr_10_vs_10_TS%02d",TSIt)) );
    h2 -> Draw("COLZ");
    c1 -> SetLogz();
    h2 -> SetTitle(";QIE10 ADC;QIE10 ADC");
    f_bis -> Draw("same");
    c1 -> Print(Form("%s/h2_ADCCorr_10_vs_10_TS%02d.png",TSDir.c_str(),TSIt));
    delete c1;
    
    
    c1 = new TCanvas("c1",Form("h2_LETDCCorr_10_vs_10_TS%02d",TSIt),700,600);
    h2 = (TH1F*)( inFile->Get(Form("qie10Analysis/h2_LETDCCorr_10_vs_10_TS%02d",TSIt)) );
    h2 -> Draw("COLZ");
    c1 -> SetLogz();
    h2 -> SetTitle(";QIE10 LE TDC;QIE10 LE TDC");
    f_bis -> Draw("same");
    c1 -> Print(Form("%s/h2_LETDCCorr_10_vs_10_TS%02d.png",TSDir.c_str(),TSIt));
    delete c1;
  }
  
  
  
  
  
  
  for(int depth = 3; depth <= 4; ++depth)
    for(int ieta = 29; ieta <= 41; ++ieta)
    {
      std::string label(Form("%02d-%02d_%01d",ieta,39,depth));
      
      std::string channelDir(Form("%s/%s",baseDir.c_str(),label.c_str()));
      gROOT->ProcessLine(Form(".! mkdir %s",channelDir.c_str()));
      gROOT->ProcessLine(Form(".! cp index.php %s/",channelDir.c_str()));
      
      
      c1 = new TCanvas("c1",Form("p1_shape_10_%s",label.c_str()),700,600);
      p1 = (TProfile*)( inFile->Get(Form("qie10Analysis/p1_shape_10_%s",label.c_str())) );
      p1 -> Draw("P");
      p1 -> SetTitle(";TS;#LT ADC #GT");
      c1 -> Print(Form("%s/shape_10.png",channelDir.c_str()));
      delete c1;
      
      
      c1 = new TCanvas("c1",Form("p1_shape_8_%s",label.c_str()),700,600);
      p1 = (TProfile*)( inFile->Get(Form("qie10Analysis/p1_shape_8_%s",label.c_str())) );
      p1 -> Draw("P");
      p1 -> SetTitle(";TS;#LT ADC #GT");
      c1 -> Print(Form("%s/shape_8.png",channelDir.c_str()));
      delete c1;
      
      
      c1 = new TCanvas("c1",Form("h1_ADC_10_%s",label.c_str()),700,600);
      h1 = (TH1F*)( inFile->Get(Form("qie10Analysis/h1_ADC_10_%s",label.c_str())) );
      h1 -> Draw();
      c1 -> SetLogy();
      h1 -> SetTitle(";QIE10 ADC;entries");
      c1 -> Print(Form("%s/ADC_10.png",channelDir.c_str()));
      delete c1;
      
      
      c1 = new TCanvas("c1",Form("h1_ADC_8_%s",label.c_str()),700,600);
      h1 = (TH1F*)( inFile->Get(Form("qie10Analysis/h1_ADC_8_%s",label.c_str())) );
      h1 -> Draw();
      c1 -> SetLogy();
      h1 -> SetTitle(";QIE8 ADC;entries");
      c1 -> Print(Form("%s/ADC_8.png",channelDir.c_str()));
      delete c1;
      
      
      c1 = new TCanvas("c1",Form("h1_maxADC_10_%s",label.c_str()),700,600);
      h1 = (TH1F*)( inFile->Get(Form("qie10Analysis/h1_maxADC_10_%s",label.c_str())) );
      h1 -> Draw();
      c1 -> SetLogy();
      h1 -> SetTitle(";max QIE10 ADC;entries");
      c1 -> Print(Form("%s/maxADC_10.png",channelDir.c_str()));
      delete c1;
      
      
      c1 = new TCanvas("c1",Form("h1_maxADC_8_%s",label.c_str()),700,600);
      h1 = (TH1F*)( inFile->Get(Form("qie10Analysis/h1_maxADC_8_%s",label.c_str())) );
      h1 -> Draw();
      c1 -> SetLogy();
      h1 -> SetTitle(";max QIE8 ADC;entries");
      c1 -> Print(Form("%s/maxADC_8.png",channelDir.c_str()));
      delete c1;
      
      
      c1 = new TCanvas("c1",Form("h1_LETDC_%s",label.c_str()),700,600);
      h1 = (TH1F*)( inFile->Get(Form("qie10Analysis/h1_LETDC_%s",label.c_str())) );
      h1 -> Draw();
      c1 -> SetLogy();
      h1 -> SetTitle(";QIE10 LE TDC;entries");
      c1 -> Print(Form("%s/LETDC.png",channelDir.c_str()));
      delete c1;
      
      
      c1 = new TCanvas("c1",Form("h1_LETDC_vs_ADC_%s",label.c_str()),700,600);
      h2 = (TH1F*)( inFile->Get(Form("qie10Analysis/h2_LETDC_vs_ADC_%s",label.c_str())) );
      h2 -> Draw("COLZ");
      c1 -> SetLogz();
      h2 -> SetTitle(";QIE10 ADC;QIE10 LE TDC");
      c1 -> Print(Form("%s/LETDC_vs_ADC.png",channelDir.c_str()));
      delete c1;
      
      
      c1 = new TCanvas("c1",Form("h2_ADCSumCorr_10_vs_8_%s",label.c_str()),700,600);
      h2 = (TH1F*)( inFile->Get(Form("qie10Analysis/h2_ADCSumCorr_10_vs_8_%s",label.c_str())) );
      h2 -> Draw("COLZ");
      c1 -> SetLogz();
      h2 -> SetTitle(";QIE8 ADC;QIE10 ADC");
      f_bis -> Draw("same");
      c1 -> Print(Form("%s/h2_ADCSumCorr_10_vs_8.png",channelDir.c_str()));
      delete c1;
      
      
      c1 = new TCanvas("c1",Form("h2_ADCCorr_10_vs_8_%s",label.c_str()),700,600);
      h2 = (TH1F*)( inFile->Get(Form("qie10Analysis/h2_ADCCorr_10_vs_8_%s",label.c_str())) );
      h2 -> Draw("COLZ");
      c1 -> SetLogz();
      h2 -> SetTitle(";QIE8 ADC;QIE10 ADC");
      f_bis -> Draw("same");
      c1 -> Print(Form("%s/h2_ADCCorr_10_vs_8.png",channelDir.c_str()));
      delete c1;
      
      
      c1 = new TCanvas("c1",Form("h2_ADCCorr_10_vs_10_%s",label.c_str()),700,600);
      h2 = (TH1F*)( inFile->Get(Form("qie10Analysis/h2_ADCCorr_10_vs_10_%s",label.c_str())) );
      h2 -> Draw("COLZ");
      c1 -> SetLogz();
      h2 -> SetTitle(";QIE10 ADC;QIE10 ADC");
      f_bis -> Draw("same");
      c1 -> Print(Form("%s/h2_ADCCorr_10_vs_10.png",channelDir.c_str()));
      delete c1;
      
      
      c1 = new TCanvas("c1",Form("h2_LETDCCorr_10_vs_10_%s",label.c_str()),700,600);
      h2 = (TH1F*)( inFile->Get(Form("qie10Analysis/h2_LETDCCorr_10_vs_10_%s",label.c_str())) );
      h2 -> Draw("COLZ");
      c1 -> SetLogz();
      h2 -> SetTitle(";QIE10 LE TDC;QIE10 LE TDC");
      f_bis -> Draw("same");
      c1 -> Print(Form("%s/h2_LETDCCorr_10_vs_10.png",channelDir.c_str()));
      delete c1;
      
      
      
      for(int TSIt = 0; TSIt < 10; ++TSIt)
      {
        std::string TSChannelDir(Form("%s/TS%02d",channelDir.c_str(),TSIt));
        gROOT->ProcessLine(Form(".! mkdir %s",TSChannelDir.c_str()));
        gROOT->ProcessLine(Form(".! cp index.php %s/",TSChannelDir.c_str()));
        
        c1 = new TCanvas("c1",Form("h1_ADC_10_%s_TS%02d",label.c_str(),TSIt),700,600);
        h1 = (TH1F*)( inFile->Get(Form("qie10Analysis/h1_ADC_10_%s_TS%02d",label.c_str(),TSIt)) );
        h1 -> Draw();
        c1 -> SetLogy();
        h1 -> SetTitle(";QIE10 ADC;entries");
        c1 -> Print(Form("%s/ADC_10_TS%02d.png",TSChannelDir.c_str(),TSIt));
        delete c1;
        
        
        c1 = new TCanvas("c1",Form("h1_ADC_8_%s_TS%02d",label.c_str(),TSIt),700,600);
        h1 = (TH1F*)( inFile->Get(Form("qie10Analysis/h1_ADC_8_%s_TS%02d",label.c_str(),TSIt)) );
        h1 -> Draw();
        c1 -> SetLogy();
        h1 -> SetTitle(";QIE8 ADC;entries");
        c1 -> Print(Form("%s/ADC_8_TS%02d.png",TSChannelDir.c_str(),TSIt));
        delete c1;
        
        
        c1 = new TCanvas("c1",Form("h1_LETDC_%s_TS%02d",label.c_str(),TSIt),700,600);
        h1 = (TH1F*)( inFile->Get(Form("qie10Analysis/h1_LETDC_%s_TS%02d",label.c_str(),TSIt)) );
        h1 -> Draw();
        c1 -> SetLogy();
        h1 -> SetTitle(";QIE10 LE TDC;entries");
        c1 -> Print(Form("%s/LETDC_TS%02d.png",TSChannelDir.c_str(),TSIt));
        delete c1;
        
        
        c1 = new TCanvas("c1",Form("h1_LETDC_vs_ADC_%s_TS%02d",label.c_str(),TSIt),700,600);
        h2 = (TH1F*)( inFile->Get(Form("qie10Analysis/h2_LETDC_vs_ADC_%s_TS%02d",label.c_str(),TSIt)) );
        h2 -> Draw("COLZ");
        c1 -> SetLogz();
        h2 -> SetTitle(";QIE10 ADC;QIE10 LE TDC");
        c1 -> Print(Form("%s/LETDC_vs_ADC_TS%02d.png",TSChannelDir.c_str(),TSIt));
        delete c1;
        
        
        c1 = new TCanvas("c1",Form("h2_ADCCorr_10_vs_8_%s_TS%02d",label.c_str(),TSIt),700,600);
        h2 = (TH1F*)( inFile->Get(Form("qie10Analysis/h2_ADCCorr_10_vs_8_%s_TS%02d",label.c_str(),TSIt)) );
        h2 -> Draw("COLZ");
        c1 -> SetLogz();
        h2 -> SetTitle(";QIE8 ADC;QIE10 ADC");
        f_bis -> Draw("same");
        c1 -> Print(Form("%s/h2_ADCCorr_10_vs_8_TS%02d.png",TSChannelDir.c_str(),TSIt));
        delete c1;
        
        
        c1 = new TCanvas("c1",Form("h2_ADCCorr_10_vs_10_%s_TS%02d",label.c_str(),TSIt),700,600);
        h2 = (TH1F*)( inFile->Get(Form("qie10Analysis/h2_ADCCorr_10_vs_10_%s_TS%02d",label.c_str(),TSIt)) );
        h2 -> Draw("COLZ");
        c1 -> SetLogz();
        h2 -> SetTitle(";QIE10 ADC;QIE10 ADC");
        f_bis -> Draw("same");
        c1 -> Print(Form("%s/h2_ADCCorr_10_vs_10_TS%02d.png",TSChannelDir.c_str(),TSIt));
        delete c1;
        
        
        c1 = new TCanvas("c1",Form("h2_LETDCCorr_10_vs_10_%s_TS%02d",label.c_str(),TSIt),700,600);
        h2 = (TH1F*)( inFile->Get(Form("qie10Analysis/h2_LETDCCorr_10_vs_10_%s_TS%02d",label.c_str(),TSIt)) );
        h2 -> Draw("COLZ");
        c1 -> SetLogz();
        h2 -> SetTitle(";QIE10 LE TDC;QIE10 LE TDC");
        f_bis -> Draw("same");
        c1 -> Print(Form("%s/h2_LETDCCorr_10_vs_10_TS%02d.png",TSChannelDir.c_str(),TSIt));
        delete c1;
      }    
    }
}
