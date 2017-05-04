#ifndef GENBB_DECAY0_I135_H_
#define GENBB_DECAY0_I135_H_ 1

namespace mygsl {
  class rng;
}
namespace genbb {
  class primary_event;
  namespace decay0 {

    void I135(mygsl::rng & prng_, primary_event & event_, double tcnuc, double & tdnuc);


  } // end of namespace decay0
} // end of namespace genbb

#endif // GENBB_DECAY0_I135_H_
// Local Variables: --
// mode: c++ --
// End: --
