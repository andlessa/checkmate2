#include "cms_sus_16_032.h"
#include <algorithm>
#include <string>
#include <iostream>
// AUTHOR: Andre Lessa
//  EMAIL: andre.lessa@ufabc.edu.br
void Cms_sus_16_032::initialize() {
  setAnalysisName("cms_sus_16_032");
  setInformation(""
    "# cms\n"
    "# cms-sus-16-032\n"
    "# b-jets and c-jets plus MET\n"
  "");
  setLuminosity(35.9*units::INVFB);
  bookSignalRegions("HT_200_MCT_150;HT_200_MCT_250;HT_200_MCT_350;HT_200_MCT_450;HT_500_MCT_150;HT_500_MCT_250;HT_500_MCT_350;HT_500_MCT_450;HT_500_MCT_600;HT_1000_MCT_150;HT_1000_MCT_250;HT_1000_MCT_350;HT_1000_MCT_450;HT_1000_MCT_600;HT_1000_MCT_800;1b_ETmiss_250;1b_ETmiss_300;1b_ETmiss_500;1b_ETmiss_750;1b_ETmiss_1000;2b_ETmiss_250;2b_ETmiss_250_HT_100;2b_ETmiss_300;2b_ETmiss_300_HT_100;2b_ETmiss_500;2b_ETmiss_500_HT_100;1c_ETmiss_250;1c_ETmiss_300;1c_ETmiss_500;1c_ETmiss_750;1c_ETmiss_1000;2c_ETmiss_250;2c_ETmiss_250_HT_100;2c_ETmiss_300;2c_ETmiss_300_HT_100;2c_ETmiss_500;2c_ETmiss_500_HT_100;2c_ETmiss_750;2c_ETmiss_750_HT_100;0b_ETmiss_300;0b_ETmiss_500;0b_ETmiss_750;0b_ETmiss_1000;0b_ETmiss_1250;NSV_ETmiss_250;NSV_ETmiss_300;NSV_ETmiss_500;NSV_ETmiss_750;NSV_ETmiss_1000");

  bookCutflowRegions("Comp_00_total;Comp_01_MET;Comp_02_Njet;Comp_03_MET2;Comp_04_JetReq;Comp_05_LeptonVeto;Comp_06_DeltaPhi;Comp_07_pTISR;Comp_08_rISR;Comp_09_NSum;Comp_10_NSV0;");

  // Book Histograms (hists must be defined in the corresponding .h file)
  // TH1F *hNc = new TH1F("Nc","", 100, 0., 10.);
  // TH1F *hHTc = new TH1F("HTc","", 100, 0., 1000.);
  // TH1F *hNcCut = new TH1F("NcCut","", 100, 0., 10.);
  // TH1F *hMET = new TH1F("MET","", 100, 0., 1000.);
  //
  // hists.push_back(hNc);
  // hists.push_back(hHTc);
  // hists.push_back(hNcCut);
  // hists.push_back(hMET);
}

void Cms_sus_16_032::analyze() {
  // Your eventwise analysis code goes here
  // The following objects are always defined unless they are 'ignored' above. They form std::vector objects of the respective Delphes class type (except for Etmiss which is a single object)
  // All std::vector members and etmiss have the common properties PT, Eta, Phi and P4() with the latter giving access to the full ROOT TLorentzVector.
  // Within a std::vector, all members are ordered with highest pt coming first.

  // electronsLoose, electronsMedium, electronsTight   are list of electrons that passed respective efficiency and reconstruction cuts
  // muonsCombinedPlus, muonsCombined                  as above for muons
  // photonsMedium                                     as above for photons
  // jets are all reconstructed jets                   as above for jets. Note that electrons are most certainly also reconstructed as a jet -> overlap removal do avoid double counting necessary!
  // tracks, towers                                    calorimeter and tracker information. Usually not needed.
  // missingET                                         rec missing ET EXCLUDING muons.


  // Here is a couple of useful functions and lines:
  //------------Phase Space Cuts (defined for jets, electronsXYZ, muonsXYZ, photonsXYZ)
  // jets = filterPhaseSpace(jets, 20., -2.8, 2.8)  // The vector 'jets' only contains jets with pt >= 20 GeV and -2.8 < eta < 2.8. This function is applicable to other particles too (electronsMedium, ... ).
  // jets = overlapRemoval(jets, electronsLoose, 0.2) Removes all jets for which there exists any electron in 'electronsLoose' with deltaR < 0.2.
  // jets = overlapRemovel(jets, 0.2) If two jets overlap within deltaR < 0.2, only the harder jet is stored.

  //------------Isolation Checks (defined for electronsXYZ, muonsXYZ, photonsXYZ
  //------------        For each object, if the user entered N isolation conditions, they can be
  //------------        checked individually be the second argument (running from 0 to N-1).
  // electronsMedium = filterIsolation(electronsMedium, 0)            Removes electrons that do not pass the first isolation condition entered into the AnalysisManager by the user
  // std::vector<int> flags; flags.push_back(0); flags.push_back(2);
  // electronsMedium = filterIsolation(electronsMedium, flags)        Same as above, but both the first and the third condition have to be fulfilled
  // electronsMedium = filterIsolation(electronsMedium)               Same as above, but all conditions have to be fulfilled.

  //-----------Flavour Tag Checks (defined for jets only)
  //----------          Tau tags "loose", "medium" or "tight" can be individually checked if the user enabled tau tagging in the AM.
  //----------          For b-tags, if N working points have been defined, the ith condition can be tested by putting i-1 into the second argument (if there is only one, the argument can be omitted)
  // if checkTauTag(jets[0], "tight") leadingJetIsTagged = True;
  // if checkBTag(jets[0], 0) leadingJetIsBTagged = True;


  //-----------Auxiliary Information
  // - Always ensure that you don't access vectors out of bounds. E.g. 'if(jets[1]->PT > 150)' should rather be if (jets.size() > 1 && jets[1]->PT > 150).
  // - Use rand()/(RAND_MAX+1.) for random numbers between 0 and 1. The random seed is determined from system time or by the RandomSeed parameter in CheckMATE.
  // - The 'return' statement will end this function for the current event and hence should be called whenever the current event is to be vetoed.
  // - Many advanced kinematical functions like mT2 are implemented. Check the manual for more information.
  // - If you need output to be stored in other files than the cutflow/signal files we provide, check the manual for how to do this conveniently.

  //Basic object definitions:
  //Jets:
  jets = filterPhaseSpace(jets, 25., -2.4, 2.4);
  std::vector<Jet*> bjets;
  std::vector<Jet*> cjets;
  std::vector<Jet*> lightjets;
  std::vector<bool> bTagsLoose,bTagsMedium;
  std::vector<bool> cTagsMedium;
  for (int i = 0; i < jets.size(); ++i){
      if (tagBJet(jets[i],"loose"))
        bTagsLoose.push_back(true);
      else  bTagsLoose.push_back(false);
      if (tagBJet(jets[i],"medium"))
        bTagsMedium.push_back(true);
      else  bTagsMedium.push_back(false);
      if (tagCJet(jets[i]) &&
          !(bTagsMedium[i] || bTagsLoose[i]))
         cTagsMedium.push_back(true);
      else cTagsMedium.push_back(false);

      if (jets[i]->PT < 500. && bTagsMedium[i]){
         bjets.push_back(jets[i]);
      }
      else if (jets[i]->PT >= 500. && bTagsLoose[i]){
         bjets.push_back(jets[i]);
      }
      else if (tagCJet(jets[i])){
         cjets.push_back(jets[i]);
      }
      else {
         lightjets.push_back(jets[i]);
      }
  }

  //SV (secondary vertex):
  int NSV = 0;
  double prob;
  double SVeff = 0.2;
  double dR;
  for (int i = 0; i< true_b.size(); ++i){
      //CMS-SUS-16-049 claims the  20% SV tagging efficiency is valid for the 10 GeV-20 GeV range:
      if (true_b[i]->PT > 25.0 || true_b[i]->PT < 10.0) continue;
      dR = 1.0;
      for (int j = 0; j < bjets.size(); ++j){
          dR = std::min(dR,true_b[i]->P4().DeltaR(bjets[j]->P4()));
      }
      for (int j = 0; j < cjets.size(); ++j){
          dR = std::min(dR,true_b[i]->P4().DeltaR(cjets[j]->P4()));
      }
      for (int j = 0; j < lightjets.size(); ++j){
          dR = std::min(dR,true_b[i]->P4().DeltaR(lightjets[j]->P4()));
      }
      if (dR < 0.4) continue;
      prob = rand()/(RAND_MAX+1.);
      if (prob < SVeff) ++NSV;
  }

  //ISR system:
  vector<Jet*> ISR;
  if (jets.size() == 1){
      if (bjets.size()+cjets.size() == 0){
          ISR.push_back(jets[0]); //1 jet (not b or c tagged)
      }
  }
  else if (jets.size() > 1){
      if ((jets[1]->PT >= 500 && bTagsLoose[1])
        || (jets[1]->PT < 500 && bTagsMedium[1])){
          if (!bTagsLoose[0])
              ISR.push_back(jets[0]); // leading jet is not b-tagged, but sub-leading is
      }
      else if (cTagsMedium[1]){
          if (!cTagsMedium[0])
              ISR.push_back(jets[0]); // leading jet is not c-tagged, but sub-leading is
      }
      else {
          ISR.push_back(jets[0]); // sub-leading is not b or c-tagged
          if (jets[1]->PT > 50)
            ISR.push_back(jets[1]); //sub-leading has pT > 50
      }
  }

  //Muons and Electrons:
  vector<Electron*> electrons;
  vector<Muon*> muons;
  electrons = filterIsolation(electronsLoose, 0);
  electrons = filterPhaseSpace(electrons, 10., -2.4, 2.4);
  muons = filterIsolation(muonsLoose, 0);
  muons = filterPhaseSpace(muons, 10., -2.4, 2.4);
  //Taus:
  int ntaus = 0;
  for (int i = 0; i < tracks.size(); ++i){
      if (tracks[i]->PT < 10.0 || fabs(tracks[i]->Eta) > 2.5)
        continue;
      double Iso = 0.;
      for (int j = 0; j < tracks.size(); ++j){
          if (i == j) continue;
          if (tracks[i]->P4().DeltaR(tracks[j]->P4()) < 0.3)
            continue;
          Iso += tracks[j]->PT;
      }
      if (Iso < 0.1*tracks[i]->PT) ++ntaus;
  }

  // //Kinematical variables
  missingET->addMuons(muonsCombined);  // Adds muons to missing ET.
  double HT = 0., HTb = 0., HTc = 0.;
  for (int i = 0; i < min(2,int(jets.size())); ++i){
      HT += jets[i]->PT;
  }
  for (int i = 0; i < min(2,int(bjets.size())); ++i){
      HTb += bjets[i]->PT;
  }
  for (int i = 0; i < min(2,int(cjets.size())); ++i){
      HTc += cjets[i]->PT;
  }
  double deltaPhiMin = 0.;
  double deltaPhi, mT;
  for (int i = 0; i < min(3,int(jets.size())); ++i){
      deltaPhi = fabs(jets[i]->P4().DeltaPhi(missingET->P4()));
      if (i == 0) deltaPhiMin = deltaPhi;
      else deltaPhiMin = min(deltaPhiMin,deltaPhi);
  }
  double MET = missingET->P4().Pt();
  double mTmin = 0.;
  for (int i = 0; i < min(2,int(jets.size())); ++i){
      deltaPhi = fabs(jets[i]->P4().DeltaPhi(missingET->P4()));
      mT = sqrt(2.*jets[i]->PT*MET*(1.-cos(deltaPhi)));
      if (i == 0) mTmin = mT;
      else mTmin = min(mTmin,mT);
  }
  double mCT = 0;
  if (jets.size() >= 2){
      deltaPhi = fabs(jets[0]->P4().DeltaPhi(jets[1]->P4()));
      mCT = sqrt(2.*jets[0]->PT*jets[1]->PT*(1.+cos(deltaPhi)));
  }

  TLorentzVector p4ISR(0.,0.,0.,0.);
  for(int iISR = 0; iISR < ISR.size(); ++ iISR){;
      p4ISR += ISR[iISR]->P4();
  }
  double pTISR = p4ISR.Pt();
  double rISR = (p4ISR+missingET->P4()).Pt()/MET;

  // hists[0]->Fill(pTISR);
  // hists[1]->Fill(HTc);


  //Signal Regions
  //Event Pre-Selection:
  //Non-compressed region:
  countCutflowEvent("NonComp_00_total");
  countCutflowEvent("Comp_00_total");
  bool NCselection = true;
  //Compressed region
  bool Cselection = true;
  if (MET < 200.) {
      NCselection = false;
      Cselection = false;
  }
  if (NCselection) countCutflowEvent("NonComp_01_MET");
  if (Cselection) countCutflowEvent("Comp_01_MET");

  if (jets.size() < 2) {
      NCselection = false;
      Cselection = false;
  }
  if (NCselection) countCutflowEvent("NonComp_02_Njet");
  if (Cselection) countCutflowEvent("Comp_02_Njet");

  if (MET < 250.) {
      NCselection = false;
      Cselection = false;
  }
  if (NCselection) countCutflowEvent("NonComp_03_MET2");
  if (Cselection) countCutflowEvent("Comp_03_MET2");

  if (jets.size() >= 5 && jets[4]->PT > 75.0) {
      NCselection = false;
      Cselection = false;
  }
  else if (jets.size() < 2 || jets[0]->PT < 100.0
            || jets[1]->PT < 75.0) {
        NCselection = false;
  }
  if (NCselection) countCutflowEvent("NonComp_04_JetReq");
  if (Cselection) countCutflowEvent("Comp_04_JetReq");

  if (muons.size() > 0 || electrons.size() > 0 || ntaus > 0){
      NCselection = false;
      Cselection = false;
  }
  if (NCselection) countCutflowEvent("NonComp_05_LeptonVeto");
  if (Cselection) countCutflowEvent("Comp_05_LeptonVeto");

  if (deltaPhiMin < 0.4) {
      NCselection = false;
      Cselection = false;
  }
  if (NCselection) countCutflowEvent("NonComp_06_DeltaPhi");
  if (Cselection) countCutflowEvent("Comp_06_DeltaPhi");

  //Noncompressed region specific selection:
  if (NCselection){
      if (mTmin < 250.0) NCselection = false;
      else if (NCselection) countCutflowEvent("NonComp_07_MTmin");
      if (bjets.size() < 2) NCselection = false;
      else if (bjets[0]->PT != jets[0]->PT)
          NCselection = false;
      else {
          if (NCselection) countCutflowEvent("NonComp_08_Nb1");
          if (bjets[1]->PT != jets[1]->PT) NCselection = false;
          else if (NCselection) countCutflowEvent("NonComp_09_Nb2");
      }
      if (mCT < 150.0) NCselection = false;
      else if (NCselection) countCutflowEvent("NonComp_10_MCT");
  }

  // //Compressed region specific selection:
  if (Cselection){
      if (jets[0]->PT < 100.) Cselection = false;
      if (jets[1]->PT < 25.) Cselection = false;
      if (bjets.size() > 0 && bjets[0]->PT == jets[0]->PT)
          Cselection = false;
      if (cjets.size() > 0 && cjets[0]->PT == jets[0]->PT)
          Cselection = false;
      if ((bjets.size() > 0 && jets[1]->PT == bjets[0]->PT)
          || (cjets.size() > 0 && jets[1]->PT == cjets[0]->PT)){
              if (jets[1]->PT < 25.0) Cselection = false;
      }
      else if (jets[1]->PT < 50.0) Cselection = false;
      if (pTISR < 250.0) Cselection = false;
      if (Cselection) countCutflowEvent("Comp_07_pTISR");
      if (rISR > 0.5) Cselection = false;
      if (Cselection) countCutflowEvent("Comp_08_rISR");
      if (bjets.size() + cjets.size() + NSV > 0){
          if (Cselection) countCutflowEvent("Comp_09_NSum");
          if (NSV == 0 && Cselection)
            countCutflowEvent("Comp_10_NSV0");
      }
  }

  // //Noncompressed signal regions:
  string SR;
  if (NCselection){
      vector<double> SR_HT = {200,500,1000};
      vector<double> SR_MCT = {150,250,350,450,600,800};
      for (int iht = 0; iht < SR_HT.size(); ++iht){
          if (HT < SR_HT[iht]) continue;
          if (iht < SR_HT.size()-1 && HT >= SR_HT[iht+1])
              continue;
          for (int imct = 0; imct < iht+4; ++imct){
              if (mCT < SR_MCT[imct]) continue;
              if (imct < iht+3 && mCT >= SR_MCT[imct+1]) continue;
              SR = "HT_"+to_string(int(SR_HT[iht]));
              SR += "_MCT_"+to_string(int(SR_MCT[imct]));
              countSignalEvent(SR);
              break;
          }
      }
  }

  // //Compressed regions:
  if (Cselection){
      vector<double> SR_MET;
      if (bjets.size() == 1 && HTb < 100.){
          SR_MET = {250,300,500,750,1000};
          for (int i = 0; i < SR_MET.size(); ++i){
              if (MET < SR_MET[i]) continue;
              if (i < SR_MET.size()-1 && MET >= SR_MET[i+1])
                  continue;
              SR =  "1b_ETmiss_"+to_string(int(SR_MET[i]));
              countSignalEvent(SR);
              break;
          }
      }
      else if (bjets.size() == 2 && HTb < 200.){
          SR_MET = {250,300,500};
          for (int i = 0; i < SR_MET.size(); ++i){
              if (MET < SR_MET[i]) continue;
              if (i < SR_MET.size()-1 && MET >= SR_MET[i+1])
                  continue;
              SR =  "2b_ETmiss_"+to_string(int(SR_MET[i]));
              if (HTb > 100) SR += "_HT_100";
              countSignalEvent(SR);
              break;
         }
      }
      else if (cjets.size() == 1 && HTc < 100.){
          SR_MET = {250,300,500,750,1000};
          for (int i = 0; i < SR_MET.size(); ++i){
              if (MET < SR_MET[i]) continue;
              if (i < SR_MET.size()-1 && MET >= SR_MET[i+1])
                  continue;
              SR =  "1c_ETmiss_"+to_string(int(SR_MET[i]));
              countSignalEvent(SR);
              break;
          }
      }
      else if (cjets.size() == 2 && HTc < 200.){
          SR_MET = {250,300,500,750};
          for (int i = 0; i < SR_MET.size(); ++i){
              if (MET < SR_MET[i]) continue;
              if (i < SR_MET.size()-1 && MET >= SR_MET[i+1])
                  continue;
              SR =  "2c_ETmiss_"+to_string(int(SR_MET[i]));
              if (HTc > 100) SR += "_HT_100";
              countSignalEvent(SR);
              break;
         }
      }
      else if (bjets.size()+cjets.size()+NSV == 0){
          SR_MET = {300,500,750,1000,1250};
          for (int i = 0; i < SR_MET.size(); ++i){
              if (MET < SR_MET[i]) continue;
              if (i < SR_MET.size()-1 && MET >= SR_MET[i+1])
                  continue;
              SR =  "0b_ETmiss_"+to_string(int(SR_MET[i]));
              countSignalEvent(SR);
              break;
          }
      }
      else if (bjets.size()+cjets.size() == 0 && NSV > 0){
          SR_MET = {250,300,500,750,1000};
          for (int i = 0; i < SR_MET.size(); ++i){
              if (MET < SR_MET[i]) continue;
              if (i < SR_MET.size()-1 && MET >= SR_MET[i+1])
                  continue;
              SR =  "NSV_ETmiss_"+to_string(int(SR_MET[i]));
              countSignalEvent(SR);
              break;
          }
      }
  }
  return;
}

void Cms_sus_16_032::finalize() {
  // Whatever should be done after the run goes here

  //Save histograms
  // for (int i = 0; i < hists.size(); ++ i){
  //       TH1F *h = hists[i];
  //       string filename(h->GetName());
  //       filename  += ".csv";
  //       FILE* histoFile = fopen(filename.c_str(), "w");
  //       double xbin,binContent;
  //       fprintf(histoFile,"#   X,   Bin\n");
  //       for (int j=1; j<=h->GetNbinsX();j++) {
  //           xbin = h->GetXaxis()->GetBinCenter(j);
  //           binContent = h->GetBinContent(j);
  //           fprintf(histoFile,"%1.3e, %1.3e\n",xbin,binContent);
  //       }
  // }


}


//B-tagging based on CSVv2 algorithm
//(from https://twiki.cern.ch/twiki/bin/view/CMSPublic/SUSMoriond2017ObjectsEfficiency)
bool Cms_sus_16_032::tagBJet(Jet *cand, string efficiency) {

    const double DR_B_TRUTH = 0.2;
    const double PTMIN_B_TRUTH = 10.0;
    const double ETAMAX_B_TRUTH = 2.4;

    vector<vector<double>> bTagEffLoose = {{30.0,35.0,0.7706},
        {35.0,40.0,0.7815},{40.0,50.0,0.7925},{50.0,60.0,0.8055},
        {60.0,70.0,0.8159},{70.0,80.0,0.8230},{80.0,90.0,0.8281},
        {90.0,100.0,0.8298},{100.0,125.0,0.8326},{125.0,150.0,0.8391},
        {150.0,175.0,0.8350},{175.0,200.0,0.8273},{200.0,225.0,0.8237},
        {225.0,250.0,0.8192},{250.0,275.0,0.8149},{275.0,300.0,0.8156},
        {300.0,350.0,0.8044},{350.0,400.0,0.7951},{400.0,450.0,0.7965},
        {450.0,500.0,0.7929},{500.0,600.0,0.7900},{600.0,700.0,0.7811},
        {700.0,800.0,0.7740},{800.0,1000.0,0.7899}};

    vector<vector<double>> bTagEffMedium = {{30.0,35.0,0.5735},
        {35.0,40.0,0.5872},{40.0,50.0,0.5999},{50.0,60.0,0.6159},
        {60.0,70.0,0.6277},{70.0,80.0,0.6352},{80.0,90.0,0.6402},
        {90.0,100.0,0.6415},{100.0,125.0,0.6429},{125.0,150.0,0.6457},
        {150.0,175.0,0.6353},{175.0,200.0,0.6165},{200.0,225.0,0.6051},
        {225.0,250.0,0.5938},{250.0,275.0,0.5871},{275.0,300.0,0.5771},
        {300.0,350.0,0.5663},{350.0,400.0,0.5425},{400.0,450.0,0.5296},
        {450.0,500.0,0.5085},{500.0,600.0,0.4840},{600.0,700.0,0.4820},
        {700.0,800.0,0.4663},{800.0,1000.0,0.4663}};

    double prob = rand()/(RAND_MAX+1.);
    double eff = 0.0;
    /* Loop over bs and try to find an overlap.
    * If there is one, use b signal efficiency*/
    for(int b = 0; b < true_b.size(); b++) {
      if(true_b[b]->PT > PTMIN_B_TRUTH &&
        fabs(true_b[b]->Eta) < ETAMAX_B_TRUTH &&
         true_b[b]->P4().DeltaR(cand->P4()) < DR_B_TRUTH) {
            if (efficiency == "loose")
              eff = getEffFromData(bTagEffLoose,cand->PT);
            else if (efficiency == "medium")
              eff = getEffFromData(bTagEffMedium,cand->PT);
            break;
      }
    }
    // If no b overlap, test with truth c's and maybe use c-efficiency
    if (eff == 0.0) {
      for(int c = 0; c < true_c.size(); c++) {
          if(true_c[c]->PT > PTMIN_B_TRUTH &&
             fabs(true_c[c]->Eta) < ETAMAX_B_TRUTH &&
             true_c[c]->P4().DeltaR(cand->P4()) < DR_B_TRUTH) {
                 if (efficiency == "loose")
                    eff = 0.4 ;//loose mistagging
                 else if (efficiency == "medium")
                    eff = 0.2; //medium mistagging
                 break;
            }
      }
    }
    // If no b and no c overlap, use light jet Rej
    if (eff == 0.0){
        if (efficiency == "loose") eff = 0.1; // loose light jet mistagging
        else if (efficiency == "medium") eff = 0.02; // medium light jet mistagging
    }
    if (prob < eff)  return true;
    else return false;
}

//B-tagging based on CSVv2 algorithm
//(from https://twiki.cern.ch/twiki/bin/view/CMSPublic/SUSMoriond2017ObjectsEfficiency)
bool Cms_sus_16_032::tagCJet(Jet *cand) {

    const double DR_C_TRUTH = 0.2;
    const double PTMIN_C_TRUTH = 10.0;
    const double ETAMAX_C_TRUTH = 2.4;

    double prob = rand()/(RAND_MAX+1.);
    double eff = 0.0;
    /* Loop over cs and try to find an overlap.
    * If there is one, use c signal efficiency*/
    for(int c = 0; c < true_c.size(); c++) {
        if(true_c[c]->PT > PTMIN_C_TRUTH &&
           fabs(true_c[c]->Eta) < ETAMAX_C_TRUTH &&
           true_c[c]->P4().DeltaR(cand->P4()) < DR_C_TRUTH) {
               eff = 0.4; //medium mistagging
               break;
          }
    }
    // If no c overlap, test with truth b's and maybe use b-efficiency
    if (eff == 0.0) {
        for(int b = 0; b < true_b.size(); b++) {
          if(true_b[b]->PT > PTMIN_C_TRUTH &&
            fabs(true_b[b]->Eta) < ETAMAX_C_TRUTH &&
            true_b[b]->P4().DeltaR(cand->P4()) < DR_C_TRUTH) {
                eff = 0.2; //b-jet mistagging
                break;
          }
       }
    }
    // If no b and no c overlap, use light jet Rej
    if (eff == 0.0){
        eff = 0.2; // medium light jet mistagging
    }
    if (prob < eff)  return true;
    else return false;

}

//Get efficiency for a given pT for a binned data set.
double Cms_sus_16_032::getEffFromData(vector<vector<double>> effData,
                                                double pt){

    int npts = effData.size();

    for (int i = 0; i < npts; ++i){
        if (pt > effData[i][0] && pt <= effData[i][1])
            return effData[i][2];
     }
     return 0.0;
}
