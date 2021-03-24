#!/usr/bin/env python

"""
Simple code for computing the combined upper limit for a given signal over several signal regions (bins).

It assumes the simplified likelihood framework, using
the covariant matrix for all signal regions.
"""

from simplifiedLikelihoods import Data, UpperLimitComputer
import sys
import numpy as np


def getCombinedR(data,cov,orderSRs,deltas_rel=0.):
    """
    Compute a combined r-value using the covariance matrix.

    :param data: 2-d numpy array with number of observed events (obs),
                 number of signal events (s),
                 number of expected background events (bkg) for each
                 signal region (sr)
    :param cov: covariace matrix (CovarianceHandler object)
    :param orderSRs: lists of SR strings in the same order used
                     for the covariance matrix
    :param deltas_rel: Relative uncertainty for the number of signal events.

    :return: Combined r-value (sum(nsig)/ul_combined)
    """
    nobs = [0]*len(orderSRs)
    nsig = [0]*len(orderSRs)
    bg = [0]*len(orderSRs)
    # Get ordered data for the specified SRs
    for pt in data:
        if not pt['sr'] in orderSRs:
            continue
        i = orderSRs.index(pt['sr'])
        nobs[i] = pt['obs']
        nsig[i] = pt['s']
        bg[i] = pt['bkg']

    #Compute the combined upper limit
    ul = getCombinedUpperLimitFor(nobs, bg, nsig,
                    cov.covariance, deltas_rel)

    #Compute the new r-value:
    r = sum(nsig)/ul

    return r


def getCombinedUpperLimitFor(nobs, bg, nsig, cov, deltas_rel=0.2):
    """
    Get combined upper limit.

    :param nobs: list of observed events in each signal region. The list
                    should obey the ordering in the covariance matrix.
    :param bg: list of expected BG events in each signal region. The list
                    should obey the ordering in the covariance matrix.
    :param nsig: list of signal events in each signal region. The list
                    should be consistent with the covariance matrix.
    :param deltas_rel: relative uncertainty in signal (float). Default value is 20%.
    :returns: upper limit on the number of signal events summed over all signal regions
    """
    computer = UpperLimitComputer(ntoys=10000)

    ret = computer.ulSigma(Data(observed=nobs, backgrounds=bg, covariance=cov,
                                 third_moment=None, nsignal=nsig, deltas_rel=deltas_rel),
                           marginalize=False, expected=False)

    return ret



class CovarianceHandler:
    """Basic class to handle convariance matrices."""

    def __init__ ( self, filename, histoname, max_datasets=None,
                   aggregate = None , triangular=False):
        """Load the covariance matrix from ROOT file."""
        import ROOT
        f=ROOT.TFile(filename)
        h=self.getHistogram(f, histoname)
        xaxis = h.GetXaxis()
        self.n=h.GetNbinsX()+1
        if max_datasets:
            self.n=min(max_datasets+1,self.n)
        self.datasetOrder = []
        self.covariance = []
        self.blinded_regions = []
        for i in range ( 1, self.n ):
            if i in self.blinded_regions:
                continue
            self.datasetOrder.append(xaxis.GetBinLabel(i))
            row = []
            for j in range ( 1, self.n ):
                if j in self.blinded_regions:
                    continue
                el = h.GetBinContent(i,j)
                if i==j and el < 1e-4:
                   print( "variance in the covariance matrix at position %d has a very small (%g) value" % (i,el) )
                   el = 1e-4
                row.append(el)
            self.covariance.append(row)

        if aggregate != None:
            ## aggregate the stuff
            self.aggregateThis(aggregate)

        if triangular:
            c = np.array(self.covariance)
            if (np.triu(c) == c).all() or (np.tril(c) == c).all():
                self.covariance = c + c.transpose() - np.diag(np.diag(c))
                self.covariance = self.covariance.tolist()
        self.checkCovarianceMatrix()

    def computeAggCov(self, agg1, agg2 ):
        """Compute the covariance between agg1 and agg2."""
        C=0.
        for i in agg1:
            for j in agg2:
                C+=self.covariance[i-1][j-1]
        return C

    def checkCovarianceMatrix( self ):
        """Quick check if the covariance matrix is invertible."""
        from simplifiedLikelihoods import Data
        n=len(self.covariance)
        m=Data( [0.]*n, [0.]*n, self.covariance )
        # print( "Check %d-dim covariance matrix for positive definiteness." % n)
        try:
            I=(m.covariance)**(-1)
        except Exception as e:
            print( "Inversion failed. %s" % e)
            sys.exit()
        try:
            from scipy import stats
            l=stats.multivariate_normal.logpdf([0.]*n,mean=[0.]*n,cov=m.covariance)
        except Exception as e:
            import numpy
            print( "computation of logpdf failed: %s" % e )
            print( "the diagonal reads: %s " % ( numpy.diag ( m.covariance ) ) )
            sys.exit()

    def getHistogram( self, f, histoname ):
        """Retrieve histogram.

        :param f: filehandle
        """
        h=f.Get ( histoname )
        if h: return h
        if not "/" in histoname:
            print( "cannot find %s in %s" % (histoname, f.GetName()))
            sys.exit()
        tokens = histoname.split("/")
        if not len(tokens)==2:
            print("cannot interpret histoname %s in %s" % \
                            ( histoname, f.name ) )
            sys.exit()
        c= f.Get ( tokens[0] )
        if not c:
            print( "cannot retrieve %s from %s" % \
                            ( histoname, f.name ) )
            sys.exit()
        if c.ClassName() == "TCanvas":
            h=c.GetPrimitive ( tokens[1] )
            if h: return h
            print( "cannot retrieve %s from %s" % \
                            ( histoname, f.name ) )
            sys.exit()
        print( "cannot interpret %s in %s" % \
                        ( histoname, f.name ) )
        sys.exit()
