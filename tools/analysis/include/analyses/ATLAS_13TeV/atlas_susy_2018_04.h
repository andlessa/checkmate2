#ifndef ATLAS_SUSY_2018_04_H_
#define ATLAS_SUSY_2018_04_H_
// AUTHOR: Andre Lessa
//  EMAIL: andre.lessa@ufabc.edu.br
#include "AnalysisBase.h"

class Atlas_susy_2018_04 : public AnalysisBase {
  public:
    Atlas_susy_2018_04() : AnalysisBase()  {}
    ~Atlas_susy_2018_04() {}

    void initialize();
    void analyze();
    void finalize();

  private:
    vector<bool> getTauTags(Jet* cand,
                                    std::vector<Track*> tracks,
		                            std::vector<GenParticle*> true_tau);

    double getEffFromData(vector<vector<double>> effData,
                                    double pt);
    double tauSigEffSingleLoose(double pt);
    double tauSigEffSingleMedium(double pt);
    double tauSigEffSingleTight(double pt);
    double tauSigEffMultiLoose(double pt);
    double tauSigEffMultiMedium(double pt);
    double tauSigEffMultiTight(double pt);

};

#endif
