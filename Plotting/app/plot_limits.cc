//C++
#include <iostream>
#include <fstream>
#include <map>
#include <stdlib.h>
#include <utility>
//ROOT
#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TLegend.h>
//LOCAL INCLUDES
#include "CommandLineInput.hh"

struct Limit
{
  double obs;
  double exp0p025;
  double exp0p16;
  double exp0p5;
  double exp0p84;
  double exp0p975;
};

const unsigned MAX_POINTS=1000;
bool plotLHCb = false;

const float lumi = 5;
const float minLHCb = 5e-13;//5e-8;
const float min = 5e-8;
const float max = 0.1;

//exp/obs limit line color
Color_t Color_lim_pi0   = kBlue+2;
Color_t Color_lim_pihad = kRed+2;
Color_t Color_lhcb_0p5 = kGreen+2;
Color_t Color_lhcb_1p0 = kOrange+2;

//Margins
const float leftMargin   = 0.12;
const float rightMargin  = 0.05;
const float topMargin    = 0.07;
const float bottomMargin = 0.10;

//CMS STANDARD
TString extraText   = "Preliminary";
TString lumiText = "41.6 fb^{-1} (13 TeV)";

bool AddCMS( TCanvas* C )
{
  C->cd();
  float lumix = 0.955;
  float lumiy = 0.945;
  float lumifont = 42;

  float cmsx = 0.25;
  float cmsy = 0.94;
  float cmsTextFont   = 61;  // default is helvetic-bold
  float extrax = cmsx +0.20;
  float extray = cmsy;
  float extraTextFont = 52;  // default is helvetica-italics
  // ratio of "CMS" and extra text size
  float extraOverCmsTextSize  = 0.76;
  float cmsSize = 0.06;
  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);
  float extraTextSize = extraOverCmsTextSize*cmsSize;
  latex.SetTextFont(lumifont);
  latex.SetTextAlign(31);
  latex.SetTextSize(cmsSize);
  latex.DrawLatex(lumix, lumiy,lumiText);

  latex.SetTextFont(cmsTextFont);
  latex.SetTextAlign(31);
  latex.SetTextSize(cmsSize);
  latex.DrawLatex(cmsx, cmsy, "CMS");

  latex.SetTextFont(extraTextFont);
  latex.SetTextAlign(31);
  latex.SetTextSize(extraTextSize);
  latex.DrawLatex(extrax, extray, extraText);
  return true;
};

void setLineQualities (TGraph* g, float minimum, Color_t c, TString type, int style){
  g->SetMaximum(max);
  g->SetMinimum(minimum); 
  g->SetLineColor(c);
  g->SetTitle("");
  g->SetTitle(";#Phi proper decay length [mm];95 % CL Upper Limit on BR(B #rightarrow K#Phi);");

  if(type == "exp"){
    g->SetLineStyle(style);
    g->SetLineWidth(3);
  }
  if(type == "obs"){
    g->SetMarkerStyle(31);
    g->SetMarkerColor(c);
    g->SetMarkerSize(1);
  }
  if( type == "band"){
    g->SetFillColorAlpha(c, 0.15);
    g->SetLineWidth(2);
  }
}

void setOffsets(TGraph* g){
  g->GetXaxis()->SetTitleOffset(1.3);
  g->GetYaxis()->SetTitleOffset(0);
}

void setLegendQualities(TLegend* l, int nColumns){
  l->SetBorderSize(0);
  l->SetLineColor(1);
  l->SetLineStyle(1);
  l->SetLineWidth(1);
  l->SetFillColor(0);
  l->SetFillStyle(1001);
  l->SetTextSize(0.03);
//  if(plotLHCb)l->SetTextSize(0.01);
  l->SetNColumns(nColumns);

}

void readData(const char* fileName, float x[], float y[], int& nPoints) {
    std::cout<<"Reading LHCb data"<<std::endl;
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << fileName << std::endl;
        return;
    }

    nPoints = 0;
    while (nPoints < MAX_POINTS && file >> x[nPoints] >> y[nPoints]) {
        //x-coords are in meters in file
        x[nPoints] = 1000.*x[nPoints];
        std::cout<< x[nPoints]<<"   "<<y[nPoints]<<std::endl;
        nPoints++;
    }

    file.close();
}

void setCanvas(TCanvas* c){

  c->SetHighLightColor(2);
  c->SetFillColor(0);
  c->SetBorderMode(0);
  c->SetBorderSize(2);
  c->SetLeftMargin( leftMargin );
  c->SetRightMargin( rightMargin );
  c->SetTopMargin( topMargin );
  c->SetBottomMargin( bottomMargin );
  c->SetFrameBorderMode(0);
  c->SetFrameBorderMode(0);
  c->SetLogy();
  c->SetLogx();

  gStyle->SetPaintTextFormat("4.3f");
}

int main( int argc, char** argv )
{
float minimum = min;
if(plotLHCb) minimum = minLHCb;
TString s_region="";
  //-----------------
  //Input File List
  //-----------------
  std::string inputList = ParseCommandLine( argc, argv, "-inputList=" );
  if (  inputList == "" )
    {
      std::cerr << "[ERROR]: please provide an inputList. Use --inputList=" << std::endl;
      return -1;
    }
  std::ifstream ifs ( inputList.c_str(), std::ifstream::in );//input file list


  std::map<float, Limit> mymap_pi0;
  std::map<float, Limit> mymap_pihad;

  std::string fname;
  TString s_fname;
  double limitSF;
  if( ifs.is_open() )
    {
      while( ifs.good() )
	{
	  ifs >> fname;
          s_fname=fname;
	  if ( ifs.eof() ) break;
	  TFile* fin = new TFile( fname.c_str(), "READ" );
	  if ( fin->IsZombie() ) continue;

          int low = fname.find("ctau");
	  int high = fname.find("/higgs");
	  std::string ctau = fname.substr( low+4, high-low-4 );
	  float _ctau = atof( ctau.c_str() );

	  TTree* tree = (TTree*)fin->Get("limit");
	  double limit;
	  Limit tmpLimit;
	  limitSF = 0.00001;
          std::cout<<s_fname<< "    "<<ctau<<"   "<<_ctau<<std::endl;
          //if (s_fname.Contains("SigPi0") && _ctau == 10000.0f) limitSF = 0.00001*1000.;
          //if (s_fname.Contains("SigPi0") && _ctau == 3.0f)     limitSF = 0.00001*100000.;
          //if (s_fname.Contains("SigPi0") && _ctau == 5.0f)     limitSF = 0.00001*1000.;
          //if (s_fname.Contains("SigPi0") && _ctau == 7.0f)     limitSF = 0.00001*100.;
	  tree->SetBranchAddress( "limit", &limit );
	  tree->GetEntry(0);
	  tmpLimit.exp0p025 = limit*limitSF;
	  tree->GetEntry(1);
	  tmpLimit.exp0p16 = limit*limitSF;
	  tree->GetEntry(2);
	  tmpLimit.exp0p5 = limit*limitSF;
	  tree->GetEntry(3);
	  tmpLimit.exp0p84 = limit*limitSF;
	  tree->GetEntry(4);
	  tmpLimit.exp0p975 = limit*limitSF;
	  //tree->GetEntry(5);
	  tmpLimit.obs = tmpLimit.exp0p5;

	  //std::cout << "ctau: " << ctau << "-> " << tmpLimit.exp0p025 << " " << tmpLimit.exp0p16 << " " << tmpLimit.exp0p5 << " " << tmpLimit.exp0p84<< " " 
          //          << tmpLimit.exp0p975 << " " << tmpLimit.obs << "   SF="<<limitSF <<std::endl;

          if (s_fname.Contains("SigPi0")){
	    if ( mymap_pi0.find( _ctau ) == mymap_pi0.end() )
	      {
	        mymap_pi0[_ctau] = tmpLimit;
	      }
          }
          if (s_fname.Contains("SigPiPlusPiMinus")){
	    if ( mymap_pihad.find( _ctau ) == mymap_pihad.end() )
	      {
	        mymap_pihad[_ctau] = tmpLimit;
	      }
          }
          if      (s_fname.Contains("csc")) s_region="CSC";
          else if (s_fname.Contains("dt"))  s_region="DT";
          else                              s_region="CSC+DT Combined";
	  delete fin;
	}
    }
  else
    {
      std::cerr << "[ERROR] can't open file " << inputList << std::endl;
    }
  std::cout<<"Pi0"<<std::endl;
  for ( auto tmp : mymap_pi0 ){std::cout << "cTau: " << tmp.first << " " << tmp.second.exp0p5 << std::endl;}
  std::cout<<"Pihad"<<std::endl;
  for ( auto tmp : mymap_pihad ){std::cout << "cTau: " << tmp.first << " " << tmp.second.exp0p5 << std::endl;}
  std::cout<<"end curve check"<<std::endl;
  TFile* out   = new TFile("out_test.root", "recreate");

//begin curves
//-----Get LHCB curve
  const char* lhcb_0p5 = "/uscms/home/ddiaz/nobackup/BParkingLLPs/CMSSW_9_4_4/src/Stats/CMSSW_10_2_13/src/ABCD/Plotting/data/lhcb_bkmumu_0p5gev.txt";
  const char* lhcb_1p0 = "/uscms/home/ddiaz/nobackup/BParkingLLPs/CMSSW_9_4_4/src/Stats/CMSSW_10_2_13/src/ABCD/Plotting/data/lhcb_bkmumu_1gev.txt";

  float xValues_lhcb_0p5[MAX_POINTS];
  float yValues_lhcb_0p5[MAX_POINTS];
  float xValues_lhcb_1p0[MAX_POINTS];
  float yValues_lhcb_1p0[MAX_POINTS];

  int nPoints_lhcb_0p5;
  int nPoints_lhcb_1p0;

  readData(lhcb_0p5, xValues_lhcb_0p5, yValues_lhcb_0p5, nPoints_lhcb_0p5);
  readData(lhcb_1p0, xValues_lhcb_1p0, yValues_lhcb_1p0, nPoints_lhcb_1p0);
  // Create TGraph
  TGraph* graph_lhcb_0p5 = new TGraph(nPoints_lhcb_0p5, xValues_lhcb_0p5, yValues_lhcb_0p5);
  TGraph* graph_lhcb_1p0 = new TGraph(nPoints_lhcb_1p0, xValues_lhcb_1p0, yValues_lhcb_1p0);
  //format LHCb
  graph_lhcb_0p5        ->SetLineStyle(9);
  graph_lhcb_0p5        ->SetLineColor(Color_lhcb_0p5);
  graph_lhcb_0p5        ->SetLineWidth(3);
  graph_lhcb_0p5        ->SetTitle("");
  graph_lhcb_0p5        ->SetMarkerStyle(31);

  graph_lhcb_1p0        ->SetLineStyle(9);
  graph_lhcb_1p0        ->SetLineColor(Color_lhcb_1p0);
  graph_lhcb_1p0        ->SetLineWidth(3);
  graph_lhcb_1p0        ->SetTitle("");
  graph_lhcb_1p0        ->SetMarkerStyle(31);

//--------------Pi0
  int npoints_pi0 = mymap_pi0.size();
  float x_pi0[npoints_pi0];
  float expL_pi0[npoints_pi0];
  float obsL_pi0[npoints_pi0];

  float xp_pi0[2*npoints_pi0];
  float OneS_pi0[2*npoints_pi0];
  //float TwoS_pi0[2*npoints_pi0];

  int ctr = 0;
  for ( auto tmp : mymap_pi0 ){
      x_pi0[ctr]      = tmp.first;
      obsL_pi0[ctr]   = tmp.second.obs;
      expL_pi0[ctr]   = tmp.second.exp0p5;

      xp_pi0[ctr] = tmp.first;
      xp_pi0[2*npoints_pi0-(ctr+1)] = tmp.first;

      OneS_pi0[ctr] = tmp.second.exp0p16;
      OneS_pi0[2*npoints_pi0-(ctr+1)] = tmp.second.exp0p84;

      //TwoS_pi0[ctr] = tmp.second.exp0p025;
      //TwoS_pi0[2*npoints_pi0-(ctr+1)] = tmp.second.exp0p975;

      ctr++;
  }


  TGraph* gObs_pi0 = new TGraph(npoints_pi0, x_pi0, obsL_pi0);
  TGraph* gExp_pi0 = new TGraph(npoints_pi0, x_pi0, expL_pi0);

  TGraph* gOneS_pi0 = new TGraph(2*npoints_pi0, xp_pi0, OneS_pi0);
  //formatting
  setLineQualities(gExp_pi0, minimum, Color_lim_pi0, "exp", 7);
  setLineQualities(gOneS_pi0, minimum, Color_lim_pi0, "band", 7);
  setLineQualities(gObs_pi0, minimum, Color_lim_pi0, "obs", 7);
//---------- end Pi0

//--------------Pihad
  int npoints_pihad = mymap_pihad.size();
  float x_pihad[npoints_pihad];
  float expL_pihad[npoints_pihad];
  float obsL_pihad[npoints_pihad];

  float xp_pihad[2*npoints_pihad];
  float OneS_pihad[2*npoints_pihad];
  //float TwoS_pihad[2*npoints_pihad];

  ctr = 0;
  for ( auto tmp : mymap_pihad )
    {
      x_pihad[ctr]      = tmp.first;
      obsL_pihad[ctr]   = tmp.second.obs;
      expL_pihad[ctr]   = tmp.second.exp0p5;

      xp_pihad[ctr] = tmp.first;
      xp_pihad[2*npoints_pihad-(ctr+1)] = tmp.first;

      OneS_pihad[ctr] = tmp.second.exp0p16;
      OneS_pihad[2*npoints_pihad-(ctr+1)] = tmp.second.exp0p84;

      //TwoS_pihad[ctr] = tmp.second.exp0p025;
      //TwoS_pihad[2*npoints_pihad-(ctr+1)] = tmp.second.exp0p975;

      ctr++;
    }


  //TFile* out   = new TFile("out_test.root", "recreate");
  TGraph* gObs_pihad = new TGraph(npoints_pihad, x_pihad, obsL_pihad);
  TGraph* gExp_pihad = new TGraph(npoints_pihad, x_pihad, expL_pihad);

  TGraph* gOneS_pihad = new TGraph(2*npoints_pihad, xp_pihad, OneS_pihad);

  //formatting
  setLineQualities(gExp_pihad, minimum, Color_lim_pihad, "exp", 9);
  setLineQualities(gOneS_pihad, minimum, Color_lim_pihad, "band", 9);
  setLineQualities(gObs_pihad, minimum, Color_lim_pihad, "obs", 9);
//---------- end Pihad
//end lines
  TCanvas* c = new TCanvas( "c", "c", 2119, 33, 800, 700 );
  setCanvas(c);


  //---------Draw
  //gTwoS_pi0->Draw("AF");
  //gObs_pi0->Draw("L");
  gExp_pihad->Draw("AL");
  gExp_pi0->Draw("L");
  gOneS_pi0->Draw("F");
  gOneS_pihad->Draw("F");
//  if(plotLHCb) graph_lhcb_0p5->Draw("L");
//  if(plotLHCb) graph_lhcb_1p0->Draw("L");

  setOffsets(gExp_pihad);
  setOffsets(gOneS_pihad);
  setOffsets(gObs_pihad);
  setOffsets(gExp_pi0);
  setOffsets(gOneS_pi0);
  setOffsets(gObs_pi0);

  TLegend* leg = new TLegend( 0.15, 0.15, 0.5, 0.35, NULL, "brNDC" );
  setLegendQualities(leg,1);
 
  TLegend* leg2 = new TLegend( 0.5, 0.15, 0.925, 0.35, NULL, "brNDC" );
  setLegendQualities(leg2,1);
  
  leg->AddEntry( gExp_pi0,   "#Phi #rightarrow  #Pi^{0}#Pi^{0}", "l" );
  leg->AddEntry( gExp_pihad, "#Phi #rightarrow  #Pi^{+}#Pi^{-}", "l" );
  if(plotLHCb)leg->AddEntry( graph_lhcb_0p5, "LHCb B #rightarrow K(S #rightarrow #mu#mu); m_{S} = 0.5 GeV" , "l" );
  if(plotLHCb)leg->AddEntry( graph_lhcb_1p0, "LHCb B #rightarrow K(S #rightarrow #mu#mu); m_{S} = 1 GeV" , "l" );

  leg2->AddEntry( gOneS_pi0, "#pm 1 s.d. expected", "f" );
  leg2->AddEntry( gOneS_pihad, "#pm 1 s.d. expected", "f" );
  if(plotLHCb) leg2->AddEntry("", "", "");  // Add an empty entry
  if(plotLHCb) leg2->AddEntry("", "", "");  // Add an empty entry

  leg->Draw("SAME");
  leg2->Draw("SAME");

  //Phi Mass label
  TLatex latexMass;
  latexMass.SetNDC();
  latexMass.SetTextSize(0.038);
  latexMass.SetTextFont(42);
  latexMass.DrawLatex(0.15, 0.88, "m_{#Phi} = 0.3 GeV   "+s_region);

  //95% CL label
  float cmsx = 0.93;
  float cmsy = 0.88;
  float cmsSize = 0.04;
  float cmsTextFont = 41;  // default is helvetic-bold
  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);
  latex.SetTextAlign(31);
  latex.SetTextSize(cmsSize);
  latex.SetTextFont(cmsTextFont);
  latex.DrawLatex(cmsx, cmsy, "95% CL upper limits");

  AddCMS(c);

  //c->SetLogx();
  c->SaveAs("Limit_bp.pdf");
  c->SaveAs("Limit_bp.C");

  gObs_pi0->GetXaxis()->SetRangeUser(0, 30);
  gObs_pi0->Write("gObs");
  gExp_pi0->GetXaxis()->SetRangeUser(0, 30);
  gExp_pi0->Write("gExp");
  gOneS_pi0->Write("gOneS");

  out->Close();
  return 0;
}


