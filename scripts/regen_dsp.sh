#!/bin/sh
# Regenerates Source/dsp/RenkySumDSP.h from dsp/RenkySum.dsp.
# Requires the Faust compiler (brew install faust). The generated header is
# committed, so the normal build does not need Faust at all.
set -e
cd "$(dirname "$0")/.."
faust -lang cpp -cn RenkySumDSP -scn FaustDsp -o Source/dsp/RenkySumDSP.h dsp/RenkySum.dsp
echo "Wrote Source/dsp/RenkySumDSP.h"
