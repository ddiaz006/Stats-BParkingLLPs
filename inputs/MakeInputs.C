#include <iostream>
#include <string>
//ROOT
#include <TFile.h>
#include <TH1F.h>
#include <TROOT.h>
#include <fstream>

//Try scaling by 200*(1/Integral())
const float SF_sig = 40.3458 * 0.00001; //the lumi * SF
bool rebin=false;
bool scaleSig=true;

TString detector_region = "dt"; //"csc" or "dt" 
TString signalType = "SigPi0_M0p3";  //"SigPi0_M0p3" or "SigPiPlusPiMinus_M0p3"


TH1F* getHisto(TString region, TString Name, TString var){
   TFile* f;
   if(Name.Contains("BToKPhi"))   f = TFile::Open("signal/"+Name+"_"+region+"_plots.root");
   else f = TFile::Open(Name+"_"+region+"_plots.root");
   TH1F* h = (TH1F*)f->Get(var);
   if( Name.Contains("BToKPhi") && scaleSig ) h->Scale(SF_sig); 
   return h;
}

void makeDataCard(TString sigName, TString Type, float yield_A, float yield_B, float yield_C, float yield_D, float yield_sig){
  float yield_pred, yield_obs;
  yield_pred = (yield_C/yield_D)*yield_B;
  if (Type=="IT") yield_obs = yield_pred;
  else yield_obs = yield_A;
  ofstream dataCard;
  dataCard.open("datacards/"+sigName+"-"+Type+"_"+detector_region+".txt");

  dataCard << "# System: "<<detector_region<<"( "<<Type<<" )\n";
  dataCard << "# Signal: "<<sigName<<"\n";
  dataCard << "# SF_tot =  40.3458 * 0.00001; //the lumi * SF"<<"\n";
  dataCard << "imax 1 \n";
  dataCard << "jmax 1 \n";
  dataCard << "kmax * \n";
  dataCard << "------------------------------------------------ \n";
  dataCard << "------------------------------------------------ \n";
  dataCard << "bin"<<" bin1 "<<"\n";
  dataCard << "observation "<<yield_obs<<"\n";
  dataCard << "------------------------------------------------ \n";
  dataCard << "bin"<<" bin1 "<<" bin1 "<<"\n";
  dataCard << "process"<<" 0 "<<" 1 "<<"\n";
  dataCard << "process"<<" sig "<<" bkg "<<"\n";
  dataCard << "rate "<<yield_sig<<" "<<yield_pred<<"\n";
  dataCard << "------------------------------------------------ \n";
  
  dataCard.close();
}

TString makeLine(TString sigName, TString Type, float yield_A, float yield_B, float yield_C, float yield_D, float yield_sig){
  float yield_pred, yield_obs, sigma_pred;
  yield_pred = (yield_C/yield_D)*yield_B;
  sigma_pred = yield_pred*sqrt(1./yield_B + 1./yield_C + 1./yield_D);
  if (Type=="IT") yield_obs = yield_pred;
  else yield_obs = yield_A;
  
  TString line;
  line.Form("%s & %f & %f & %f & %f$\\pm$%f & %f\\\\", detector_region.Data(), yield_B, yield_C, yield_D, yield_pred, sigma_pred, yield_obs);

  return line;
}

TString makeLineSignal(TString sigName, TString Type, float yield_A, float yield_B, float yield_C, float yield_D, float yield_sig){
  float yield_pred, yield_obs, sigma_pred;
  yield_pred = (yield_C/yield_D)*yield_B;
  sigma_pred = yield_pred*sqrt(1./yield_B + 1./yield_C + 1./yield_D);
  if (Type=="IT") yield_obs = yield_pred;
  else yield_obs = yield_A;
  
  TString line;
  line.Form("%s & %s & %f & %f & %f$\\pm$%f & %f\\\\", detector_region.Data(), sigName.Data(), yield_C, yield_D, yield_pred, sigma_pred, yield_obs);

  return line;
}

void makeTable(TString Type, const std::vector<TString>& line){
  ofstream table;
  table.open("bkg-pred_"+detector_region+"_"+Type+".tex");
  table<<"\\begin{table*}[!tbph]"<<std::endl;
  table<<"\\centering"<<std::endl;
  table<<"\\begin{tabular}{l|c|c|c|c|c|}"<<std::endl;
  table<<"Region & B & C & D & Prediction & Obs.\\\\"<<std::endl;
  table<<"\\hline"<<std::endl;
  table<< line[0] << std::endl;
  table<<"\\end{tabular}"<<std::endl;
  table<<"\\end{table*}"<<std::endl;
  
  table.close();
}


void MakeInputs(TString Type){
TString var; 
  if      (detector_region == "csc") var="h_cscRechitClusterSize_FailPass";
  else if (detector_region == "dt")  var="h_dtRechitClusterSize_FailPass";
  else    {std::cout<<"bad detector region"<<std::endl;}

  std::vector<TH1F*> histos;

  std::vector< std::pair<TString,TString> > names;

  //map of names, should be one for each pass/fail pair
  if(signalType=="SigPi0_M0p3"){
    names.push_back(std::pair<TString,TString>("Parking_2018","Data"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to1"     ,"SigPi0_M0p3_ctau1"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to3"     ,"SigPi0_M0p3_ctau3"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to5"     ,"SigPi0_M0p3_ctau5"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to7"     ,"SigPi0_M0p3_ctau7"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to10"    ,"SigPi0_M0p3_ctau10"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to30"    ,"SigPi0_M0p3_ctau30"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to50"    ,"SigPi0_M0p3_ctau50"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to70"    ,"SigPi0_M0p3_ctau70"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to100"   ,"SigPi0_M0p3_ctau100"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to300"   ,"SigPi0_M0p3_ctau300"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to500"   ,"SigPi0_M0p3_ctau500"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to700"   ,"SigPi0_M0p3_ctau700"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to1000"  ,"SigPi0_M0p3_ctau1000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to3000"  ,"SigPi0_M0p3_ctau3000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to5000"  ,"SigPi0_M0p3_ctau5000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to7000"  ,"SigPi0_M0p3_ctau7000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to10000" ,"SigPi0_M0p3_ctau10000"));
  }
  else if (signalType=="SigPiPlusPiMinus_M0p3"){
    names.push_back(std::pair<TString,TString>("Parking_2018","Data"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to1"     ,"SigPiPlusPiMinus_M0p3_ctau1"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to3"     ,"SigPiPlusPiMinus_M0p3_ctau3"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to5"     ,"SigPiPlusPiMinus_M0p3_ctau5"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to7"     ,"SigPiPlusPiMinus_M0p3_ctau7"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to10"    ,"SigPiPlusPiMinus_M0p3_ctau10"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to30"    ,"SigPiPlusPiMinus_M0p3_ctau30"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to50"    ,"SigPiPlusPiMinus_M0p3_ctau50"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to70"    ,"SigPiPlusPiMinus_M0p3_ctau70"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to100"   ,"SigPiPlusPiMinus_M0p3_ctau100"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to300"   ,"SigPiPlusPiMinus_M0p3_ctau300"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to500"   ,"SigPiPlusPiMinus_M0p3_ctau500"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to700"   ,"SigPiPlusPiMinus_M0p3_ctau700"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to1000"  ,"SigPiPlusPiMinus_M0p3_ctau1000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to3000"  ,"SigPiPlusPiMinus_M0p3_ctau3000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to5000"  ,"SigPiPlusPiMinus_M0p3_ctau5000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to7000"  ,"SigPiPlusPiMinus_M0p3_ctau7000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to10000" ,"SigPiPlusPiMinus_M0p3_ctau10000"));
  }
  else if (signalType=="Both"){
    names.push_back(std::pair<TString,TString>("Parking_2018","Data"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to1"     ,"SigPi0_M0p3_ctau1"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to3"     ,"SigPi0_M0p3_ctau3"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to5"     ,"SigPi0_M0p3_ctau5"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to7"     ,"SigPi0_M0p3_ctau7"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to10"    ,"SigPi0_M0p3_ctau10"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to30"    ,"SigPi0_M0p3_ctau30"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to50"    ,"SigPi0_M0p3_ctau50"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to70"    ,"SigPi0_M0p3_ctau70"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to100"   ,"SigPi0_M0p3_ctau100"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to300"   ,"SigPi0_M0p3_ctau300"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to500"   ,"SigPi0_M0p3_ctau500"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to700"   ,"SigPi0_M0p3_ctau700"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to1000"  ,"SigPi0_M0p3_ctau1000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to3000"  ,"SigPi0_M0p3_ctau3000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to5000"  ,"SigPi0_M0p3_ctau5000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to7000"  ,"SigPi0_M0p3_ctau7000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi0p3_ctau300to10000" ,"SigPi0_M0p3_ctau10000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to1"     ,"SigPiPlusPiMinus_M0p3_ctau1"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to3"     ,"SigPiPlusPiMinus_M0p3_ctau3"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to5"     ,"SigPiPlusPiMinus_M0p3_ctau5"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to7"     ,"SigPiPlusPiMinus_M0p3_ctau7"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to10"    ,"SigPiPlusPiMinus_M0p3_ctau10"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to30"    ,"SigPiPlusPiMinus_M0p3_ctau30"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to50"    ,"SigPiPlusPiMinus_M0p3_ctau50"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to70"    ,"SigPiPlusPiMinus_M0p3_ctau70"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to100"   ,"SigPiPlusPiMinus_M0p3_ctau100"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to300"   ,"SigPiPlusPiMinus_M0p3_ctau300"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to500"   ,"SigPiPlusPiMinus_M0p3_ctau500"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to700"   ,"SigPiPlusPiMinus_M0p3_ctau700"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to1000"  ,"SigPiPlusPiMinus_M0p3_ctau1000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to3000"  ,"SigPiPlusPiMinus_M0p3_ctau3000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to5000"  ,"SigPiPlusPiMinus_M0p3_ctau5000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to7000"  ,"SigPiPlusPiMinus_M0p3_ctau7000"));
    names.push_back(std::pair<TString,TString>("BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi0p3_ctau300to10000" ,"SigPiPlusPiMinus_M0p3_ctau10000"));
  }
  else {std::cout<<"bad signalType"<<std::endl;}
  for(int i=0; i<names.size(); i++ ) {
    if(Type=="IT"){
      histos.push_back(getHisto("Fail",names[i].first, var));
      histos.push_back(getHisto("Pass"  ,names[i].first, var));
    }
    else if(Type=="OOT"){
      histos.push_back(getHisto("FailOOT",names[i].first, var));
      histos.push_back(getHisto("PassOOT"  ,names[i].first, var));
    }
    else {std::cout<<"Incorrect Type"<<std::endl; break;}
  }

//  //Write histos to root file
  std::vector<TString> tableLine; 
  TFile* fout = new TFile("inputs_"+detector_region+"_"+signalType+"_"+Type+".root", "RECREATE");
  int index;
  for(int i=0; i<histos.size(); i++){
    index = i/2;
    if((i+1)%2 == 0) std::cout<<" Pass "<<names[index].second<<"   "<<histos[i]->GetBinContent(2)<<std::endl;
    //else             std::cout<<i<<" Fail "<<names[index].second<<"   "<<histos[i]->GetBinContent(1)<<std::endl;
    TString name;
    if( (i+1)%2 == 0) name="h_pass_"+names[index].second;
    else              name="h_fail_"+names[index].second;
    if(index!=0) {
     float y_A = histos[1]->GetBinContent(2);  //PASS NrecHits PASS V2(dPhi) data
     float y_B = histos[0]->GetBinContent(2);  //PASS NrecHits FAIL V2(dPhi) data
     float y_C = histos[1]->GetBinContent(1);  //FAIL NrecHits PASS V2(dPhi) data
     float y_D = histos[0]->GetBinContent(1);  //FAIL NrecHits FAIL V2(dPhi) data
     if( (i+1)%2 == 0) {
       float y_s = histos[i]->GetBinContent(2); //Get Passing signal
       //std::cout<<std::endl;
       //std::cout<<i<<"   "<<(i+1)%2<<"  "<<name<<"  SignalYield  "<<y_s<<std::endl;
       //std::cout<<std::endl;
       makeDataCard(names[index].second, Type, y_A, y_B, y_C, y_D, y_s);
       tableLine.push_back(makeLine(names[index].second, Type, y_A, y_B, y_C, y_D, y_s));
     }
    histos[i]->Write(name);   
    }
  }
  fout->Close();
  makeTable(Type, tableLine);
}
