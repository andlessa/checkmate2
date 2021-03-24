#include "atlas_exot_2018_06.h"
// AUTHOR: Andre Lessa
//  EMAIL: lessa.a.p@gmail.com
void Atlas_exot_2018_06::initialize() {
  setAnalysisName("atlas_exot_2018_06");
  setInformation(""
    "# ATLAS search for new phenomena in events with an energetic jet and missing transverse momentum\n"
    "# at sqrts = 13 TeV and 139/fb\n"
  "");
  setLuminosity(139.0*units::INVFB);
  bookSignalRegions("EM0;EM1;EM2;EM3;EM4;EM5;EM6;EM7;EM8;EM9;EM10;EM11;EM12;IM0;IM1;IM2;IM3;IM4;IM5;IM6;IM7;IM8;IM9;IM10;IM11;IM12");
  // You can also book cutflow regions with bookCutflowRegions("CR1;CR2;..."). Note that the regions are
  //  always ordered alphabetically in the cutflow output files.

  // You should initialize any declared variables here
}

std::string Atlas_exot_2018_06::signal_regions [26] = {"EM0","EM1","EM2","EM3","EM4","EM5","EM6","EM7","EM8","EM9","EM10","EM11","EM12","IM0","IM1","IM2","IM3","IM4","IM5","IM6","IM7","IM8","IM9","IM10","IM11","IM12"};
int Atlas_exot_2018_06::signal_region_borders [26][2] = {{200,250},{250,300},{300,350},{350,400},{400,500},{500,600},{600,700},{700,800},{800,900},{900,1000},{1000,1100},{1100,1200},{1200,-1},{200,-1},{250,-1},{300,-1},{350,-1},{400,-1},{500,-1},{600,-1},{700,-1},{800,-1},{900,-1},{1000,-1},{1100,-1},{1200,-1}};


void Atlas_exot_2018_06::analyze() {
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

    missingET->addMuons(muonsCombined);

    jets = filterPhaseSpace(jets, 20., -2.8, 2.8);
    electronsLoose = filterPhaseSpace(electronsLoose, 7., -2.47, 2.47);
    muonsCombined = filterPhaseSpace(muonsCombined, 7., -2.5, 2.5);
    //Note: ATLAS uses a tight selection for photons (here we approximate by the medium selection)
    photonsMedium = filterPhaseSpace(photonsMedium, 10., -2.37, 2.37);
    photonsMedium  = overlapRemoval(photonsMedium, electronsLoose, 0.4);
    photonsMedium  = overlapRemoval(photonsMedium, muonsCombined, 0.4);

    std::vector<Jet*> tauJets = getTauJets();
    overlapRemoval_jetMuon();
    overlapRemoval_jetElectron();
    jets = overlapRemoval(jets, tauJets, 0.2);
    jets = overlapRemoval(jets, photonsMedium, 0.4);

    countCutflowEvent("0_nocuts");

    float MET = missingET->P4().Et();
    if (MET<150) return;
    countCutflowEvent("1_MET > 150");

    if( jets.size() < 1 ) return;
    countCutflowEvent("2_Njet > 0");

    if( electronsLoose.size() > 0 ) return;
    if( muonsCombined.size() > 0 ) return;
    if( tauJets.size() > 0 ) return;
    countCutflowEvent("3_Lepton veto");

    if( jets.size() > 4 ) return;
    countCutflowEvent("4_Njet <= 4");

    float dphi = fabs(dPhi(jets,missingET));
    if (MET < 250 && dphi < 0.6) return;
    if (MET > 250 && dphi < 0.4) return;
    countCutflowEvent("5_dPhi cut");

    if ( (jets[0]->PT < 150) || (abs(jets[0]->P4().Eta()) > 2.4) ) return;
    countCutflowEvent("6_Lead jet cut");

    if (MET < 200) return;
    countCutflowEvent("7_MET > 200");

    //Exclusive signa regions:
    for(int i=0;i<26;i++){
      if (MET < float(signal_region_borders[i][0])) continue;
      if (MET > float(signal_region_borders[i][1]) && signal_region_borders[i][1]!=-1) continue;
      countSignalEvent(signal_regions[i]);
    }
  }

//Muon Jet Overlap removal. Adapted from thorsten
void Atlas_exot_2018_06::overlapRemoval_jetMuon() {
      std::vector<Muon*> surviving_muons;
      std::vector<Jet*> surviving_jets;
      std::vector<bool> jet_overlaps(jets.size(), false);
      for (int i=0; i<muonsCombined.size(); i++) {
          bool muon_overlap = false;
          double minDR = 0.4;
          double minPT = 30.;
          for (int j=0; j<jets.size(); j++) {
              if (fabs((muonsCombined[i])->P4().DeltaR(jets[j]->P4())) >= minDR) {
                  continue;
              }
              if (jets[j]->PT < minPT) {
                  continue;
              }

              if (countMatchingTracks(tracks, jets[j]) >= 3) {
                  // remove muon
                  muon_overlap = true;
              } else {
                  // remove jet
                  jet_overlaps[j] = true;
              }
          }
          if (!muon_overlap) {
              surviving_muons.push_back(muonsCombined[i]);
          }
      }

      for (int i=0; i<jet_overlaps.size(); i++) {
          if (!(jet_overlaps[i])) {
              surviving_jets.push_back(jets[i]);
          }
      }
      jets = surviving_jets;
      muonsCombined = surviving_muons;
}

//Electron Jet Overlap removal. Adapted from thorsten
void Atlas_exot_2018_06::overlapRemoval_jetElectron() {
    std::vector<Electron*> surviving_electrons;
    std::vector<Jet*> surviving_jets;
    std::vector<bool> jet_overlaps(jets.size(), false);
    for (int i=0; i<electronsLoose.size(); i++) {
        bool electron_overlap = false;
        double minPT = 30.;
        for (int j=0; j<jets.size(); j++) {
            if (jets[j]->PT < minPT) {
                continue;
            }
            double DR = fabs((electronsLoose[i])->P4().DeltaR(jets[j]->P4()));

            if ((!checkBTag(jets[j], 0))) {
                //Remove light jets (not b tagged) close to electrons:
                if  (DR < 0.2 ) {jet_overlaps[j] = true;} // remove jet
                else if (DR < 0.4 ) {electron_overlap = true;} // remove electron
            } else {
                //If b-jet and the separation smaller than 0.4, remove electron
                if (DR < 0.4 ) {electron_overlap = true;}
            }
        }
        if (!electron_overlap) {
            surviving_electrons.push_back(electronsLoose[i]);
        }
    }

    for (int i=0; i<jet_overlaps.size(); i++) {
        if (!(jet_overlaps[i])) {
            surviving_jets.push_back(jets[i]);
        }
    }
    jets = surviving_jets;
    electronsLoose = surviving_electrons;
}

//Get tau jets and remove them from the jet list.
std::vector<Jet*> Atlas_exot_2018_06::getTauJets() {

  std::vector<Jet*> surviving_jets;
  std::vector<Jet*> tauJets;
  //First get tau jets:
  for (int i=0; i<jets.size(); i++) {
    if(checkTauTag(jets[i], "loose")) {
          tauJets.push_back(jets[i]);
    }
    else {surviving_jets.push_back(jets[i]);} //Non tau-jets
  }

  tauJets = filterPhaseSpace(tauJets,20.,-2.5,2.5);
  tauJets = overlapRemoval(tauJets, electronsLoose, 0.2);
  tauJets = overlapRemoval(tauJets, muonsCombinedPlus, 0.2);
  jets = surviving_jets;

  return tauJets;
}

//The original jet finder algorithm used a DeltaR of 0.4, so do that again.
//Only tracks with pT > 500 MeV are considered.
int Atlas_exot_2018_06::countMatchingTracks(std::vector<Track*> tracks, Jet* jet) {
  int ntracks = 0;
  for (int i=0; i < tracks.size(); i++) {
      if (jet->P4().DeltaR((tracks[i])->P4()) < 0.4 && (tracks[i]->PT > 0.5)) {
          ntracks++;
      }
  }
  return ntracks;
}

float Atlas_exot_2018_06::dPhi(std::vector<Jet*> jets, ETMiss* missingET){

    double dphimin=0;
    for(int k=0;k<std::min(int(jets.size()),4);k++){
      if( k==0 ){
        dphimin = fabs( jets[k]->P4().DeltaPhi( missingET->P4() ) );
      }else{
        dphimin = std::min(fabs( jets[k]->P4().DeltaPhi( missingET->P4() ) ),dphimin);
      }
    }
    return dphimin;
}

void Atlas_exot_2018_06::finalize() {
  // Whatever should be done after the run goes here
}
