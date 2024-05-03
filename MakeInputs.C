#include <iostream>
#include <string>
//ROOT
#include <TFile.h>
#include <TH1F.h>
#include <TROOT.h>
#include <fstream>

//Try scaling by 200*(1/Integral())
const float theLumi = 40.3458;
//const float SF_sig = theLumi * 0.0001; //the lumi * SF
const float theSF = 0.000001;
bool scaleSig=true;

TString detector_region = "csc"; //"csc" or "dt" 
const TString s_mass = "0p3";
const TString s_ct = "300";

TString vName = getenv ("version");
TString input_path = "/uscms/home/ddiaz/nobackup/BParkingLLPs/CMSSW_9_4_4/src/Stats/CMSSW_10_2_13/src/ABCD/inputs_"+vName+"/"; //OPT/"; 

TH1F* getHisto(TString region, TString Name, TString var){
   TFile* f;
   if(Name.Contains("BToKPhi"))   f = TFile::Open(input_path+"signal/"+Name+"_"+region+"_plots.root");
   else f = TFile::Open(input_path+Name+"_"+region+"_plots.root");
   TH1F* h = (TH1F*)f->Get(var);
   if( Name.Contains("BToKPhi") && scaleSig && !var.Contains("_uw")) h->Scale(theLumi); 
   return h;
}

std::map<TString,float> getSF(){
  ifstream SFfile;
  SFfile.open(input_path+"SF.txt");

  TString name;
  float sf;
  std::map<TString,float> SFs;
  while(SFfile>> name >> sf) SFs[name]=sf;
  SFfile.close();  
  return SFs;
}

void makeDataCard(TString type, TString sigName, TString detector_region, float y_bkg[], float y_s[], float y_s_uw[], float SF ){
  float yield_pred;
  if (y_bkg[3] > 0)
    yield_pred = (y_bkg[2]/y_bkg[3])*y_bkg[1];
  else yield_pred = -1;
  if (!scaleSig) SF = 1.0;

  y_s[0] = SF*y_s[0]; 
  y_s[1] = SF*y_s[1]; 
  y_s[2] = SF*y_s[2]; 
  y_s[3] = SF*y_s[3]; 
  std::cout<<"Making: "+input_path+"datacards/"+sigName+"-"+type+"_"+detector_region+".txt"<<endl;
  //std::cout<<"TheSF: "<<SF<<std::endl;
  ofstream dataCard;
  dataCard.open(input_path+"datacards/"+sigName+"-"+type+"_"+detector_region+".txt");

  dataCard << "# System: "<<detector_region<<"\n";
  dataCard << "# Signal: "<<sigName<<"\n";
  dataCard << "# Note: Uses the Arrays"<<"\n";
  dataCard << "# SF_tot =  40.3458 * "<<SF<<" ="<<theLumi*SF<<" //the lumi * SF"<<"\n";
  dataCard << "imax 4 \n";
  dataCard << "jmax 1 \n";
  dataCard << "kmax * \n";
  dataCard << "------------------------------------------------ \n";
  dataCard << "------------------------------------------------ \n";
  dataCard << "bin"<<" chA "<<" chB "<<" chC "<<" chD "<<"\n";
  dataCard << "observation "<<yield_pred<<" "<<y_bkg[1]<<" "<<y_bkg[2]<<" "<<y_bkg[3]<<"\n";
  dataCard << "------------------------------------------------ \n";
  dataCard << "bin"<<" chA "<<" chA "<<" chB "<<" chB "<<" chC "<<" chC "<<" chD "<<" chD "<<"\n";
  dataCard << "process "<<" 0 "<<" 1 "<<" 0 "<<" 1 "<<" 0 "<<" 1 "<<" 0 "<<" 1 "<<"\n";
  dataCard << "process"<<" sig "<<" bkg "<<" sig "<<" bkg "<<" sig "<<" bkg "<<" sig "<<" bkg "<<"\n";
  dataCard << "rate "<<y_s[0]<<" 1 "<<y_s[1]<<" 1 "<<y_s[2]<<" 1 "<<y_s[3]<<" 1 "<<"\n";
  dataCard << "------------------------------------------------ \n";
  dataCard << "s_A_"<<detector_region<<" rateParam "<<" chA "<<" bkg "<<" (@0*@1/@2) " <<"s_B_"<<detector_region<<",s_C_"<<detector_region<<",s_D_"<<detector_region<<"\n";
  dataCard << "s_B_"<<detector_region<<" rateParam "<<" chB "<<" bkg "<<  y_bkg[1]<<" [0,"<< 7.*y_bkg[1] <<"]"<<"\n";
  dataCard << "s_C_"<<detector_region<<" rateParam "<<" chC "<<" bkg "<<  y_bkg[2]<<" [0,"<< 7.*y_bkg[2] <<"]"<<"\n";
  dataCard << "s_D_"<<detector_region<<" rateParam "<<" chD "<<" bkg "<<  y_bkg[3]<<" [0,"<< 7.*y_bkg[3] <<"]"<<"\n";
  dataCard << "lumi             lnN 1.025  - 1.025  - 1.025  - 1.025  -"<<"\n";
  if (detector_region =="csc"){
    //datacard << "cluster_eff_CSC  lnN 1.0014 - 1.0014 - 1.0014 - 1.0014 -"<<"\n"; 
    dataCard << "ME1112_veto_CSC  lnN 1.024  - 1.024  - 1.024  - 1.024  -"<<"\n";
    dataCard << "RE1_veto_CSC     lnN 1.008  - 1.008  - 1.008  - 1.008  -"<<"\n";
    dataCard << "MB1_veto_CSC     lnN 1.012  - 1.012  - 1.012  - 1.012  -"<<"\n";
    dataCard << "RB1_veto_CSC     lnN 1.027  - 1.027  - 1.027  - 1.027  -"<<"\n";
    dataCard << "Muon_veto_CSC    lnN 1.005  - 1.005  - 1.005  - 1.005  -"<<"\n";
    //dataCard << "Closure_CSC      lnN   -   1.2  - -  - -  - -"<<"\n";
    if (y_s_uw[0] < 100 && y_s_uw[0] > 0 ) dataCard << "mc_stats_s_A_CSC gmN "<<y_s_uw[0]<<" "<< y_s[0]/y_s_uw[0]<<" -  - -  - -  - -" <<"\n";
    if (y_s_uw[1] < 100 && y_s_uw[1] > 0 ) dataCard << "mc_stats_s_B_CSC gmN "<<y_s_uw[1]<<" - -  "<< y_s[1]/y_s_uw[1]<<" -  - -  - -" <<"\n";
    if (y_s_uw[2] < 100 && y_s_uw[2] > 0 ) dataCard << "mc_stats_s_C_CSC gmN "<<y_s_uw[2]<<" - -  - -  "<< y_s[2]/y_s_uw[2]<<" -  - -" <<"\n";
    if (y_s_uw[3] < 100 && y_s_uw[3] > 0 ) dataCard << "mc_stats_s_D_CSC gmN "<<y_s_uw[3]<<" - -  - -  - -  "<< y_s[3]/y_s_uw[3]<<"  -" <<"\n";
  }
  if (detector_region =="dt"){
    //dataCard << "cluster_eff_DT lnN 1.0002 - 1.0002 - 1.0002 - 1.0002 -"<<"\n"; 
    dataCard << "RPC_match_DT   lnN 1.0377 - 1.0377 - 1.0377 - 1.0377 -"<<"\n";
    dataCard << "Muon_veto_DT   lnN 1.001  - 1.001  - 1.001  - 1.001  -"<<"\n";
    dataCard << "MB1_veto_DT    lnN 1.0743 - 1.0743 - 1.0743 - 1.0743 -"<<"\n";
    if (y_s_uw[0] < 100 && y_s_uw[0] > 0 ) dataCard << "mc_stats_s_A_DT gmN "<<y_s_uw[0]<<" "<< y_s[0]/y_s_uw[0]<<" -  - -  - -  - -" <<"\n";
    if (y_s_uw[1] < 100 && y_s_uw[1] > 0 ) dataCard << "mc_stats_s_B_DT gmN "<<y_s_uw[1]<<" - -  "<< y_s[1]/y_s_uw[1]<<" -  - -  - -" <<"\n";
    if (y_s_uw[2] < 100 && y_s_uw[2] > 0 ) dataCard << "mc_stats_s_C_DT gmN "<<y_s_uw[2]<<" - -  - -  "<< y_s[2]/y_s_uw[2]<<" -  - -" <<"\n";
    if (y_s_uw[3] < 100 && y_s_uw[3] > 0 ) dataCard << "mc_stats_s_D_DT gmN "<<y_s_uw[3]<<" - -  - -  - -  "<< y_s[3]/y_s_uw[3]<<"  -" <<"\n";

  }
  dataCard.close();
}

TString makeLine(TString detector_region, TString sigName, TString Type, float yield_A, float yield_B, float yield_C, float yield_D, float yield_sig){
  float yield_pred, yield_obs, sigma_pred;
  yield_pred = (yield_C/yield_D)*yield_B;
  sigma_pred = yield_pred*sqrt(1./yield_B + 1./yield_C + 1./yield_D);
  if (Type=="IT") yield_obs = yield_pred;
  else yield_obs = yield_A;
  
  TString line;
  line.Form("& & &%s & %f & %f & %f & %f$\\pm$%f & %f\\\\", detector_region.Data(), yield_B, yield_C, yield_D, yield_pred, sigma_pred, yield_obs);

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

void makeTable(TString detector_region, TString Type, const std::vector<TString>& line){
  ofstream table;
  table.open(input_path+"bkg-pred_"+detector_region+"_"+Type+".tex");
  table<<"\\begin{table*}[!tbph]"<<std::endl;
  table<<"\\centering"<<std::endl;
  table<<"\\begin{tabular}{l|l|l|c|c|c|c|c|c|}"<<std::endl;
  table<<"&NRecHits&$\\Delta\\Phi$&Region & B & C & D & Prediction & Obs. \\\\"<<std::endl;
  table<<"\\hline"<<std::endl;
  table<< line[0] << std::endl;
  table<<"\\end{tabular}"<<std::endl;
  table<<"\\end{table*}"<<std::endl;
  
  table.close();
}

void doMakeInputs(TString Type, TString detector_region, TString signalType){
TString var; 
  if      (detector_region == "csc") var="h_cscRechitClusterSize_FailPass";
  else if (detector_region == "dt")  var="h_dtRechitClusterSize_FailPass";
  else    {std::cout<<"bad detector region"<<std::endl;}

  std::vector<TH1F*> histos_PassN;
  std::vector<TH1F*> histos_FailN;
  std::vector<TH1F*> histos_PassN_uw;
  std::vector<TH1F*> histos_FailN_uw;
 
  std::map<TString, float> SFmap = getSF();
  std::vector< std::pair<TString,TString> > names;
  TString prefix_Pi0 = "BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi"+s_mass+"_ctau"+s_ct;
  TString prefix_Pihad = "BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi"+s_mass+"_ctau"+s_ct;
  TString s_prefix_Pi0 = "BToKPhi_MuonLLPDecayGenFilter_PhiToPi0Pi0_mPhi";
  TString s_prefix_Pihad = "BToKPhi_MuonLLPDecayGenFilter_PhiToPiPlusPiMinus_mPhi";
  //map of names, should be one for each pass/fail pair
  if(signalType=="SigPi0_M0p3"){
    names.push_back(std::pair<TString,TString>("Parking_2018","Data"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to1"     ,"SigPi0_M"+s_mass+"_ctau1"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to3"     ,"SigPi0_M"+s_mass+"_ctau3"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to5"     ,"SigPi0_M"+s_mass+"_ctau5"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to7"     ,"SigPi0_M"+s_mass+"_ctau7"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to10"    ,"SigPi0_M"+s_mass+"_ctau10"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to30"    ,"SigPi0_M"+s_mass+"_ctau30"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to50"    ,"SigPi0_M"+s_mass+"_ctau50"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to70"    ,"SigPi0_M"+s_mass+"_ctau70"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to100"   ,"SigPi0_M"+s_mass+"_ctau100"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to300"   ,"SigPi0_M"+s_mass+"_ctau300"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to500"   ,"SigPi0_M"+s_mass+"_ctau500"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to700"   ,"SigPi0_M"+s_mass+"_ctau700"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to1000"  ,"SigPi0_M"+s_mass+"_ctau1000"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to3000"  ,"SigPi0_M"+s_mass+"_ctau3000"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to5000"  ,"SigPi0_M"+s_mass+"_ctau5000"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to7000"  ,"SigPi0_M"+s_mass+"_ctau7000"));
    names.push_back(std::pair<TString,TString>(prefix_Pi0+"to10000" ,"SigPi0_M"+s_mass+"_ctau10000"));
  }
  else if (signalType=="SigPiPlusPiMinus_M0p3"){
    names.push_back(std::pair<TString,TString>("Parking_2018","Data"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to1"     ,"SigPiPlusPiMinus_M"+s_mass+"_ctau1"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to3"     ,"SigPiPlusPiMinus_M"+s_mass+"_ctau3"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to5"     ,"SigPiPlusPiMinus_M"+s_mass+"_ctau5"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to7"     ,"SigPiPlusPiMinus_M"+s_mass+"_ctau7"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to10"    ,"SigPiPlusPiMinus_M"+s_mass+"_ctau10"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to30"    ,"SigPiPlusPiMinus_M"+s_mass+"_ctau30"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to50"    ,"SigPiPlusPiMinus_M"+s_mass+"_ctau50"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to70"    ,"SigPiPlusPiMinus_M"+s_mass+"_ctau70"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to100"   ,"SigPiPlusPiMinus_M"+s_mass+"_ctau100"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to300"   ,"SigPiPlusPiMinus_M"+s_mass+"_ctau300"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to500"   ,"SigPiPlusPiMinus_M"+s_mass+"_ctau500"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to700"   ,"SigPiPlusPiMinus_M"+s_mass+"_ctau700"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to1000"  ,"SigPiPlusPiMinus_M"+s_mass+"_ctau1000"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to3000"  ,"SigPiPlusPiMinus_M"+s_mass+"_ctau3000"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to5000"  ,"SigPiPlusPiMinus_M"+s_mass+"_ctau5000"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to7000"  ,"SigPiPlusPiMinus_M"+s_mass+"_ctau7000"));
    names.push_back(std::pair<TString,TString>(prefix_Pihad+"to10000" ,"SigPiPlusPiMinus_M"+s_mass+"_ctau10000"));
  }
//--------------OOT
  else if (signalType=="OOT"){
    names.push_back(std::pair<TString,TString>("Parking_2018","Data"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to1"      ,"SigPi0_M0p3_ctau1"));
  }
//--------------M0p3
  else if (signalType=="M0p3_high"){
    names.push_back(std::pair<TString,TString>("Parking_2018","Data"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to1"     ,"SigPi0_M0p3_high_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to3"     ,"SigPi0_M0p3_high_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to5"     ,"SigPi0_M0p3_high_ctau5")); 
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to7"     ,"SigPi0_M0p3_high_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to10"    ,"SigPi0_M0p3_high_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to30"    ,"SigPi0_M0p3_high_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to50"    ,"SigPi0_M0p3_high_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to70"    ,"SigPi0_M0p3_high_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to100"   ,"SigPi0_M0p3_high_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to300"   ,"SigPi0_M0p3_high_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to500"   ,"SigPi0_M0p3_high_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to700"   ,"SigPi0_M0p3_high_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to1000"  ,"SigPi0_M0p3_high_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to3000"  ,"SigPi0_M0p3_high_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to5000"  ,"SigPi0_M0p3_high_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to7000"  ,"SigPi0_M0p3_high_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau3000to10000" ,"SigPi0_M0p3_high_ctau10000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to1"     ,"SigPiPlusPiMinus_M0p3_high_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to3"     ,"SigPiPlusPiMinus_M0p3_high_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to5"     ,"SigPiPlusPiMinus_M0p3_high_ctau5"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to7"     ,"SigPiPlusPiMinus_M0p3_high_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to10"    ,"SigPiPlusPiMinus_M0p3_high_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to30"    ,"SigPiPlusPiMinus_M0p3_high_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to50"    ,"SigPiPlusPiMinus_M0p3_high_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to70"    ,"SigPiPlusPiMinus_M0p3_high_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to100"   ,"SigPiPlusPiMinus_M0p3_high_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to300"   ,"SigPiPlusPiMinus_M0p3_high_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to500"   ,"SigPiPlusPiMinus_M0p3_high_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to700"   ,"SigPiPlusPiMinus_M0p3_high_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to1000"  ,"SigPiPlusPiMinus_M0p3_high_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to3000"  ,"SigPiPlusPiMinus_M0p3_high_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to5000"  ,"SigPiPlusPiMinus_M0p3_high_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to7000"  ,"SigPiPlusPiMinus_M0p3_high_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau3000to10000" ,"SigPiPlusPiMinus_M0p3_high_ctau10000"));
  }
//--------------M0p3
  else if (signalType=="M0p3_low"){
    names.push_back(std::pair<TString,TString>("Parking_2018","Data"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to1"      ,"SigPi0_M0p3_low_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to3"      ,"SigPi0_M0p3_low_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to5"      ,"SigPi0_M0p3_low_ctau5")); 
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to7"      ,"SigPi0_M0p3_low_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to10"     ,"SigPi0_M0p3_low_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to30"     ,"SigPi0_M0p3_low_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to50"     ,"SigPi0_M0p3_low_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to70"     ,"SigPi0_M0p3_low_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to100"    ,"SigPi0_M0p3_low_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to300"    ,"SigPi0_M0p3_low_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to500"    ,"SigPi0_M0p3_low_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to700"    ,"SigPi0_M0p3_low_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to1000"   ,"SigPi0_M0p3_low_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to3000"   ,"SigPi0_M0p3_low_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to5000"   ,"SigPi0_M0p3_low_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to7000"   ,"SigPi0_M0p3_low_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p3_ctau300to10000"  ,"SigPi0_M0p3_low_ctau10000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to1"      ,"SigPiPlusPiMinus_M0p3_low_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to3"      ,"SigPiPlusPiMinus_M0p3_low_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to5"      ,"SigPiPlusPiMinus_M0p3_low_ctau5"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to7"      ,"SigPiPlusPiMinus_M0p3_low_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to10"     ,"SigPiPlusPiMinus_M0p3_low_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to30"     ,"SigPiPlusPiMinus_M0p3_low_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to50"     ,"SigPiPlusPiMinus_M0p3_low_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to70"     ,"SigPiPlusPiMinus_M0p3_low_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to100"    ,"SigPiPlusPiMinus_M0p3_low_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to300"    ,"SigPiPlusPiMinus_M0p3_low_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to500"    ,"SigPiPlusPiMinus_M0p3_low_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to700"    ,"SigPiPlusPiMinus_M0p3_low_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to1000"   ,"SigPiPlusPiMinus_M0p3_low_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to3000"   ,"SigPiPlusPiMinus_M0p3_low_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to5000"   ,"SigPiPlusPiMinus_M0p3_low_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to7000"   ,"SigPiPlusPiMinus_M0p3_low_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p3_ctau300to10000"  ,"SigPiPlusPiMinus_M0p3_low_ctau10000"));
  }
//--------------M0p5_high
  else if (signalType=="M0p5_high"){
    names.push_back(std::pair<TString,TString>("Parking_2018","Data"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to1"      ,"SigPi0_M0p5_high_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to3"      ,"SigPi0_M0p5_high_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to5"      ,"SigPi0_M0p5_high_ctau5")); 
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to7"      ,"SigPi0_M0p5_high_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to10"     ,"SigPi0_M0p5_high_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to30"     ,"SigPi0_M0p5_high_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to50"     ,"SigPi0_M0p5_high_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to70"     ,"SigPi0_M0p5_high_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to100"    ,"SigPi0_M0p5_high_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to300"    ,"SigPi0_M0p5_high_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to500"    ,"SigPi0_M0p5_high_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to700"    ,"SigPi0_M0p5_high_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to1000"   ,"SigPi0_M0p5_high_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to3000"   ,"SigPi0_M0p5_high_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to5000"   ,"SigPi0_M0p5_high_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to7000"   ,"SigPi0_M0p5_high_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau5000to10000"  ,"SigPi0_M0p5_high_ctau10000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to1"      ,"SigPiPlusPiMinus_M0p5_high_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to3"      ,"SigPiPlusPiMinus_M0p5_high_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to5"      ,"SigPiPlusPiMinus_M0p5_high_ctau5"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to7"      ,"SigPiPlusPiMinus_M0p5_high_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to10"     ,"SigPiPlusPiMinus_M0p5_high_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to30"     ,"SigPiPlusPiMinus_M0p5_high_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to50"     ,"SigPiPlusPiMinus_M0p5_high_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to70"     ,"SigPiPlusPiMinus_M0p5_high_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to100"    ,"SigPiPlusPiMinus_M0p5_high_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to300"    ,"SigPiPlusPiMinus_M0p5_high_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to500"    ,"SigPiPlusPiMinus_M0p5_high_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to700"    ,"SigPiPlusPiMinus_M0p5_high_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to1000"   ,"SigPiPlusPiMinus_M0p5_high_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to3000"   ,"SigPiPlusPiMinus_M0p5_high_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to5000"   ,"SigPiPlusPiMinus_M0p5_high_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to7000"   ,"SigPiPlusPiMinus_M0p5_high_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau5000to10000"  ,"SigPiPlusPiMinus_M0p5_high_ctau10000"));
  }
//--------------M0p5_low
  else if (signalType=="M0p5_low"){
    names.push_back(std::pair<TString,TString>("Parking_2018","Data"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to1"     ,"SigPi0_M0p5_low_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to3"     ,"SigPi0_M0p5_low_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to5"     ,"SigPi0_M0p5_low_ctau5")); 
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to7"     ,"SigPi0_M0p5_low_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to10"    ,"SigPi0_M0p5_low_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to30"    ,"SigPi0_M0p5_low_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to50"    ,"SigPi0_M0p5_low_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to70"    ,"SigPi0_M0p5_low_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to100"   ,"SigPi0_M0p5_low_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to300"   ,"SigPi0_M0p5_low_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to500"   ,"SigPi0_M0p5_low_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to700"   ,"SigPi0_M0p5_low_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to1000"  ,"SigPi0_M0p5_low_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to3000"  ,"SigPi0_M0p5_low_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to5000"  ,"SigPi0_M0p5_low_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to7000"  ,"SigPi0_M0p5_low_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"0p5_ctau500to10000" ,"SigPi0_M0p5_low_ctau10000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to1"     ,"SigPiPlusPiMinus_M0p5_low_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to3"     ,"SigPiPlusPiMinus_M0p5_low_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to5"     ,"SigPiPlusPiMinus_M0p5_low_ctau5"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to7"     ,"SigPiPlusPiMinus_M0p5_low_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to10"    ,"SigPiPlusPiMinus_M0p5_low_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to30"    ,"SigPiPlusPiMinus_M0p5_low_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to50"    ,"SigPiPlusPiMinus_M0p5_low_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to70"    ,"SigPiPlusPiMinus_M0p5_low_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to100"   ,"SigPiPlusPiMinus_M0p5_low_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to300"   ,"SigPiPlusPiMinus_M0p5_low_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to500"   ,"SigPiPlusPiMinus_M0p5_low_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to700"   ,"SigPiPlusPiMinus_M0p5_low_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to1000"  ,"SigPiPlusPiMinus_M0p5_low_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to3000"  ,"SigPiPlusPiMinus_M0p5_low_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to5000"  ,"SigPiPlusPiMinus_M0p5_low_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to7000"  ,"SigPiPlusPiMinus_M0p5_low_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"0p5_ctau500to10000" ,"SigPiPlusPiMinus_M0p5_low_ctau10000"));
  }
//--------------M1p0_high
  else if (signalType=="M1p0_high"){
    names.push_back(std::pair<TString,TString>("Parking_2018","Data"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to1"     ,"SigPi0_M1p0_high_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to3"     ,"SigPi0_M1p0_high_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to5"     ,"SigPi0_M1p0_high_ctau5")); 
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to7"     ,"SigPi0_M1p0_high_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to10"    ,"SigPi0_M1p0_high_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to30"    ,"SigPi0_M1p0_high_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to50"    ,"SigPi0_M1p0_high_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to70"    ,"SigPi0_M1p0_high_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to100"   ,"SigPi0_M1p0_high_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to300"   ,"SigPi0_M1p0_high_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to500"   ,"SigPi0_M1p0_high_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to700"   ,"SigPi0_M1p0_high_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to1000"  ,"SigPi0_M1p0_high_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to3000"  ,"SigPi0_M1p0_high_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to5000"  ,"SigPi0_M1p0_high_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to7000"  ,"SigPi0_M1p0_high_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau10000to10000" ,"SigPi0_M1p0_high_ctau10000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to1"     ,"SigPiPlusPiMinus_M1p0_high_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to3"     ,"SigPiPlusPiMinus_M1p0_high_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to5"     ,"SigPiPlusPiMinus_M1p0_high_ctau5"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to7"     ,"SigPiPlusPiMinus_M1p0_high_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to10"    ,"SigPiPlusPiMinus_M1p0_high_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to30"    ,"SigPiPlusPiMinus_M1p0_high_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to50"    ,"SigPiPlusPiMinus_M1p0_high_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to70"    ,"SigPiPlusPiMinus_M1p0_high_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to100"   ,"SigPiPlusPiMinus_M1p0_high_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to300"   ,"SigPiPlusPiMinus_M1p0_high_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to500"   ,"SigPiPlusPiMinus_M1p0_high_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to700"   ,"SigPiPlusPiMinus_M1p0_high_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to1000"  ,"SigPiPlusPiMinus_M1p0_high_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to3000"  ,"SigPiPlusPiMinus_M1p0_high_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to5000"  ,"SigPiPlusPiMinus_M1p0_high_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to7000"  ,"SigPiPlusPiMinus_M1p0_high_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau10000to10000" ,"SigPiPlusPiMinus_M1p0_high_ctau10000"));
  }
//--------------M1p0_low
  else if (signalType=="M1p0_low"){
    names.push_back(std::pair<TString,TString>("Parking_2018","Data"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to1"     ,"SigPi0_M1p0_low_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to3"     ,"SigPi0_M1p0_low_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to5"     ,"SigPi0_M1p0_low_ctau5")); 
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to7"     ,"SigPi0_M1p0_low_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to10"    ,"SigPi0_M1p0_low_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to30"    ,"SigPi0_M1p0_low_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to50"    ,"SigPi0_M1p0_low_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to70"    ,"SigPi0_M1p0_low_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to100"   ,"SigPi0_M1p0_low_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to300"   ,"SigPi0_M1p0_low_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to500"   ,"SigPi0_M1p0_low_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to700"   ,"SigPi0_M1p0_low_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to1000"  ,"SigPi0_M1p0_low_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to3000"  ,"SigPi0_M1p0_low_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to5000"  ,"SigPi0_M1p0_low_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to7000"  ,"SigPi0_M1p0_low_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"1p0_ctau1000to10000" ,"SigPi0_M1p0_low_ctau10000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to1"     ,"SigPiPlusPiMinus_M1p0_low_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to3"     ,"SigPiPlusPiMinus_M1p0_low_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to5"     ,"SigPiPlusPiMinus_M1p0_low_ctau5"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to7"     ,"SigPiPlusPiMinus_M1p0_low_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to10"    ,"SigPiPlusPiMinus_M1p0_low_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to30"    ,"SigPiPlusPiMinus_M1p0_low_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to50"    ,"SigPiPlusPiMinus_M1p0_low_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to70"    ,"SigPiPlusPiMinus_M1p0_low_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to100"   ,"SigPiPlusPiMinus_M1p0_low_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to300"   ,"SigPiPlusPiMinus_M1p0_low_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to500"   ,"SigPiPlusPiMinus_M1p0_low_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to700"   ,"SigPiPlusPiMinus_M1p0_low_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to1000"  ,"SigPiPlusPiMinus_M1p0_low_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to3000"  ,"SigPiPlusPiMinus_M1p0_low_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to5000"  ,"SigPiPlusPiMinus_M1p0_low_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to7000"  ,"SigPiPlusPiMinus_M1p0_low_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"1p0_ctau1000to10000" ,"SigPiPlusPiMinus_M1p0_low_ctau10000"));
  }
//--------------M2p0_high
  else if (signalType=="M2p0_high"){
    names.push_back(std::pair<TString,TString>("Parking_2018","Data"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to1"     ,"SigPi0_M2p0_high_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to3"     ,"SigPi0_M2p0_high_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to5"     ,"SigPi0_M2p0_high_ctau5")); 
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to7"     ,"SigPi0_M2p0_high_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to10"    ,"SigPi0_M2p0_high_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to30"    ,"SigPi0_M2p0_high_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to50"    ,"SigPi0_M2p0_high_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to70"    ,"SigPi0_M2p0_high_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to100"   ,"SigPi0_M2p0_high_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to300"   ,"SigPi0_M2p0_high_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to500"   ,"SigPi0_M2p0_high_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to700"   ,"SigPi0_M2p0_high_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to1000"  ,"SigPi0_M2p0_high_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to3000"  ,"SigPi0_M2p0_high_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to5000"  ,"SigPi0_M2p0_high_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to7000"  ,"SigPi0_M2p0_high_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau10000to10000" ,"SigPi0_M2p0_high_ctau10000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to1"     ,"SigPiPlusPiMinus_M2p0_high_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to3"     ,"SigPiPlusPiMinus_M2p0_high_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to5"     ,"SigPiPlusPiMinus_M2p0_high_ctau5"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to7"     ,"SigPiPlusPiMinus_M2p0_high_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to10"    ,"SigPiPlusPiMinus_M2p0_high_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to30"    ,"SigPiPlusPiMinus_M2p0_high_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to50"    ,"SigPiPlusPiMinus_M2p0_high_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to70"    ,"SigPiPlusPiMinus_M2p0_high_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to100"   ,"SigPiPlusPiMinus_M2p0_high_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to300"   ,"SigPiPlusPiMinus_M2p0_high_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to500"   ,"SigPiPlusPiMinus_M2p0_high_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to700"   ,"SigPiPlusPiMinus_M2p0_high_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to1000"  ,"SigPiPlusPiMinus_M2p0_high_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to3000"  ,"SigPiPlusPiMinus_M2p0_high_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to5000"  ,"SigPiPlusPiMinus_M2p0_high_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to7000"  ,"SigPiPlusPiMinus_M2p0_high_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau10000to10000" ,"SigPiPlusPiMinus_M2p0_high_ctau10000"));
  }
//--------------M2p0_low
  else if (signalType=="M2p0_low"){
    names.push_back(std::pair<TString,TString>("Parking_2018","Data"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to1"     ,"SigPi0_M2p0_low_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to3"     ,"SigPi0_M2p0_low_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to5"     ,"SigPi0_M2p0_low_ctau5")); 
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to7"     ,"SigPi0_M2p0_low_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to10"    ,"SigPi0_M2p0_low_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to30"    ,"SigPi0_M2p0_low_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to50"    ,"SigPi0_M2p0_low_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to70"    ,"SigPi0_M2p0_low_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to100"   ,"SigPi0_M2p0_low_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to300"   ,"SigPi0_M2p0_low_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to500"   ,"SigPi0_M2p0_low_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to700"   ,"SigPi0_M2p0_low_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to1000"  ,"SigPi0_M2p0_low_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to3000"  ,"SigPi0_M2p0_low_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to5000"  ,"SigPi0_M2p0_low_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to7000"  ,"SigPi0_M2p0_low_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"2p0_ctau2000to10000" ,"SigPi0_M2p0_low_ctau10000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to1"     ,"SigPiPlusPiMinus_M2p0_low_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to3"     ,"SigPiPlusPiMinus_M2p0_low_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to5"     ,"SigPiPlusPiMinus_M2p0_low_ctau5"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to7"     ,"SigPiPlusPiMinus_M2p0_low_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to10"    ,"SigPiPlusPiMinus_M2p0_low_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to30"    ,"SigPiPlusPiMinus_M2p0_low_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to50"    ,"SigPiPlusPiMinus_M2p0_low_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to70"    ,"SigPiPlusPiMinus_M2p0_low_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to100"   ,"SigPiPlusPiMinus_M2p0_low_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to300"   ,"SigPiPlusPiMinus_M2p0_low_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to500"   ,"SigPiPlusPiMinus_M2p0_low_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to700"   ,"SigPiPlusPiMinus_M2p0_low_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to1000"  ,"SigPiPlusPiMinus_M2p0_low_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to3000"  ,"SigPiPlusPiMinus_M2p0_low_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to5000"  ,"SigPiPlusPiMinus_M2p0_low_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to7000"  ,"SigPiPlusPiMinus_M2p0_low_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"2p0_ctau2000to10000" ,"SigPiPlusPiMinus_M2p0_low_ctau10000"));
  }
//--------------M3p0_high
  else if (signalType=="M3p0_high"){
    names.push_back(std::pair<TString,TString>("Parking_2018","Data"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to1"     ,"SigPi0_M3p0_high_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to3"     ,"SigPi0_M3p0_high_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to5"     ,"SigPi0_M3p0_high_ctau5")); 
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to7"     ,"SigPi0_M3p0_high_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to10"    ,"SigPi0_M3p0_high_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to30"    ,"SigPi0_M3p0_high_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to50"    ,"SigPi0_M3p0_high_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to70"    ,"SigPi0_M3p0_high_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to100"   ,"SigPi0_M3p0_high_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to300"   ,"SigPi0_M3p0_high_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to500"   ,"SigPi0_M3p0_high_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to700"   ,"SigPi0_M3p0_high_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to1000"  ,"SigPi0_M3p0_high_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to3000"  ,"SigPi0_M3p0_high_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to5000"  ,"SigPi0_M3p0_high_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to7000"  ,"SigPi0_M3p0_high_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau10000to10000" ,"SigPi0_M3p0_high_ctau10000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to1"     ,"SigPiPlusPiMinus_M3p0_high_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to3"     ,"SigPiPlusPiMinus_M3p0_high_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to5"     ,"SigPiPlusPiMinus_M3p0_high_ctau5"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to7"     ,"SigPiPlusPiMinus_M3p0_high_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to10"    ,"SigPiPlusPiMinus_M3p0_high_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to30"    ,"SigPiPlusPiMinus_M3p0_high_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to50"    ,"SigPiPlusPiMinus_M3p0_high_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to70"    ,"SigPiPlusPiMinus_M3p0_high_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to100"   ,"SigPiPlusPiMinus_M3p0_high_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to300"   ,"SigPiPlusPiMinus_M3p0_high_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to500"   ,"SigPiPlusPiMinus_M3p0_high_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to700"   ,"SigPiPlusPiMinus_M3p0_high_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to1000"  ,"SigPiPlusPiMinus_M3p0_high_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to3000"  ,"SigPiPlusPiMinus_M3p0_high_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to5000"  ,"SigPiPlusPiMinus_M3p0_high_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to7000"  ,"SigPiPlusPiMinus_M3p0_high_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau10000to10000" ,"SigPiPlusPiMinus_M3p0_high_ctau10000"));
  } 
//--------------M3p0_low
  else if (signalType=="M3p0_low"){
    names.push_back(std::pair<TString,TString>("Parking_2018","Data"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to1"     ,"SigPi0_M3p0_low_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to3"     ,"SigPi0_M3p0_low_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to5"     ,"SigPi0_M3p0_low_ctau5")); 
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to7"     ,"SigPi0_M3p0_low_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to10"    ,"SigPi0_M3p0_low_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to30"    ,"SigPi0_M3p0_low_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to50"    ,"SigPi0_M3p0_low_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to70"    ,"SigPi0_M3p0_low_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to100"   ,"SigPi0_M3p0_low_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to300"   ,"SigPi0_M3p0_low_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to500"   ,"SigPi0_M3p0_low_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to700"   ,"SigPi0_M3p0_low_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to1000"  ,"SigPi0_M3p0_low_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to3000"  ,"SigPi0_M3p0_low_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to5000"  ,"SigPi0_M3p0_low_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to7000"  ,"SigPi0_M3p0_low_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pi0+"3p0_ctau3000to10000" ,"SigPi0_M3p0_low_ctau10000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to1"     ,"SigPiPlusPiMinus_M3p0_low_ctau1"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to3"     ,"SigPiPlusPiMinus_M3p0_low_ctau3"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to5"     ,"SigPiPlusPiMinus_M3p0_low_ctau5"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to7"     ,"SigPiPlusPiMinus_M3p0_low_ctau7"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to10"    ,"SigPiPlusPiMinus_M3p0_low_ctau10"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to30"    ,"SigPiPlusPiMinus_M3p0_low_ctau30"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to50"    ,"SigPiPlusPiMinus_M3p0_low_ctau50"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to70"    ,"SigPiPlusPiMinus_M3p0_low_ctau70"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to100"   ,"SigPiPlusPiMinus_M3p0_low_ctau100"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to300"   ,"SigPiPlusPiMinus_M3p0_low_ctau300"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to500"   ,"SigPiPlusPiMinus_M3p0_low_ctau500"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to700"   ,"SigPiPlusPiMinus_M3p0_low_ctau700"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to1000"  ,"SigPiPlusPiMinus_M3p0_low_ctau1000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to3000"  ,"SigPiPlusPiMinus_M3p0_low_ctau3000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to5000"  ,"SigPiPlusPiMinus_M3p0_low_ctau5000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to7000"  ,"SigPiPlusPiMinus_M3p0_low_ctau7000"));
    names.push_back(std::pair<TString,TString>(s_prefix_Pihad+"3p0_ctau3000to10000" ,"SigPiPlusPiMinus_M3p0_low_ctau10000"));
  } 
  else {std::cout<<"bad signalType"<<std::endl;}
  // 0=A, 1=B, 2=C, 3=D
  float y_bkg[4];
  float y_s[4];
  float y_s_uw[4];
  TString var_uw = var+"_uw";
  //std::cout<<var_uw<<std::endl;
  for(int i=0; i<names.size(); i++ ) {
    if(Type=="IT"){
      histos_FailN.push_back(getHisto("Fail",names[i].first, var));      //Fails NRecHit
      histos_PassN.push_back(getHisto("Pass"  ,names[i].first, var));    //Pass  NRecHit
      histos_FailN_uw.push_back(getHisto("Fail",names[i].first, var_uw));      //Fails NRecHit
      histos_PassN_uw.push_back(getHisto("Pass"  ,names[i].first, var_uw));    //Pass  NRecHit
    }
    else if(Type=="OOT"){
      histos_FailN.push_back(getHisto("FailOOT",names[i].first, var));   //Pass NRecHit
      histos_PassN.push_back(getHisto("PassOOT"  ,names[i].first, var)); //Fails NRecHit
      histos_FailN_uw.push_back(getHisto("FailOOT",names[i].first, var_uw));   //Pass NRecHit
      histos_PassN_uw.push_back(getHisto("PassOOT"  ,names[i].first, var_uw)); //Fails NRecHit
    }
    else {std::cout<<"Incorrect Type"<<std::endl; break;}
  }
  float y_A = histos_PassN[0]->GetBinContent(2);  //PASS NrecHits PASS V2(dPhi) data
  float y_B = histos_FailN[0]->GetBinContent(2);  //PASS NrecHits FAIL V2(dPhi) data
  float y_C = histos_PassN[0]->GetBinContent(1);  //FAIL NrecHits PASS V2(dPhi) data
  float y_D = histos_FailN[0]->GetBinContent(1);  //FAIL NrecHits FAIL V2(dPhi) data
     
  y_bkg[0] = histos_PassN[0]->GetBinContent(2);  //PASS NrecHits PASS V2(dPhi) data
  y_bkg[1] = histos_FailN[0]->GetBinContent(2);  //PASS NrecHits FAIL V2(dPhi) data
  y_bkg[2] = histos_PassN[0]->GetBinContent(1);  //FAIL NrecHits PASS V2(dPhi) data
  y_bkg[3] = histos_FailN[0]->GetBinContent(1);  //FAIL NrecHits FAIL V2(dPhi) data

  //Write histos to root file
  std::vector<TString> tableLine; 
  for(int i=1; i<names.size(); i++){
    float SF = 1.;
    if (SFmap.find(names[i].second) != SFmap.end()) SF = SFmap[names[i].second];
    float y_s_A = SF*histos_PassN[i]->GetBinContent(2); //PASS NrecHits PASS V2(dPhi) signal
    float y_s_B = SF*histos_FailN[i]->GetBinContent(2); //PASS NrecHits FAIL V2(dPhi) signal
    float y_s_C = SF*histos_PassN[i]->GetBinContent(1); //FAIL NrecHits PASS V2(dPhi) signal
    float y_s_D = SF*histos_FailN[i]->GetBinContent(1); //FAIL NrecHits FAIL V2(dPhi) signal
    y_s[0] = SF*histos_PassN[i]->GetBinContent(2); //PASS NrecHits PASS V2(dPhi) signal
    y_s[1] = SF*histos_FailN[i]->GetBinContent(2); //PASS NrecHits FAIL V2(dPhi) signal
    y_s[2] = SF*histos_PassN[i]->GetBinContent(1); //FAIL NrecHits PASS V2(dPhi) signal
    y_s[3] = SF*histos_FailN[i]->GetBinContent(1); //FAIL NrecHits FAIL V2(dPhi) signal
    y_s_uw[0] = histos_PassN_uw[i]->GetBinContent(2); //PASS NrecHits PASS V2(dPhi) signal
    y_s_uw[1] = histos_FailN_uw[i]->GetBinContent(2); //PASS NrecHits FAIL V2(dPhi) signal
    y_s_uw[2] = histos_PassN_uw[i]->GetBinContent(1); //FAIL NrecHits PASS V2(dPhi) signal
    y_s_uw[3] = histos_FailN_uw[i]->GetBinContent(1); //FAIL NrecHits FAIL V2(dPhi) signal
    //std::cout<<y_s_uw[0]<<" "<<y_s_uw[1]<<" "<<y_s_uw[2]<<" "<<y_s_uw[3]<<endl;
    //makeDataCard(Type,names[i].second, detector_region, y_B, y_C, y_D, y_s_A, y_s_B, y_s_C, y_s_D, theSF);
    makeDataCard(Type,names[i].second, detector_region, y_bkg, y_s, y_s_uw, theSF);
    tableLine.push_back(makeLine(detector_region,names[i].second, Type, y_A, y_B, y_C, y_D, y_s_A));
  }
  makeTable(detector_region,Type, tableLine);

}


void MakeInputs(TString type, TString region, TString m){

 doMakeInputs(type, region, m);
// doMakeInputs("OOT", "dt", "OOT");
//
// doMakeInputs("IT", "csc", "M0p3");
// doMakeInputs("IT", "dt", "M0p3");
//
// doMakeInputs("IT", "csc", "M0p5");
// doMakeInputs("IT", "dt", "M0p5");
//
// doMakeInputs("IT", "csc", "M1p0");
// doMakeInputs("IT", "dt", "M1p0");
//
// doMakeInputs("IT", "csc", "M1p0");
// doMakeInputs("IT", "dt", "M1p0");
//
// doMakeInputs("IT", "csc", "M3p0");
// doMakeInputs("IT", "dt", "M3p0");

}
