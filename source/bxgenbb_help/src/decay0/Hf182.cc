
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/Hf182.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/decay0/alpha.h>
#include <genbb_help/decay0/gamma.h>
#include <genbb_help/decay0/electron.h>
#include <genbb_help/decay0/positron.h>
#include <genbb_help/decay0/particle.h>
#include <genbb_help/decay0/pair.h>
#include <genbb_help/decay0/nucltransK.h>
#include <genbb_help/decay0/nucltransKL.h>
#include <genbb_help/decay0/nucltransKLM.h>
#include <genbb_help/decay0/nucltransKLM_Pb.h>
#include <genbb_help/decay0/beta.h>
#include <genbb_help/decay0/beta1.h>
#include <genbb_help/decay0/beta2.h>
#include <genbb_help/decay0/beta_1fu.h>
#include <genbb_help/decay0/PbAtShell.h>

namespace genbb {
  namespace decay0 {

    // Hf182.f 
    // This file was extracted from the 'decay0' program by V.I. Tretyak
    // Copyright 1995-2011 V.I. Tretyak
    // This program is free software
    // it under the terms of the GNU General Public License as published by
    // the Free Software Foundation
    // your option) any later version.
    // 
    // This program is distributed in the hope that it will be useful, but
    // WITHOUT ANY WARRANTY
    // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    // General Public License for more details.
    // 
    // You should have received a copy of the GNU General Public License
    // along with this program
    // Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
    // 

    void Hf182(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double p;
      double tclev;
      double thlev;
      double thnuc;
      // Scheme of 182Hf decay ("Table of Isotopes", 7th ed., 1978).
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 5.03.1996.
      thnuc=2.84011e14;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      tclev=0.;
      goto label_270;
    label_270  :
      decay0_beta(prng_, event_, 0.160,73.,0.,0.,t);
      thlev=1.2e-9;
      p=100.*prng_();
      if (p <= 91.77) goto label_27001;
      if (p <= 92.00) goto label_27002;
      goto label_27003;
    label_27001  :
      decay0_nucltransK(prng_, event_, 0.270,0.067,3.1e-1,0.,tclev,thlev,tdlev);
      return;
    label_27002  :
      decay0_nucltransK(prng_, event_, 0.173,0.067,9.5e-2,0.,tclev,thlev,tdlev);
      goto label_98000;
    label_27003  :
      decay0_nucltransK(prng_, event_, 0.156,0.067,1.5e-1,0.,tclev,thlev,tdlev);
      goto label_11400;
    label_11400  :
      thlev=0.;
      decay0_nucltransK(prng_, event_, 0.114,0.067,4.5e-0,0.,tclev,thlev,tdlev);
      return;
    label_98000  :
      thlev=0.;
      decay0_nucltransK(prng_, event_, 0.098,0.067,5.0e-0,0.,tclev,thlev,tdlev);
      return;
    }
    // end of Hf182.f




  } // end of namespace decay0 
} // end of namespace genbb 

// end of Hf182.cc
// Local Variables: --
// mode: c++ --
// End: --
