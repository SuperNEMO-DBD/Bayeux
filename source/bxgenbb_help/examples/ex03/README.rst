===========================
geomtools ``examples/ex03``
===========================

Introduction
============

 * Description:

   This  example illustrates  how to configure and use the event
   generator manager.
   Also it is shown how to use the ``bxgenbb_inspector`` utility.

 * Source file(s) :

   * ``ex03.cxx`` : the main program.
   * ``ex03_bank_reader.cxx`` : the program to read primary event data
     from a file (Boost/Serialization-based dpp I/O format).

 * Configuration files :

   * ``config/manager.conf`` : the main configuration file of the primary particle generator manager.
   * ``config/generators/generators.def`` : the definitions of some event generators

     * ``config/generators/Co60/*.def`` : description of the nuclear and levels and associated decays for Co-60
     * ``config/generators/Ni60/*.def`` : description of the nuclear and levels and associated decays for Ni-60


 * Built object(s) :

     * ``ex03`` : the example executable linked to the ``Bayeux`` library.
     * ``ex03_bank_reader`` : the reader executable linked to the ``Bayeux`` library.

 * Build method: CMake.

Quick start
===========

1. Build, install and setup the Bayeux library
2. Make a copy of the example directory:

.. code:: sh

      shell> cp -a $(bxquery --exampledir)/genbb_help/ex03 /tmp/genbb_help_ex03
      shell> cd /tmp/genbb_help_ex03
..

3. Build and install the example:

.. code:: sh

      shell> mkdir _build.d
      shell> cd _build.d
      shell> cmake \
	  -DCMAKE_INSTALL_PREFIX=./_install.d \
          -DBayeux_DIR:PATH=$(bxquery --cmakedir) \
          ..
      shell> make
      shell> make install
      shell> cd ..
..

4. Run the example:

.. code:: sh

      shell> ./ex03
..

5. Run the ``bxgenbb_inspector`` :

     Print the list of available generators: :

.. code:: sh

      shell> bxgenbb_inspector \
               --configuration config/manager.conf \
               --action list
..

     Generate 10000 Co60 decay events, build and save histograms
     in a ROOT file:

.. code:: sh

      shell> bxgenbb_inspector \
               --configuration config/manager.conf \
               --action shoot \
               --generator "Co60" \
               --prng-seed 314159 \
               --number-of-events 1000000 \
               --modulo 1000 \
               --prompt \
               --delayed \
               --prompt-time-limit 1 \
               --histo-def "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_prompt.conf" \
               --histo-def "@genbb_help:inspector/config/le_nuphy-1.0/inspector_histos_delayed.conf" \
               --output-file "histos_Co60.root"
..

     Generate 100 Co60 decay events, save them in a data file using
     the dpp I/O format:

.. code:: sh

      shell> bxgenbb_inspector \
               --configuration config/manager.conf \
               --action shoot \
               --generator "Co60" \
               --prng-seed 314159 \
               --number-of-events 100 \
               --modulo 1000 \
               --output-mode "bank" \
               --output-bank-label "PE" \
               --output-file "Co60_banks.data.gz"
..

     Read the primary event from the generated file:

.. code:: sh

      shell> ./_install.d/ex03_bank_reader Co60_banks.data.gz
..

6. Check the output file:

   Output histograms browsable from ROOT via the ``histos_Co60.root`` file :

.. code:: sh

      shell> root histos_Co60.root
      root [1] TBrowser b;
      root [2] .q
..


7. Clean:

.. code:: sh

      shell> rm -fr ./_build.d
      shell> rm -fr ./_install.d
..

8. Note:

   Run an automated test script:

.. code:: sh

      shell> ./process.sh
..
