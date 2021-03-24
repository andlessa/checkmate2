#!/bin/sh

homeDIR="$( pwd )"

echo "[Checking system dependencies]"
PKG_OK=$(dpkg-query -W -f='${Status}' autoconf 2>/dev/null | grep -c "ok installed")
if test $PKG_OK = "0" ; then
  echo "autoconf not found. Install it with sudo apt-get install autoconf."
  exit
fi
PKG_OK=$(dpkg-query -W -f='${Status}' libtool 2>/dev/null | grep -c "ok installed")
if test $PKG_OK = "0" ; then
  echo "libtool not found. Install it with sudo apt-get install libtool."
  exit
fi
PKG_OK=$(dpkg-query -W -f='${Status}' gzip 2>/dev/null | grep -c "ok installed")
if test $PKG_OK = "0" ; then
  echo "gzip not found. Install it with sudo apt-get install gzip."
  exit
fi
PKG_OK=$(dpkg-query -W -f='${Status}' bzr 2>/dev/null | grep -c "ok installed")
if test $PKG_OK = "0" ; then
  echo "bzr not found. Install it with sudo apt-get install bzr."
  exit
fi



madgraph="MG5_aMC_v2.9.2.tar.gz"
URL=https://launchpad.net/mg5amcnlo/2.0/2.9.x/+download/$madgraph
echo -n "Install MadGraph (y/n)? "
read answer
if echo "$answer" | grep -iq "^y" ;then
	mkdir MG5;
	echo "[installer] getting MadGraph5"; wget $URL 2>/dev/null || curl -O $URL; tar -zxf $madgraph -C MG5 --strip-components 1;
	cd $homeDIR
	rm $madgraph;
fi

#Get HepMC tarball
hepmc="hepmc2.06.11.tgz"
echo -n "Install HepMC (y/n)? "
read answer
if echo "$answer" | grep -iq "^y" ;then
	mkdir hepMC_tmp
	URL=http://hepmc.web.cern.ch/hepmc/releases/$hepmc
	echo "[installer] getting HepMC"; wget $URL 2>/dev/null || curl -O $URL; tar -zxf $hepmc -C hepMC_tmp;
	mkdir HepMC-2.06.11; mkdir HepMC-2.06.11/build; mkdir HepMC;
	echo "Installing HepMC in ./HepMC";
	cd HepMC-2.06.11/build;
	../../hepMC_tmp/HepMC-2.06.11/configure --prefix=$homeDIR/HepMC --with-momentum=GEV --with-length=MM;
	make;
	make check;
	make install;

	#Clean up
	cd $homeDIR;
	rm -rf hepMC_tmp; rm $hepmc; rm -rf HepMC-2.06.11;
fi


#Get pythia tarball
pythia="pythia8244.tgz"
URL=http://home.thep.lu.se/~torbjorn/pythia8/$pythia
echo -n "Install Pythia (y/n)? "
read answer
if echo "$answer" | grep -iq "^y" ;then
	if hash gzip 2>/dev/null; then
		mkdir pythia8;
		echo "[installer] getting Pythia"; wget $URL 2>/dev/null || curl -O $URL; tar -zxf $pythia -C pythia8 --strip-components 1;
		echo "Installing Pythia in pythia8";
		cd pythia8;
		./configure --with-hepmc2=$homeDIR/HepMC --with-root=$ROOTSYS --prefix=$homeDIR/pythia8 --with-gzip
		make -j4; make install;
		cd $homeDIR
		rm $pythia;
	else
		echo "[installer] gzip is required. Try to install it with sudo apt-get install gzip";
	fi
fi


echo -n "Install Delphes (y/n)? "
read answer
if echo "$answer" | grep -iq "^y" ;then
	echo "[installer] getting Delphes";
  git clone --branch 3.4.2 https://github.com/delphes/delphes.git Delphes;
  cd Delphes;
  cp ../Makefile_Delphes ./Makefile;
  export PYTHIA8=$homeDIR/pythia8;
  echo "[installer] installing Delphes";
  make HAS_PYTHIA8=true;
  cd $homeDIR;
fi

echo -n "Compile CheckMATE3 with external tools (y/n)? "
read answer
if echo "$answer" | grep -iq "^y" ;then
  cd ../;
  autoreconf -i -f;
  ./configure --with-rootsys=$ROOTSYS --with-delphes=$homeDIR/Delphes --with-pythia=$homeDIR/pythia8 --with-madgraph=$homeDIR/MG5 --with-hepmc=$homeDIR/HepMC
  echo "[installer] installing CheckMATE3";
  make -j4
  echo "[installer] recompiling CheckMATE";
  make;
  cd $homeDIR
fi



