#include "atlas_susy_2018_04.h"
// AUTHOR: Andre Lessa
//  EMAIL: andre.lessa@ufabc.edu.br


void Atlas_susy_2018_04::initialize() {
  setAnalysisName("atlas_susy_2018_04");
  setInformation(""
    "# ATLAS\n"
    "# ATLAS-SUSY-2018-04\n"
    "# 2 hadronic taus, etmiss, no b-jets\n"
    "# sqrt(s) = 13 TeV\n"
    "# int(L) = 139 fb^-1\n"
  "");
  setLuminosity(139.0*units::INVFB);
  ignore("towers");
  bookSignalRegions("SR-lowMass;SR-highMass");
  // You can also book cutflow regions with bookCutflowRegions("CR1;CR2;..."). Note that the regions are
  //  always ordered alphabetically in the cutflow output files.

  // You should initialize any declared variables here
}

void Atlas_susy_2018_04::analyze() {
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


	  jets = filterPhaseSpace(jets, 20., -2.8, 2.8);
	  electronsLoose = filterPhaseSpace(electronsLoose, 17., -2.47, 2.47);
	  electronsLoose = filterIsolation(electronsLoose);
	  muonsCombinedPlus = filterPhaseSpace(muonsCombinedPlus, 14., -2.7, 2.7);
	  muonsCombinedPlus = filterIsolation(muonsCombinedPlus);

	  int nTightTaus = 0;
	  int nMediumTaus = 0;
	  std::vector<Jet*> taujets;
	  std::vector<Jet*> bjets;
	  std::vector<Jet*> lightjets;
	  std::vector<bool> tauTags;


	  for (int i = 0; i < jets.size(); i++){
        //Recompute tau tagging:
        tauTags = getTauTags(jets[i],tracks,true_tau);

		if (!(tauTags[1] || tauTags[2] || checkBTag(jets[i], 0))){
			lightjets.push_back(jets[i]);
		}
		else if (fabs(jets[i]->Eta) < 2.5 && jets[i]->PT > 20.){
			if (tauTags[1] || tauTags[2]){
				taujets.push_back(jets[i]);
				if (tauTags[2]) ++nTightTaus;
				if (tauTags[1]) ++nMediumTaus;
			}
			else if(checkBTag(jets[i], 0) ) bjets.push_back(jets[i]);
	    }
	  }

	  electronsLoose  = overlapRemoval(electronsLoose, electronsLoose, 0.05);
	  taujets   = overlapRemoval(taujets, electronsLoose, 0.2);
	  taujets   = overlapRemoval(taujets, muonsCombinedPlus, 0.2);
	  electronsLoose  = overlapRemoval(electronsLoose, muonsCombinedPlus, 0.01);
	  lightjets		  = overlapRemoval(lightjets, electronsLoose, 0.2);
	  electronsLoose  = overlapRemoval(electronsLoose, lightjets, 0.4);
	  lightjets       = overlapRemoval(lightjets, muonsCombinedPlus, 0.2);
	  muonsCombinedPlus  = overlapRemoval(muonsCombinedPlus, jets, 0.4);
	  lightjets       = overlapRemoval(lightjets, taujets, 0.2);

	  missingET->addMuons(muonsCombinedPlus);  // Adds muons to missing ET. This should almost always be done which is why this line is not commented out.

	  //Event selection:
	  countCutflowEvent("0) Total");
	  if (taujets.size() < 2) return;
	  if (taujets[0]->PT < 50.0 || taujets[1]->PT < 40.) return;
	  if (nMediumTaus != 2 || (taujets[0]->Charge)*(taujets[1]->Charge) > 0) return;
	  countCutflowEvent("1-3) GF+BC+ 2 medium OS taus");
	  if (!bjets.empty()) return;
	  countCutflowEvent("4) b-jet veto");
	  if (!electronsLoose.empty() || !muonsCombinedPlus.empty()) return;
	  countCutflowEvent("5) light lepton veto");
	  double mtautau = (taujets[0]->P4() + taujets[1]->P4()).M();
	  if (mtautau < 120.) return;
	  countCutflowEvent("6) Z/H veto");

	  double met = missingET->P4().Et();
	  double deltaPhi = fabs(taujets[0]->P4().DeltaPhi(taujets[1]->P4()));
	  double deltaR = taujets[0]->P4().DeltaR(taujets[1]->P4());
	  double mT2tautau = mT2(taujets[0]->P4(), taujets[1]->P4(), 0., missingET->P4());

	  //Overall trigger eff:
	  double triggerEff = 0.8;
	  double random_trigger = (double) rand()/ (double) (RAND_MAX + 1.);
	  bool passTrigger = (random_trigger < triggerEff);


	  //SR-lowMass
	  //di-tau trigger:
	  if (taujets[0]->PT > 95.0 && taujets[0]->PT > 75.0 && passTrigger){
		  countCutflowEvent("LowMass: 1) Trigger");
		  if ((75. < met) && (met < 150.)){
			  countCutflowEvent("LowMass: 2) MET");
			  if (nTightTaus == 2){
				  countCutflowEvent("LowMass: 3) 2 tight taus");
				  if (deltaPhi > 0.8){
					  countCutflowEvent("LowMass: 4) Deltaphi");
					  if (deltaR < 3.2){
						  countCutflowEvent("LowMass: 5) DeltaR");
						  if (mT2tautau > 70.){
							  countCutflowEvent("LowMass: 6) mT2");
							  countSignalEvent("SR-lowMass");
						  }
					  }
				  }
			  }
		  }
	  }

	  //SR-highMass
	  //di-tau+MET trigger:
	  if (taujets[0]->PT > 75.0 && taujets[0]->PT > 40.0 && met > 150. && passTrigger){
		  countCutflowEvent("HighMass: 1) Trigger");
		  if (nTightTaus >= 1){
			  countCutflowEvent("HighMass: 2) 1 tight tau");
			  if (deltaPhi > 0.8){
				  countCutflowEvent("HighMass: 3) Deltaphi");
				  if (deltaR < 3.2){
					  countCutflowEvent("HighMass: 4) DeltaR");
					  if (mT2tautau > 70.){
						  countCutflowEvent("HighMass: 5) mT2");
						  countSignalEvent("SR-highMass");
					  }
				  }
			  }
		  }
	  }

	  return;
}

void Atlas_susy_2018_04::finalize() {
  // Whatever should be done after the run goes here
}



// ! Tag jets using flat efficiencies. Return a list of ttrue/false for each
// tau jet category. The efficiencies for 1-prong (single) and 3-prong (multi)
// tau jets can be passed as arguments.
std::vector<bool> Atlas_susy_2018_04::getTauTags(Jet* cand,
								std::vector<Track*> tracks,
	   							std::vector<GenParticle*> true_tau){

    const double DR_TAU_TRACK = 0.2;
    const double PTMIN_TAU_TRACK = 1.0;

    const double DR_TAU_TRUTH = 0.2;
    const double ETAMAX_TAU_TRUTH = 2.5;
    const double PTMIN_TAU_TRUTH = 10.0;

    double prob = rand()/(RAND_MAX+1.);
    int prongs = 0;

    std::vector<bool> tauTags;
    // These are the standard values for all candidates
    tauTags.push_back(false); //loose
    tauTags.push_back(false); // medium
    tauTags.push_back(false); //tight

    /* First, find the prong and the charge of the potential tau by
    * looping over all tracks*/
   cand->Charge = 0;
   for(int t = 0; t < tracks.size(); t++) {
       if(tracks[t]->PT < PTMIN_TAU_TRACK)
           continue;
       if(cand->P4().DeltaR(tracks[t]->P4()) < DR_TAU_TRACK) {
           prongs += 1;
           cand->Charge  += tracks[t]->Charge;
       }
   }
   // If there are 0 or more than 3 prongs, all tags are 'false'
   if(prongs == 0 || prongs > 3) {
       return tauTags;
   }
   // If it's not, let's try to find an overlapping tau
   double pT = cand->PT;
   bool tagged = false;
   for(int t = 0; t < true_tau.size(); t++) {
       double tauPT = true_tau[t]->PT;
       if(tauPT > PTMIN_TAU_TRUTH &&
          fabs(true_tau[t]->Eta) < ETAMAX_TAU_TRUTH  &&
          cand->P4().DeltaR(true_tau[t]->P4()) < DR_TAU_TRUTH) {
           tagged = true;
           if(prongs > 1) {
               if(prob < tauSigEffMultiLoose(pT)) tauTags[0] = true;
               if(prob < tauSigEffMultiMedium(pT)) tauTags[1] = true;
               if(prob < tauSigEffMultiTight(pT)) tauTags[2] = true;
           }
           else {
               if(prob < tauSigEffSingleLoose(pT)) tauTags[0] = true;
               if(prob < tauSigEffSingleMedium(pT)) tauTags[1] = true;
               if(prob < tauSigEffSingleTight(pT)) tauTags[2] = true;
           }
           break;
       }
   }

   //If there was no overlap between candidate and tau lepton
   //apply flat mis-tagging efficiencies from ATLAS ATL-PHYS-PUB-2015-045.pdf (Fig. 9):
   double mistagSingleLoose = 1./29., mistagMultiLoose =  1./29.8;
   double mistagSingleMedium = 1./49.6, mistagMultiMedium =  1./107.9;
   double mistagSingleTight = 1./115.2, mistagMultiTight =  1./314.8;
   if (!tagged){
       if(prongs > 1) {
           if(prob < mistagMultiLoose) tauTags[0] = true;
           if(prob < mistagMultiMedium) tauTags[1] = true;
           if(prob < mistagMultiTight) tauTags[2] = true;
       }
       else {
           if(prob < mistagSingleLoose) tauTags[0] = true;
           if(prob < mistagSingleMedium) tauTags[1] = true;
           if(prob < mistagSingleTight) tauTags[2] = true;
       }
   }

   return tauTags;
}

//Get efficiency for a given pT for a binned data set.
double Atlas_susy_2018_04::getEffFromData(vector<vector<double>> effData,
                                                double pt){

    int npts = effData.size();

    double pTbinCenter, pTbinLow, pTbinHigh;
    for (int i = 0; i < npts; ++i){
         //Handle lower edge (do not extrapolate)
         if (i == 0){
             if (pt < effData[i][0]) return 0.0;
             if (pt <= effData[i+1][0]) return effData[i][1];
             continue;
         }
         //Handle higher edge (extrapolate)
         if (i == npts-1){
             if (pt >= effData[i-1][0]) return effData[i][1];
             continue;
         }

          pTbinCenter = effData[i][1];
          pTbinLow = (effData[i][0]+effData[i-1][0])/2.0;
          pTbinHigh = (effData[i][0]+effData[i+1][0])/2.0;
          if (pt > pTbinLow && pt <= pTbinHigh){
              return effData[i][1];
          }
     }
     return 0.0;
}

//1-prong loose tau tagging efficiency from ATL-PHYS-PUB-2015-045.pdf (Fig. 10a)
double Atlas_susy_2018_04::tauSigEffSingleLoose(double pt) {

    vector<vector<double>> effData = {{20.,0.602},{29.455,0.602},{49.704,0.61049},
                            {69.676,0.62375},{99.634,0.63229},
                            {139.856,0.63731},{189.51,0.63163},
                            {249.427,0.62243},{329.316,0.60857},
                            {439.442,0.58892},{500.,0.58892}};

    // return 0.6; //Flat eff
    return Atlas_susy_2018_04::getEffFromData(effData,pt);
}
//1-prong medium tau tagging efficiency from ATL-PHYS-PUB-2015-045.pdf (Fig. 10a)
double Atlas_susy_2018_04::tauSigEffSingleMedium(double pt) {

    vector<vector<double>> effData = {{20.0,0.54227},{29.457,0.54227},{49.706,0.54717},
                            {69.678,0.56282},{99.636,0.57494},
                            {139.581,0.58116},{189.511,0.58026},
                            {249.428,0.56628},{329.873,0.54167},
                            {439.444,0.51724},{500.0,0.51724}};

    // return 0.55; //Flat eff
    return Atlas_susy_2018_04::getEffFromData(effData,pt);
}
//1-prong tight tau tagging efficiency from ATL-PHYS-PUB-2015-045.pdf (Fig. 10a)
double Atlas_susy_2018_04::tauSigEffSingleTight(double pt) {
    vector<vector<double>> effData = {{20,0,0.43475},{29.737,0.43475},{49.71,0.43606},
                            {69.404,0.44454},{99.362,0.45069},
                            {139.584,0.45691},{189.238,0.45123},
                            {249.432,0.43367},{329.6,0.40667},
                            {439.448,0.37746},{500.0,0.37746}};

    // return 0.45; //Flat eff
    return Atlas_susy_2018_04::getEffFromData(effData,pt);
}
//3-prong loose tau tagging efficiency from ATL-PHYS-PUB-2015-045.pdf (Fig. 10b)
double Atlas_susy_2018_04::tauSigEffMultiLoose(double pt) {

    vector<vector<double>> effData = {{20.,0.50178},{29.411,0.50178},{49.671,0.50784},
                            {69.654,0.5115},{99.635,0.54268},
                            {139.609,0.57987},{189.839,0.57767},
                            {249.774,0.55041},{329.964,0.5077},
                            {439.564,0.44119},{500.,0.44119}};

    // return 0.5; //Flat eff
    return Atlas_susy_2018_04::getEffFromData(effData,pt);
}
//3-prong medium tau tagging efficiency from ATL-PHYS-PUB-2015-045.pdf (Fig. 10b)
double Atlas_susy_2018_04::tauSigEffMultiMedium(double pt) {

    vector<vector<double>> effData = {{20.0,0.4038},{29.659,0.4038},{49.919,0.40746},
                            {69.62,0.40037},{99.322,0.42318},
                            {140.129,0.46277},{189.804,0.46176},
                            {249.462,0.4357},{329.652,0.39298},
                            {439.531,0.33126},{500.0,0.33126}};

    // return 0.4; //Flat eff
    return Atlas_susy_2018_04::getEffFromData(effData,pt);
}
//3-prong tight tau tagging efficiency from ATL-PHYS-PUB-2015-045.pdf (Fig. 10b)
double Atlas_susy_2018_04::tauSigEffMultiTight(double pt) {
    vector<vector<double>> effData = {{20,0,0.30582},{29.352,0.30582},{49.609,0.29992},
                            {69.309,0.28566},{99.289,0.31325},
                            {139.816,0.34447},{189.489,0.33749},
                            {249.426,0.31262},{329.338,0.26991},
                            {439.495,0.21177},{500.0,0.21177}};

    // return 0.3; //Flat eff
    return Atlas_susy_2018_04::getEffFromData(effData,pt);
}
