
#include <sstream>
#include <stdexcept>
#include <cmath>

#include <mygsl/rng.h>

#include <genbb_help/decay0/K40.h>
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

    // K40.f
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

    void K40(mygsl::rng & prng_, genbb::primary_event & event_, double tcnuc, double & tdnuc)
    {
      double t;
      double tdlev;
      double pdecay;
      double tclev;
      double thlev;
      double thnuc;
      // Scheme of K40 decay (J.A.Cameron et al., ENSDF 29.10.2002).
      // Input : tcnuc - time of creation of nucleus (sec)
      // Output: tdnuc - time of decay of nucleus (sec)
      // // common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),// ptime(100).
      // VIT, 3.08.1992, 22.10.1995, 28.11.2003
      // 3rd forbidden unique shape for beta decay, VIT, 27.10.2006.
      thnuc=3.94775e+16;
      tdnuc=tcnuc-thnuc/std::log(2.)*std::log(prng_());
      tclev=0.;
      pdecay=100.*prng_();
      if (pdecay <= 89.140) {
        // decay0_beta(prng_, event_, 1.311,+20.,0.,0.,t)  // 89.140% b- 40Ca(gs)
        // change to the 3rd forbidden unique shape in accordance with
        // H.Daniel, RMP 40(1968)659 and W.H.Kelly et al., Nucl. Phys. 11(1959)492
        // cf(e) = pel**6 + c1*pel**4*pnu**2 + c2*pel**2*pnu**4 + c3*pnu**6
        decay0_beta2(prng_, event_, 1.311,+20.,0.,0.,t,3,7.,7.,1.,0.);
      } else if (pdecay <= 99.800) {  // 10.660% ec 40Ar(1461)
          decay0_gamma(prng_, event_, 0.003,0.,0.,t);
          thlev=1.12e-12;
          decay0_nucltransK(prng_, event_, 1.461,0.003,3.0e-5,7.3e-5,tclev,thlev,tdlev);
      } else if (pdecay <= 99.999) {  // 0.199% ec 40Ar(gs)
        decay0_gamma(prng_, event_, 0.003,0.,0.,t);
      } else {
        decay0_beta(prng_, event_, 0.483,-18.,0.,0.,t)  ;// 0.001% b+ 40Ar(gs)
      };
      return;
    }
    // end of K40.f

  } // end of namespace decay0
} // end of namespace genbb

// end of K40.cc
// Local Variables: --
// mode: c++ --
// End: --
