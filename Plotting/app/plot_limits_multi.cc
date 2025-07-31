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

//const float lumi = 5;
const float minLHCb = 5e-13;//5e-8;
const float min = 5e-6;
const float max = 1000;

//exp/obs limit line color
Color_t Color_lim_pihad_0p3 = kBlue+2;
Color_t Color_lim_pihad_0p5 = kBlack; //kViolet+2;
Color_t Color_lim_pihad_1p0 = kGreen+2;
Color_t Color_lim_pihad_2p0 = kOrange+2;
Color_t Color_lim_pihad_3p0 = kRed+2;
Color_t Color_lhcb_0p5 = kGreen+2;
Color_t Color_lhcb_1p0 = kOrange+2;

//Margins
const float leftMargin   = 0.15;//0.12;
const float rightMargin  = 0.05;
const float topMargin    = 0.07;
const float bottomMargin = 0.15;//0.10;

//CMS STANDARD
//TString extraText   = "Preliminary";
TString extraText   = "";
//TString lumiText = "41.6 fb^{-1} (13 TeV)";
TString lumiText = "41.6 fb^{-1} (13 TeV)";

bool AddCMS( TCanvas* C )
{
  C->cd();
  float lumix = 0.955;
  float lumiy = 0.94;
  float lumifont = 42;

  float cmsx = 0.255;
  float cmsy = 0.94;
  float cmsTextFont   = 61;  // default is helvetic-bold
//  float extrax = cmsx +0.25;
//  float extray = cmsy;
  float extraTextFont = 52;  // default is helvetica-italics
  // ratio of "CMS" and extra text size
//  float extraOverCmsTextSize  = 0.6;
  float cmsSize = 0.055;
  TLatex latex;
  latex.SetNDC();
  latex.SetTextAngle(0);
  latex.SetTextColor(kBlack);
  float extraTextSize = 0.06;//extraOverCmsTextSize*cmsSize;
  latex.SetTextFont(lumifont);
  latex.SetTextAlign(31);
  latex.SetTextSize(0.05);
  latex.DrawLatex(lumix, lumiy,lumiText);

  latex.SetTextFont(cmsTextFont);
  latex.SetTextAlign(31);
  latex.SetTextSize(cmsSize);
  latex.DrawLatex(cmsx, cmsy, "CMS");

  latex.SetTextFont(extraTextFont);
  latex.SetTextAlign(31);
  latex.SetTextSize(0.055);
  latex.DrawLatex(cmsx+0.24, cmsy, extraText);
  return true;
};

float getSF(TString path){
  std::ifstream file(path);
  if (!file.is_open()) {
      std::cerr << "Failed to read SF.\n";
      return -1; 
  }

  TString line;
  double sf_value = 0.0;
  TString sf_tag = "#SF:";

  while (!file.eof()) {
      line.ReadLine(file);
      if (line.BeginsWith(sf_tag)) {
          TString value_str = line(sf_tag.Length(), line.Length());
          value_str = value_str.Strip(TString::kBoth); // Strip whitespace from both ends
          sf_value = value_str.Atof(); // Convert the extracted string to a float
          break;
      }
  }
  file.close();
  return sf_value;
}

void setLineQualities (TGraph* g, float minimum, Color_t c, TString type, int style){
  g->SetMaximum(max);
  g->SetMinimum(minimum); 
  g->SetLineColor(c);
  g->SetTitle("");
  //g->SetTitle(";#Phi proper decay length [mm];95 % CL upper limit on BR(B #rightarrow K#Phi);");
  g->SetTitle(";#Phi proper decay length [mm];Upper limit on #bf{#it{#Beta}}(B #rightarrow K#Phi);");
  g->GetXaxis()->SetTitleSize(0.09);

  if(type == "exp"){
    g->SetLineStyle(style);
    g->SetLineWidth(3);
  }
  if(type == "obs"){
    g->SetMarkerStyle(31);
    g->SetMarkerColor(c);
    g->SetMarkerSize(1);
    g->SetLineWidth(3);
  }
  if( type == "band"){
    g->SetFillColorAlpha(c, 0.15);
    //g->SetFillColor(c);
    g->SetFillStyle(3001);
    g->SetLineWidth(1);
  }
}

void setOffsets(TGraph* g){
  g->GetXaxis()->SetTitleOffset(1.2);
  g->GetYaxis()->SetTitleOffset(1.4);
}

void setLegendQualities(TLegend* l, int nColumns){
  l->SetBorderSize(0);
  l->SetLineColor(1);
  l->SetLineStyle(1);
  l->SetLineWidth(1);
  l->SetFillColor(0);
  l->SetFillStyle(1001);
  l->SetTextSize(0.055);
  l->SetMargin(0.15); 
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
        //std::cout<< x[nPoints]<<"   "<<y[nPoints]<<std::endl;
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
  //gPad->SetBottomMargin(0.12);
  gPad->SetTickx(1);
  gPad->SetTicky(1);
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
  TString s_mass = ParseCommandLine( argc, argv, "-m=" );
  std::string inputList = ParseCommandLine( argc, argv, "-inputList=" );
  if (  inputList == "" )
    {
      std::cerr << "[ERROR]: please provide an inputList. Use --inputList=" << std::endl;
      return -1;
    }
  std::ifstream ifs ( inputList.c_str(), std::ifstream::in );//input file list

  std::map<float, Limit> mymap_pihad_0p3;
  std::map<float, Limit> mymap_pihad_0p5;
  std::map<float, Limit> mymap_pihad_1p0;
  std::map<float, Limit> mymap_pihad_2p0;
  std::map<float, Limit> mymap_pihad_3p0;

  size_t lastindex = inputList.find_last_of(".");
  std::string rawName = inputList.substr(0, lastindex);

  std::cout<<"rawName: "<<rawName<<std::endl;

  std::string fname;
  TString s_fname, s_cardPath;
  double limitSF;
  if( ifs.is_open() )
    {
      while( ifs.good() )
	{
	  ifs >> fname;
          s_fname=fname;
          s_cardPath=fname;
          Ssiz_t lastSlash = s_cardPath.Last('/');
          s_cardPath.Replace(lastSlash+1, s_cardPath.Length() - lastSlash - 1, "card.txt");

          
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
	  limitSF = getSF(s_cardPath);//0.000001;
          std::cout<<"SF: "<<limitSF<<" "<< s_fname<< "    "<<"   "<<_ctau<<std::endl;
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
	  tree->GetEntry(5);
	  tmpLimit.obs = limit*limitSF;

	  //std::cout << "ctau: " << ctau << "-> " << tmpLimit.exp0p025 << " " << tmpLimit.exp0p16 << " " << tmpLimit.exp0p5 << " " << tmpLimit.exp0p84<< " " 
          //          << tmpLimit.exp0p975 << " " << tmpLimit.obs << "   SF="<<limitSF <<std::endl;

      if (s_fname.Contains("M0p3")){
	      if ( mymap_pihad_0p3.find( _ctau ) == mymap_pihad_0p3.end() )
	      {
	        mymap_pihad_0p3[_ctau] = tmpLimit;
	      }
      }
      if (s_fname.Contains("M0p5")){
	      if ( mymap_pihad_0p5.find( _ctau ) == mymap_pihad_0p5.end() )
	      {
	        mymap_pihad_0p5[_ctau] = tmpLimit;
	      }
      }
      if (s_fname.Contains("M1p0")){
	      if ( mymap_pihad_1p0.find( _ctau ) == mymap_pihad_1p0.end() )
	      {
	        mymap_pihad_1p0[_ctau] = tmpLimit;
	      }
      }
      if (s_fname.Contains("M2p0")){
	      if ( mymap_pihad_2p0.find( _ctau ) == mymap_pihad_2p0.end() )
	      {
	        mymap_pihad_2p0[_ctau] = tmpLimit;
	      }
      }
      if (s_fname.Contains("M3p0")){
	      if ( mymap_pihad_3p0.find( _ctau ) == mymap_pihad_3p0.end() )
	      {
	        mymap_pihad_3p0[_ctau] = tmpLimit;
	      }
      }


      if      (s_fname.Contains("CSC")) s_region="";//"CSC";
      else if (s_fname.Contains("DT"))  s_region="DT";
      else                              s_region="CSC & DT";
	  delete fin;
	}
    }
  else
    {
      std::cerr << "[ERROR] can't open file " << inputList << std::endl;
    }
  std::cout<<"Pihad_0p3"<<std::endl;
  for ( auto tmp : mymap_pihad_0p3 ){std::cout << "cTau: " << tmp.first << " " << tmp.second.exp0p5 << std::endl;}
  std::cout<<"Pihad_0p3"<<std::endl;

  std::cout<<"\n \nM=0.3\n"; 
  for ( auto tmp : mymap_pihad_0p3 ){std::cout << "cTau:," << tmp.first << "," << tmp.second.exp0p5 <<","<<tmp.second.exp0p16<<","<<tmp.second.exp0p84<<","<<tmp.second.obs<< std::endl;}
  std::cout<<"\n \nM=0.5\n"; 
  for ( auto tmp : mymap_pihad_0p5 ){std::cout << "cTau:," << tmp.first << "," << tmp.second.exp0p5 <<","<<tmp.second.exp0p16<<","<<tmp.second.exp0p84<<","<<tmp.second.obs<< std::endl;}
  std::cout<<"\n \nM=1.0\n"; 
  for ( auto tmp : mymap_pihad_1p0 ){std::cout << "cTau:," << tmp.first << "," << tmp.second.exp0p5 <<","<<tmp.second.exp0p16<<","<<tmp.second.exp0p84<<","<<tmp.second.obs<< std::endl;}
  std::cout<<"\n \nM=2.0\n"; 
  for ( auto tmp : mymap_pihad_2p0 ){std::cout << "cTau:," << tmp.first << "," << tmp.second.exp0p5 <<","<<tmp.second.exp0p16<<","<<tmp.second.exp0p84<<","<<tmp.second.obs<< std::endl;}
  std::cout<<"\n \nM=3.0\n"; 
  for ( auto tmp : mymap_pihad_3p0 ){std::cout << "cTau:," << tmp.first << "," << tmp.second.exp0p5 <<","<<tmp.second.exp0p16<<","<<tmp.second.exp0p84<<","<<tmp.second.obs<< std::endl;}
  std::cout<<"\n \n"; 
//  for ( auto tmp : mymap_pihad ){std::cout << "cTau: " << tmp.first << " " << tmp.second.exp0p5 << std::endl;}
//  std::cout<<"end curve check"<<std::endl;
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

//--------------M0p3
  int npoints_pihad_0p3 = mymap_pihad_0p3.size();
  float x_pihad_0p3[npoints_pihad_0p3];
  float expL_pihad_0p3[npoints_pihad_0p3];
  float obsL_pihad_0p3[npoints_pihad_0p3];
  float xp_pihad_0p3[2*npoints_pihad_0p3];
  float OneS_pihad_0p3[2*npoints_pihad_0p3];
  //float TwoS_pi0[2*npoints_pi0];
  int ctr = 0;
  for ( auto tmp : mymap_pihad_0p3 ){
      x_pihad_0p3[ctr]      = tmp.first;
      obsL_pihad_0p3[ctr]   = tmp.second.obs;
      expL_pihad_0p3[ctr]   = tmp.second.exp0p5;
      xp_pihad_0p3[ctr] = tmp.first;
      xp_pihad_0p3[2*npoints_pihad_0p3-(ctr+1)] = tmp.first;
      OneS_pihad_0p3[ctr] = tmp.second.exp0p16;
      OneS_pihad_0p3[2*npoints_pihad_0p3-(ctr+1)] = tmp.second.exp0p84;
      ctr++;
  }
  TGraph* gObs_pihad_0p3 = new TGraph(npoints_pihad_0p3, x_pihad_0p3, obsL_pihad_0p3);
  TGraph* gExp_pihad_0p3 = new TGraph(npoints_pihad_0p3, x_pihad_0p3, expL_pihad_0p3);
  TGraph* gOneS_pihad_0p3 = new TGraph(2*npoints_pihad_0p3, xp_pihad_0p3, OneS_pihad_0p3);
  //formatting
  setLineQualities(gExp_pihad_0p3, minimum, Color_lim_pihad_0p3, "exp", 7);
  setLineQualities(gOneS_pihad_0p3, minimum, Color_lim_pihad_0p3, "band", 7);
  setLineQualities(gObs_pihad_0p3, minimum, Color_lim_pihad_0p3, "obs", 7);
//---------- end M0p3
//--------------M0p5
  int npoints_pihad_0p5 = mymap_pihad_0p5.size();
  float x_pihad_0p5[npoints_pihad_0p5];
  float expL_pihad_0p5[npoints_pihad_0p5];
  float obsL_pihad_0p5[npoints_pihad_0p5];
  float xp_pihad_0p5[2*npoints_pihad_0p5];
  float OneS_pihad_0p5[2*npoints_pihad_0p5];
  //float TwoS_pi0[2*npoints_pi0];
  ctr = 0;
  for ( auto tmp : mymap_pihad_0p5 ){
      x_pihad_0p5[ctr]      = tmp.first;
      obsL_pihad_0p5[ctr]   = tmp.second.obs;
      expL_pihad_0p5[ctr]   = tmp.second.exp0p5;
      xp_pihad_0p5[ctr] = tmp.first;
      xp_pihad_0p5[2*npoints_pihad_0p5-(ctr+1)] = tmp.first;
      OneS_pihad_0p5[ctr] = tmp.second.exp0p16;
      OneS_pihad_0p5[2*npoints_pihad_0p5-(ctr+1)] = tmp.second.exp0p84;
      ctr++;
  }
  TGraph* gObs_pihad_0p5 = new TGraph(npoints_pihad_0p5, x_pihad_0p5, obsL_pihad_0p5);
  TGraph* gExp_pihad_0p5 = new TGraph(npoints_pihad_0p5, x_pihad_0p5, expL_pihad_0p5);
  TGraph* gOneS_pihad_0p5 = new TGraph(2*npoints_pihad_0p5, xp_pihad_0p5, OneS_pihad_0p5);
  //formatting
  setLineQualities(gExp_pihad_0p5, minimum, Color_lim_pihad_0p5, "exp", 7);
  setLineQualities(gOneS_pihad_0p5, minimum, Color_lim_pihad_0p5, "band", 7);
  setLineQualities(gObs_pihad_0p5, minimum, Color_lim_pihad_0p5, "obs", 7);
//---------- end M0p5
//--------------M1p0
  int npoints_pihad_1p0 = mymap_pihad_1p0.size();
  float x_pihad_1p0[npoints_pihad_1p0];
  float expL_pihad_1p0[npoints_pihad_1p0];
  float obsL_pihad_1p0[npoints_pihad_1p0];
  float xp_pihad_1p0[2*npoints_pihad_1p0];
  float OneS_pihad_1p0[2*npoints_pihad_1p0];
  //float TwoS_pi0[2*npoints_pi0];
  ctr = 0;
  for ( auto tmp : mymap_pihad_1p0 ){
      x_pihad_1p0[ctr]      = tmp.first;
      obsL_pihad_1p0[ctr]   = tmp.second.obs;
      expL_pihad_1p0[ctr]   = tmp.second.exp0p5;
      xp_pihad_1p0[ctr] = tmp.first;
      xp_pihad_1p0[2*npoints_pihad_1p0-(ctr+1)] = tmp.first;
      OneS_pihad_1p0[ctr] = tmp.second.exp0p16;
      OneS_pihad_1p0[2*npoints_pihad_1p0-(ctr+1)] = tmp.second.exp0p84;
      ctr++;
  }
  TGraph* gObs_pihad_1p0 = new TGraph(npoints_pihad_1p0, x_pihad_1p0, obsL_pihad_1p0);
  TGraph* gExp_pihad_1p0 = new TGraph(npoints_pihad_1p0, x_pihad_1p0, expL_pihad_1p0);
  TGraph* gOneS_pihad_1p0 = new TGraph(2*npoints_pihad_1p0, xp_pihad_1p0, OneS_pihad_1p0);
  //formatting
  setLineQualities(gExp_pihad_1p0, minimum, Color_lim_pihad_1p0, "exp", 7);
  setLineQualities(gOneS_pihad_1p0, minimum, Color_lim_pihad_1p0, "band", 7);
  setLineQualities(gObs_pihad_1p0, minimum, Color_lim_pihad_1p0, "obs", 7);
//---------- end M1p0
//--------------M2p0
  int npoints_pihad_2p0 = mymap_pihad_2p0.size();
  float x_pihad_2p0[npoints_pihad_2p0];
  float expL_pihad_2p0[npoints_pihad_2p0];
  float obsL_pihad_2p0[npoints_pihad_2p0];
  float xp_pihad_2p0[2*npoints_pihad_2p0];
  float OneS_pihad_2p0[2*npoints_pihad_2p0];
  //float TwoS_pi0[2*npoints_pi0];
  ctr = 0;
  for ( auto tmp : mymap_pihad_2p0 ){
      x_pihad_2p0[ctr]      = tmp.first;
      obsL_pihad_2p0[ctr]   = tmp.second.obs;
      expL_pihad_2p0[ctr]   = tmp.second.exp0p5;
      xp_pihad_2p0[ctr] = tmp.first;
      xp_pihad_2p0[2*npoints_pihad_2p0-(ctr+1)] = tmp.first;
      OneS_pihad_2p0[ctr] = tmp.second.exp0p16;
      OneS_pihad_2p0[2*npoints_pihad_2p0-(ctr+1)] = tmp.second.exp0p84;
      ctr++;
  }
  TGraph* gObs_pihad_2p0 = new TGraph(npoints_pihad_2p0, x_pihad_2p0, obsL_pihad_2p0);
  TGraph* gExp_pihad_2p0 = new TGraph(npoints_pihad_2p0, x_pihad_2p0, expL_pihad_2p0);
  TGraph* gOneS_pihad_2p0 = new TGraph(2*npoints_pihad_2p0, xp_pihad_2p0, OneS_pihad_2p0);
  //formatting
  setLineQualities(gExp_pihad_2p0, minimum, Color_lim_pihad_2p0, "exp", 7);
  setLineQualities(gOneS_pihad_2p0, minimum, Color_lim_pihad_2p0, "band", 7);
  setLineQualities(gObs_pihad_2p0, minimum, Color_lim_pihad_2p0, "obs", 7);
//---------- end M2p0
//--------------M3p0
  int npoints_pihad_3p0 = mymap_pihad_3p0.size();
  float x_pihad_3p0[npoints_pihad_3p0];
  float expL_pihad_3p0[npoints_pihad_3p0];
  float obsL_pihad_3p0[npoints_pihad_3p0];
  float xp_pihad_3p0[2*npoints_pihad_3p0];
  float OneS_pihad_3p0[2*npoints_pihad_3p0];
  //float TwoS_pi0[2*npoints_pi0];
  ctr = 0;
  for ( auto tmp : mymap_pihad_3p0 ){
      x_pihad_3p0[ctr]      = tmp.first;
      obsL_pihad_3p0[ctr]   = tmp.second.obs;
      expL_pihad_3p0[ctr]   = tmp.second.exp0p5;
      xp_pihad_3p0[ctr] = tmp.first;
      xp_pihad_3p0[2*npoints_pihad_3p0-(ctr+1)] = tmp.first;
      OneS_pihad_3p0[ctr] = tmp.second.exp0p16;
      OneS_pihad_3p0[2*npoints_pihad_3p0-(ctr+1)] = tmp.second.exp0p84;
      ctr++;
  }
  TGraph* gObs_pihad_3p0 = new TGraph(npoints_pihad_3p0, x_pihad_3p0, obsL_pihad_3p0);
  TGraph* gExp_pihad_3p0 = new TGraph(npoints_pihad_3p0, x_pihad_3p0, expL_pihad_3p0);
  TGraph* gOneS_pihad_3p0 = new TGraph(2*npoints_pihad_3p0, xp_pihad_3p0, OneS_pihad_3p0);
  //formatting
  setLineQualities(gExp_pihad_3p0, minimum, Color_lim_pihad_3p0, "exp", 7);
  setLineQualities(gOneS_pihad_3p0, minimum, Color_lim_pihad_3p0, "band", 7);
  setLineQualities(gObs_pihad_3p0, minimum, Color_lim_pihad_3p0, "obs", 7);
//---------- end M3p0
//end lines
  TCanvas* c = new TCanvas( "c", "c", 2119, 33, 800, 700 );
  setCanvas(c);

  //---------Draw
  //gTwoS_pi0->Draw("AF");
  //gObs_pi0->Draw("L");

  //Review updates
  gExp_pihad_1p0->GetXaxis()->SetLabelSize(0.05); 
  gExp_pihad_1p0->GetYaxis()->SetLabelSize(0.05); 
  gExp_pihad_1p0->GetXaxis()->SetTitleOffset(6); 
  gExp_pihad_1p0->GetYaxis()->SetTitleOffset(6); 
  // end review updates
  gExp_pihad_1p0->GetXaxis()->SetTitleSize(0.06);
  gExp_pihad_1p0->GetYaxis()->SetTitleSize(0.05);
  gExp_pihad_0p3->GetYaxis()->SetTitleSize(0.05);
  

  gExp_pihad_1p0->Draw("AL");
  gOneS_pihad_1p0->Draw("F same");
  gObs_pihad_1p0->Draw("L same");


  gExp_pihad_0p3->Draw("L");
  gOneS_pihad_0p3->Draw("F same");
  gObs_pihad_0p3->Draw("L same");
 
  gExp_pihad_0p5->Draw("L");
  gOneS_pihad_0p5->Draw("F same");
  gObs_pihad_0p5->Draw("L same");

  gExp_pihad_1p0->Draw("L");
  gOneS_pihad_1p0->Draw("F same");
  gObs_pihad_1p0->Draw("L same");

  gExp_pihad_2p0->Draw("L");
  gOneS_pihad_2p0->Draw("F same");
  gObs_pihad_2p0->Draw("L same");
  
  gExp_pihad_3p0->Draw("L");
  gOneS_pihad_3p0->Draw("F same");
  gObs_pihad_3p0->Draw("L same");


  c->Update();
  setOffsets(gExp_pihad_0p3);
  setOffsets(gOneS_pihad_0p3);
  setOffsets(gObs_pihad_0p3);
  
  setOffsets(gExp_pihad_0p5);
  setOffsets(gOneS_pihad_0p5);
  setOffsets(gObs_pihad_0p5);
  
  setOffsets(gExp_pihad_1p0);
  setOffsets(gOneS_pihad_1p0);
  setOffsets(gObs_pihad_1p0);
  
  setOffsets(gExp_pihad_2p0);
  setOffsets(gOneS_pihad_2p0);
  setOffsets(gObs_pihad_2p0);
  
  setOffsets(gExp_pihad_3p0);
  setOffsets(gOneS_pihad_3p0);
  setOffsets(gObs_pihad_3p0);

  float l_ymin = 0.65;
  float l_ymax = 0.87;

  //TLegend* leg = new TLegend( 0.15, 0.15, 0.5, 0.35, NULL, "brNDC" );
  //TLegend* leg = new TLegend( 0.15, l_ymin, 0.5, l_ymax, NULL, "brNDC" );
  TLegend* leg = new TLegend( 0.18, l_ymin, 0.5, l_ymax, NULL, "brNDC" );
  setLegendQualities(leg,1);
 
  //TLegend* leg2 = new TLegend( 0.5, 0.15, 0.925, 0.35, NULL, "brNDC" );
  TLegend* leg2 = new TLegend( 0.51, l_ymin, 0.935, l_ymax, NULL, "brNDC" );
  setLegendQualities(leg2,1);
  
  //leg->AddEntry( gExp_pihad_0p3, "#Phi #rightarrow  #Pi#Pi", "l" );
  leg->AddEntry( gExp_pihad_0p3, "m_{#Phi} = 0.3 GeV", "l" );
  leg->AddEntry( gExp_pihad_0p5, "m_{#Phi} = 0.5 GeV", "l" );
  leg->AddEntry( gExp_pihad_1p0, "m_{#Phi} = 1.0 GeV", "l" );
  leg->AddEntry( gExp_pihad_2p0, "m_{#Phi} = 2.0 GeV", "l" );
  leg->AddEntry( gExp_pihad_3p0, "m_{#Phi} = 3.0 GeV", "l" );
  if(plotLHCb)leg->AddEntry( graph_lhcb_0p5, "LHCb B #rightarrow K(S #rightarrow #mu#mu); m_{S} = 0.5 GeV" , "l" );
  if(plotLHCb)leg->AddEntry( graph_lhcb_1p0, "LHCb B #rightarrow K(S #rightarrow #mu#mu); m_{S} = 1 GeV" , "l" );

  //leg2->AddEntry( gOneS_pi0, "#pm 1 s.d. expected", "f" );
  leg2->AddEntry( gOneS_pihad_0p3, "#pm 1 s.d. expected", "f" );
  leg2->AddEntry( gOneS_pihad_0p5, "#pm 1 s.d. expected", "f" );
  leg2->AddEntry( gOneS_pihad_1p0, "#pm 1 s.d. expected", "f" );
  leg2->AddEntry( gOneS_pihad_2p0, "#pm 1 s.d. expected", "f" );
  leg2->AddEntry( gOneS_pihad_3p0, "#pm 1 s.d. expected", "f" );
  if(plotLHCb) leg2->AddEntry("", "", "");  // Add an empty entry
  if(plotLHCb) leg2->AddEntry("", "", "");  // Add an empty entry

  leg->Draw("SAME");
  leg2->Draw("SAME");

  TLegend* leg3 = new TLegend( 0.35, 0.54, 0.875, 0.64, NULL, "brNDC" );
  leg3->SetBorderSize(0);
  leg3->SetLineColor(1);
  leg3->SetLineStyle(1);
  leg3->SetLineWidth(1);
  leg3->SetFillColor(0);
  leg3->SetFillStyle(1001);
  leg3->SetTextSize(0.054);
  leg3->SetNColumns(1);

  leg3->AddEntry( gObs_pihad_0p5, "Observed limit", "l" );
  leg3->AddEntry( gExp_pihad_0p5, "Median expected limit", "l" );
  leg3->Draw("SAME");

  //Phi Mass label
  TLatex latexMass;
  latexMass.SetNDC();
  latexMass.SetTextSize(0.066);
  latexMass.SetTextFont(42);
  latexMass.SetLineWidth(2);
  //latexMass.DrawLatex(0.4, 0.88, "m_{#Phi} = "+s_mass+" GeV");//   "+s_region);
  //latexMass.DrawLatex(0.4, 0.88, "m_{#Phi} = "+s_mass+" GeV   "+s_region);
  //latexMass.DrawLatex(0.425, 0.525, "#Phi #rightarrow  #pi^{+}#pi^{-}   ");


  AddCMS(c);
  //c->SetLogx();
  c->SaveAs(("Limit_"+rawName+".pdf").c_str());
  c->SaveAs(("Limit_"+rawName+".png").c_str());
  c->SaveAs(("scripts/Limit_"+rawName+".C").c_str());

  gObs_pihad_0p3->GetXaxis()->SetRangeUser(0, 30);
  gObs_pihad_0p3->Write("gObs");
  gExp_pihad_0p3->GetXaxis()->SetRangeUser(0, 30);
  gExp_pihad_0p3->Write("gExp");
  gOneS_pihad_0p3->Write("gOneS");

  out->Close();
  return 0;
}


