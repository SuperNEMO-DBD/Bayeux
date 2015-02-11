// test_tessellated_solid.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>

// This project:
#include <geomtools/gnuplot_draw.h>
#include <geomtools/tessellation.h>
#include <geomtools/gdml_writer.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

void test0();
void test1();
void test2();
void test3();
void test4();
void test5(bool draw_);
void test6(bool draw_);

int main (int argc_, char **argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Hello, this is a test program "
              << "for class 'tessellated_solid'!" << std::endl;

    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") {
        draw = true;
        // } else if (arg == "-f" || arg == "--filename") {
        //        fn = argv_[++iarg];
      }

      iarg++;
    }

    test0();

    test1();

    test2();

    test3();

    test4();

    test5(draw);

    test6(draw);

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void test0()
{
  geomtools::vector_3d v0 (0, 0, 0);
  geomtools::vector_3d v1 (1,0, 0);
  geomtools::vector_3d v2 (1.0, 1.0, 0);
  geomtools::vector_3d v3 (0, 1, 0);
  bool c = geomtools::facet34::check_quadrangle (v0, v1, v2, v3);
  if (!c) {
    std::clog << "Invalid quadrangle !" << std::endl;
  } else {
    std::clog << "Valid quadrangle !" << std::endl;
  }
  return;
}

void test1()
{
  geomtools::vector_3d v0 (0, 0, 0);
  geomtools::vector_3d v1 (1,0, 0);
  geomtools::vector_3d v2 (0.5, 0.5, 0);
  geomtools::vector_3d v3 (0, 1, 0);
  bool c = geomtools::facet34::check_quadrangle (v0, v1, v2, v3);
  if (!c) {
    std::clog << "Invalid quadrangle !" << std::endl;
  } else {
    std::clog << "Valid quadrangle !" << std::endl;
  }
  return;
}

void test2()
{
  geomtools::vector_3d v0 (0, 0, 0);
  geomtools::vector_3d v1 (1,0, 0);
  geomtools::vector_3d v2 (0.25, 0.25, 0);
  geomtools::vector_3d v3 (0, 1, 0);
  bool c = geomtools::facet34::check_quadrangle (v0, v1, v2, v3);
  if (!c) {
    std::clog << "Invalid quadrangle !" << std::endl;
  } else {
    std::clog << "Valid quadrangle !" << std::endl;
  }
  return;
}

void test3()
{
  geomtools::vector_3d v0 (0, 0, 0);
  geomtools::vector_3d v1 (1,0, 0);
  geomtools::vector_3d v2 (0.25, 0.25, 0);
  bool c = geomtools::facet34::check_triangle (v0, v1, v2);
  if (!c) {
    std::clog << "Invalid triangle !" << std::endl;
  } else {
    std::clog << "Valid triangle !" << std::endl;
  }
  return;
}

void test4()
{
  geomtools::vector_3d v0 (0, 0, 0);
  geomtools::vector_3d v1 (1, 0, 0);
  geomtools::vector_3d v2 (2, 0, 0);
  bool c = geomtools::facet34::check_triangle (v0, v1, v2);
  if (!c) {
    std::clog << "Invalid triangle !" << std::endl;
  } else {
    std::clog << "Valid triangle !" << std::endl;
  }
  return;
}

void test5(bool draw_)
{
  geomtools::tessellated_solid TS1;

  int ivtx = 0;
  int P = TS1.add_vertex (ivtx++, 0, 0, 0);
  int Q = TS1.add_vertex (ivtx++, 1, 0, 0);
  int R = TS1.add_vertex (ivtx++, 1, 1, 0);
  int S = TS1.add_vertex (ivtx++, 0, 1, 0);
  int T = TS1.add_vertex (ivtx++, 0.5, 0.5, 1);

  int ifct = 0;
  TS1.add_facet4 (ifct++, P, S, R, Q);
  TS1.add_facet3 (ifct++, P, Q, T);
  TS1.add_facet3 (ifct++, Q, R, T);
  TS1.add_facet3 (ifct++, R, S, T);
  TS1.add_facet3 (ifct++, S, P, T);
  TS1.lock ();
  TS1.dump (std::clog);

  geomtools::tessellated_solid TS2;
  double z0 = 2;
  int A1 = TS2.add_vertex (ivtx++, 0, 0, z0);
  int B1 = TS2.add_vertex (ivtx++, 2, 0, z0);
  int C1 = TS2.add_vertex (ivtx++, 2, 1, z0);
  int D1 = TS2.add_vertex (ivtx++, 0, 1, z0);
  int A2 = TS2.add_vertex (ivtx++, 0, 0, z0 + 0.5);
  int B2 = TS2.add_vertex (ivtx++, 2, 0, z0 + 0.5);
  int C2 = TS2.add_vertex (ivtx++, 2, 1, z0 + 0.5);
  int D2 = TS2.add_vertex (ivtx++, 0, 1, z0 + 0.5);

  TS2.add_facet4 (ifct++, A2, D2, C2, B2);
  TS2.add_facet4 (ifct++, A1, D1, C1, B1);
  TS2.add_facet4 (ifct++, A1, B1, B2, A2);
  TS2.add_facet4 (ifct++, B1, C1, C2, B2);
  TS2.add_facet4 (ifct++, C1, D1, D2, C2);
  TS2.add_facet4 (ifct++, D1, A1, A2, D2);

  TS2.lock ();
  TS2.dump (std::clog);
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  if (draw_) {
    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_tessellated_solid");

    geomtools::vector_3d tessel_pos;
    geomtools::rotation_3d tessel_rot;
    geomtools::create_rotation(tessel_rot, 0.0, 0.0, 0.0);
    geomtools::gnuplot_draw::draw_tessellated(tmp_file.out(), tessel_pos, tessel_rot, TS1);
    tmp_file.out() << std::endl;
    geomtools::gnuplot_draw::draw_tessellated(tmp_file.out(), tessel_pos, tessel_rot, TS2);
    tmp_file.out() << std::endl << std::endl;
    tmp_file.close();
    usleep(200);

    {
      Gnuplot g1 ("lines");
      {
        std::ostringstream title;
        title << "set title '";
        title << "Tessellated solid from CATIA/STL export";
        // title << " (";
        // title << TS2.vertices ().size() << " vertices, " << TS2.facets().size() << " facets)";
        g1.cmd(title.str());
      }
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xyplane at -5");
      g1.set_xrange(-5, +5).set_yrange(-5, +5).set_zrange(-5, +5);
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      g1.plotfile_xyz(tmp_file.get_filename (), 1, 2, 3, "3D view");
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }

    geomtools::gdml_writer writer;
    // Defines:
    writer.add_position ("null.pos",
                         0. * CLHEP::mm,
                         0. * CLHEP::mm,
                         0. * CLHEP::mm,
                         "mm");
    // Materials:
    writer.add_element("Hydrogen", 1, "H",  1);
    writer.add_element("Carbon",   6, "C", 12);
    writer.add_element("Nitrogen", 7, "N", 14);
    writer.add_element("Oxygen",   8, "O", 16);

    std::map<std::string, double> Air_map;
    Air_map["Oxygen"]   = 0.3;
    Air_map["Nitrogen"] = 0.7;
    writer.add_material("Air",
                        "air",
                        1.29 * CLHEP::mg / CLHEP::cm3,
                        Air_map);
    writer.add_material("Aluminium",
                        13.0,
                        2.70 * CLHEP::g / CLHEP::cm3,
                        26.98);

    // Solids:
    writer.add_gdml_box("lab.solid",
                        15.0 * CLHEP::mm, 15.0 * CLHEP::mm, 15.0 * CLHEP::mm,
                        "mm");
    // writer.add_tessellated("tessella.solid", TS1, "mm");
    writer.add_gdml_box("tessella.solid",
                        5.0 * CLHEP::mm, 5.0 * CLHEP::mm, 5.0 * CLHEP::mm,
                        "mm");

    // Structures:
    writer.add_volume("dummy.log",
                      "Aluminium",
                      "tessella.solid");

    std::list<geomtools::gdml_writer::physvol> world_physvols;
    world_physvols.push_back(geomtools::gdml_writer::physvol("dummy.log", "null.pos"));
    writer.add_volume("world.log",
                      "Air",
                      "lab.solid",
                      world_physvols);

    // Setup:
    writer.add_setup("Setup", "world.log");

    writer.dump(std::clog);

    std::string gdml_file = "test_tessellated_solid_1.gdml";
    writer.save_file(gdml_file,
                     geomtools::gdml_writer::default_xml_version(),
                     geomtools::gdml_writer::default_xml_encoding(),
                     geomtools::gdml_writer::default_gdml_schema(),
                     geomtools::gdml_writer::default_xsi()
                     );



  }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
  return;
}

void test6(bool draw_)
{
  geomtools::tessellated_solid TS;

  datatools::properties TS_config;
  TS_config.store_string("length_unit", "mm");
  TS_config.store_path("stl_file", "${GEOMTOOLS_TESTING_DIR}/data/test_stl_import.stl");
  TS.initialize(TS_config);

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  if (draw_) {
    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_tessellated_solid");

    geomtools::vector_3d tessel_pos;
    geomtools::rotation_3d tessel_rot;
    geomtools::create_rotation(tessel_rot, 0.0, 0.0, 0.0);
    geomtools::gnuplot_draw::draw_tessellated(tmp_file.out(), tessel_pos, tessel_rot, TS);
    tmp_file.out() << std::endl;
    tmp_file.close();
    usleep(200);

    {
      Gnuplot g1 ("lines");
      {
        std::ostringstream title;
        title << "set title '";
        title << "Tessellated solid from CATIA/STL export";
        g1.cmd(title.str());
      }
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xyplane at -30");
      g1.set_xrange(-30, +30).set_yrange(-30, +30).set_zrange(-30, +30);
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      g1.plotfile_xyz(tmp_file.get_filename (), 1, 2, 3, "3D view");
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }

  }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
  return;
}
