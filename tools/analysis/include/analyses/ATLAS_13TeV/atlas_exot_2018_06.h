#ifndef ATLAS_EXOT_2018_06_H_
#define ATLAS_EXOT_2018_06_H_
// AUTHOR: Andre Lessa
//  EMAIL: lessa.a.p@gmail.com
#include "AnalysisBase.h"

class Atlas_exot_2018_06 : public AnalysisBase {
  public:
    Atlas_exot_2018_06() : AnalysisBase()  {}
    ~Atlas_exot_2018_06() {}

    void initialize();
    void analyze();
    void finalize();

  private:

    float dPhi(std::vector<Jet*> jets, ETMiss* missingET);
    int countMatchingTracks(std::vector<Track*> tracks, Jet* jet);
    void overlapRemoval_jetMuon();
    void overlapRemoval_jetElectron();
    std::vector<Jet*> getTauJets();
    static std::string signal_regions [26];
    static int signal_region_borders [26][2];

};

#endif
