#include "cms_sus_19_005.h"
// AUTHOR: Andre Lessa
//  EMAIL: andre.lessa@ufabc.edu.br
void Cms_sus_19_005::initialize() {
  setAnalysisName("cms_sus_19_005");
  setInformation(""
    "# CMS search, inclusive hadronic search, with mT2.\n"
    "# Signal regions with disappearing tracks have not been implemented.\n"
  "");
  setLuminosity(137.0*units::INVFB);
  bookSignalRegions("2jloose;2jtight;4jloose;4jtight;7jloose;7jtight;10jloose;10jtight;2bloose;2btight;3bloose;3btight;4bloose;4btight;7j3bloose;7j3btight;7j4bloose;7j4btight;10j4bloose;10j4btight;monoPhi");
   bookCutflowRegions("0_Trigger;1_PreSelection");
  // You can also book cutflow regions with bookCutflowRegions("CR1;CR2;..."). Note that the regions are
  //  always ordered alphabetically in the cutflow output files.

  // You should initialize any declared variables here
}

void Cms_sus_19_005::analyze() {

  //Basic object definitions:
  //Jets:
  std::vector<Jet*> bjets;
  std::vector<Jet*> lightjets;
  std::vector<Jet*> widejets = filterPhaseSpace(jets, 30., -4.7, 4.7);
  std::vector<bool> bTagsMedium;
  for (int i = 0; i < jets.size(); ++i){
    if (jets[i]->PT < 20.0) continue;
    if (fabs(jets[i]->Eta) > 2.4) continue;
    if (tagBJetMedium(jets[i])){
        bjets.push_back(jets[i]);
    }
    else if (jets[i]->PT > 30.){
        lightjets.push_back(jets[i]);
    }
  }

  //Leptons:
  //Muons and Electrons:
  vector<Electron*> electrons;
  vector<Muon*> muons;
  electrons = filterIsolation(electronsLoose, 0);
  electrons = filterPhaseSpace(electrons, 10., -2.4, 2.4);
  muons = filterIsolation(muonsLoose, 0);
  muons = filterPhaseSpace(muons, 10., -2.4, 2.4);


  //Isolated tracks:
  int nIsolated = 0;
  for(int t = 0; t < tracks.size(); t++) {
      if (tracks[t]->PT < 10.0) continue;
      if (fabs(tracks[t]->Eta) > 2.5) continue;
      double Isol = 0.0;
      for(int tj = 0; tj < tracks.size(); tj++) {
          if (tj == t) continue;
          //Compute amount of track activity around track
          if (tracks[tj]->P4().DeltaR(tracks[t]->P4()) < 0.3){
              Isol += tracks[tj]->PT;
          }
      }
      if (Isol < 0.1*tracks[t]->PT){
          ++nIsolated;
      }
  }

  //Kinematical variables:
  missingET->addMuons(muonsCombined);  // Adds muons to missing ET.
  std::vector<double> HTmissV = {0.0,0.0};
  std::vector<Jet*> alljets;
  alljets.insert(alljets.end(), lightjets.begin(), lightjets.end());
  alljets.insert(alljets.end(), bjets.begin(), bjets.end());
  double HT = 0.0;
  double maxJetPT = 0.0;
  for (int i = 0; i < alljets.size(); ++i){
      HTmissV[0] -= alljets[i]->P4().X();
      HTmissV[1] -= alljets[i]->P4().Y();
      HT += alljets[i]->PT;
      if (alljets[i]->PT > maxJetPT) maxJetPT = alljets[i]->PT;
  }
  double HTmiss = sqrt(pow(HTmissV[0],2)+pow(HTmissV[1],2));
  double pTmiss = missingET->P4().Pt();
  double mT2jet = 0.;
  if (alljets.size() >= 2){
      //Compute 4-momentum of pseudo-jets
      std::vector<TLorentzVector> pseudoJets;
      pseudoJets = getPseudoJets(alljets);
      //Compute MT2 with pseudoJets (if clustering was successful)
      if (pseudoJets[0].E() > 0.0 && pseudoJets[1].E() > 0.0){
          mT2jet = mT2(pseudoJets[0], pseudoJets[1], 0., missingET->P4());
      }
  }

 //DeltaPhi
 double dPhiMin = 100.0;
 double dPhi;
 for (int i = 0; i < min(4,int(widejets.size())); ++i){
     dPhi = fabs(widejets[i]->P4().DeltaPhi(missingET->P4()));
     dPhiMin = min(dPhiMin,dPhi);
 }
 //pTmiss-HTmiss/pTmiss:
 double rMET;
 std::vector<double> diffMETV = {0.0,0.0};
 diffMETV[0] = missingET->P4().X()-HTmissV[0];
 diffMETV[1] = missingET->P4().Y()-HTmissV[1];
 rMET = sqrt(pow(diffMETV[0],2)+pow(diffMETV[1],2))/pTmiss;

 //Object's sizes:
 int Nj = lightjets.size();
 int Nl = electrons.size() + muons.size() + nIsolated;
 int Nb = bjets.size();

 //Event pre-selection
 bool passTrigger = false;
 //Trigger (2017-2018)
 if (pTmiss > 120.0 && HTmiss > 120.) passTrigger = true;
 if (HT > 60.0 && pTmiss > 120.0 && HTmiss > 120.) passTrigger = true;
 if (HT > 500.0 && pTmiss > 100.0 && HTmiss > 100.) passTrigger = true;
 if (HT > 800.0 && pTmiss > 75.0 && HTmiss > 75.) passTrigger = true;
 if (HT > 1050.0 || maxJetPT > 500.0) passTrigger = true;

 if (!passTrigger) return;
 countCutflowEvent("0_Trigger");

 bool pass = true;
 //Global cuts
 if (HT < 250.0) pass = false;
 if (pTmiss < 30.0) pass = false;
 if (Nl > 0) pass = false;
 if (Nj == 0) pass = false;
 else if (Nj == 1 && (maxJetPT < 250.0  || pTmiss < 250.0))
    pass = false;
 else if (Nj >= 2 && mT2jet < 200.0) pass = false;
 if (HT < 1200.0 && pTmiss < 250.0) pass = false;
 if (dPhiMin < 0.3) pass = false;
 if (rMET > 0.5) pass = false;

 if (!pass) return;
 countCutflowEvent("1_PreSelection");

 //Super (aggregated) signal regions
 if (Nj >= 2 && HT > 1200.0 && mT2jet > 1200.0)
     countSignalEvent("2jloose");
 if (Nj >= 2 && HT > 1500.0 && mT2jet > 1400.0)
     countSignalEvent("2jtight");
 if (Nj >= 4 && HT > 1200.0 && mT2jet > 1000.0)
     countSignalEvent("4jloose");
 if (Nj >= 4 && HT > 1500.0 && mT2jet > 1400.0)
     countSignalEvent("4jtight");
 if (Nj >= 7 && HT > 1200.0 && mT2jet > 600.0)
     countSignalEvent("7jloose");
 if (Nj >= 7 && HT > 1500.0 && mT2jet > 800.0)
     countSignalEvent("7jtight");
 if (Nj >= 10 && HT > 1200.0 && mT2jet > 400.0)
     countSignalEvent("10jloose");
 if (Nj >= 10 && HT > 1500.0 && mT2jet > 600.0)
     countSignalEvent("10jtight");

 if (Nj >= 2 && Nb >= 2 && HT > 1200.0 && mT2jet > 600.0)
     countSignalEvent("2bloose");
 if (Nj >= 2 && Nb >= 2 && HT > 1500.0 && mT2jet > 600.0)
     countSignalEvent("2btight");
 if (Nj >= 2 && Nb >= 3 && HT > 1200.0 && mT2jet > 400.0)
     countSignalEvent("3bloose");
 if (Nj >= 2 && Nb >= 3 && HT > 1500.0 && mT2jet > 400.0)
     countSignalEvent("3btight");
 if (Nj >= 2 && Nb >= 4 && HT > 1200.0 && mT2jet > 400.0)
     countSignalEvent("4bloose");
 if (Nj >= 2 && Nb >= 4 && HT > 1500.0 && mT2jet > 400.0)
     countSignalEvent("4btight");

 if (Nj >= 7 && Nb >= 3 && HT > 1200.0 && mT2jet > 400.0)
     countSignalEvent("7j3bloose");
 if (Nj >= 7 && Nb >= 3 && HT > 1500.0 && mT2jet > 400.0)
     countSignalEvent("7j3btight");
 if (Nj >= 7 && Nb >= 4 && HT > 1200.0 && mT2jet > 400.0)
     countSignalEvent("7j4bloose");
 if (Nj >= 7 && Nb >= 4 && HT > 1500.0 && mT2jet > 400.0)
     countSignalEvent("7j4btight");
 if (Nj >= 10 && Nb >= 4 && HT > 1200.0 && mT2jet > 400.0)
     countSignalEvent("10j4bloose");
 if (Nj >= 10 && Nb >= 4 && HT > 1500.0 && mT2jet > 400.0)
     countSignalEvent("10j4btight");

 if ( 1 <= Nj <= 3 && Nb == 0 && 250.0 < HT < 450.){
     if (Nj == 1) countSignalEvent("monoPhi");
     else if (200.0 < mT2jet < 300.0) countSignalEvent("monoPhi");
 }

 return;
}

void Cms_sus_19_005::finalize() {
  // Whatever should be done after the run goes here
}

//B-tagging based on CSVv2 algorithm
//(from https://twiki.cern.ch/twiki/bin/view/CMSPublic/SUSMoriond2017ObjectsEfficiency)
bool Cms_sus_19_005::tagBJetMedium(Jet *cand) {

    const double DR_B_TRUTH = 0.2;
    const double PTMIN_B_TRUTH = 10.0;
    const double ETAMAX_B_TRUTH = 2.4;

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
                eff = 0.15; //medium mistagging
                break;
            }
      }
    }
    // If no b and no c overlap, use light jet Rej
    if (eff == 0.0) eff = 0.02; // medium light jet mistagging

    if (prob < eff)  return true;
    else return false;
}


//Get efficiency for a given pT for a binned data set.
double Cms_sus_19_005::getEffFromData(vector<vector<double>> effData,
                                                double pt){

    int npts = effData.size();

    for (int i = 0; i < npts; ++i){
        if (pt > effData[i][0] && pt <= effData[i][1])
            return effData[i][2];
     }
     return 0.0;
}

//Group jets into two pseudo jets using the Lund distance measure. If clustering fails return zero 4-vectors.
std::vector<TLorentzVector> Cms_sus_19_005::getPseudoJets(std::vector<Jet*> jets){

    //Get initial seed jets
    std::vector<int> clusterA = {0};
    std::vector<int> clusterB = {1};
    double mjj, mjjmax;
    mjjmax = 0.0;
    for (int i = 0; i < jets.size(); ++i){
        for (int j = 0; j < jets.size(); ++j){
            if (i >= j) continue;
            mjj = (jets[i]->P4()+jets[j]->P4()).M();
            if (mjj > mjjmax){
                mjjmax = mjj;
                clusterA[0] = i;
                clusterB[0] = j;
            }
        }
    }
    int nint = 0;
    std::vector<int> newclusterA;
    std::vector<int> newclusterB;
    while (newclusterA != clusterA || newclusterB != clusterB){
        if (nint > 20) {
            cout << "Too many iterations when clustering jets" << endl;
            // cout << "Nj = " << jets.size() << endl;
            // cout << "clusterA: " << clusterA.size()  << endl;
            // for (int i = 0; i < clusterA.size(); ++i){cout << "  " << clusterA[i] << endl;}
            // cout << "clusterB: " << clusterB.size()  << endl;
            // for (int i = 0; i < clusterB.size(); ++i){cout << "  " << clusterB[i] << endl;}
            // cout << endl;
            // cout << "newclusterA: " << newclusterA.size()  << endl;
            // for (int i = 0; i < newclusterA.size(); ++i){cout << "  " << newclusterA[i] << endl;}
            // cout << "newclusterB: " << newclusterB.size()  << endl;
            // for (int i = 0; i < newclusterB.size(); ++i){cout << "  " << newclusterB[i] << endl;}
            clusterA = {}; //Return empty clusters
            clusterB = {};
            break;
        }
        //Update clusters:
        if (nint != 0) {
            clusterA = newclusterA;
            clusterB = newclusterB;
            newclusterA = {};
            newclusterB = {};
        }
        // cout << "  nint = " << nint << endl;
        // cout << "  clusterA: " << clusterA.size()  << endl;
        // for (int i = 0; i < clusterA.size(); ++i){cout << "    " << clusterA[i] << endl;}
        // cout << "  clusterB: " << clusterB.size()  << endl;
        // for (int i = 0; i < clusterB.size(); ++i){cout << "    " << clusterB[i] << endl;}


        //Cluster all jets into each seed jet according to Lund distance
        double d0i = 0;
        double d1i = 0;
        TLorentzVector pA(0.,0.,0.,0.);
        TLorentzVector pB(0.,0.,0.,0.);
        //Compute total momentum for each cluster:
        for (int i = 0; i < clusterA.size(); ++i){
            pA += jets[clusterA[i]]->P4();
        }
        for (int i = 0; i < clusterB.size(); ++i){
            pB += jets[clusterB[i]]->P4();
        }
        double EA = pA.E();
        double EB = pB.E();
        double Ep, cosA, cosB, dA, dB;
        TLorentzVector p;
        for (int i = 0; i < jets.size(); ++i){
            p = jets[i]->P4();
            Ep = p.E();
            cosA = (pA.X()*p.X()+pA.Y()*p.Y()+pA.Z()*p.Z())/(p.P()*pA.P());
            cosB = (pB.X()*p.X()+pB.Y()*p.Y()+pB.Z()*p.Z())/(p.P()*pB.P());
            dA = (EA - pA.P()*cosA)*EA/pow(EA+Ep,2); // Lund distance to clusterA
            dB = (EB - pB.P()*cosB)*EB/pow(EB+Ep,2); // Lund distance to clusterB
            if (dA < dB) newclusterA.push_back(i);
            else newclusterB.push_back(i);
        }
        ++nint;
    }

    //Compute total momentum for each cluster:
    TLorentzVector pA(0.,0.,0.,0.);
    TLorentzVector pB(0.,0.,0.,0.);
    for (int i = 0; i < clusterA.size(); ++i){
        pA += jets[clusterA[i]]->P4();
    }
    for (int i = 0; i < clusterB.size(); ++i){
        pB += jets[clusterB[i]]->P4();
    }

    std::vector<TLorentzVector> pJets = {pA,pB};

    return pJets;

}
