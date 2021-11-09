// This program tests Skywalker's C++ interface with an ensemble defined by an
// enumeration.

#include <skywalker.hpp>

#include <cassert>
#include <iostream>
#include <string.h>
#include <tgmath.h>

void usage(const std::string& prog_name) {
  std::cerr << prog_name << ": usage:" << std::endl;
  std::cerr << prog_name << " <input.yaml>" << std::endl;
  exit(0);
}

static bool approx_equal(sw_real_t x, sw_real_t y) {
  return (fabs(x - y) < 1e-14);
}

using namespace skywalker;

int main(int argc, char **argv) {

  if (argc == 1) {
    usage((const char*)argv[0]);
  }
  std::string input_file = argv[1];

  // Print a banner with Skywalker's version info.
  print_banner();

  // Load the ensemble. Any error encountered is fatal.
  std::cerr << "enumeration_test: Loading ensemble from " << input_file << std::endl;
  Ensemble* ensemble = load_ensemble(input_file, "settings");

  // Make sure everything is as it should be.

  // Ensemble type
  assert(ensemble->type() == SW_ENUMERATION);

  // Settings
  Settings settings = ensemble->settings();
  assert(settings["param1"] == "hello");
  assert(settings["param2"] == "81");
  assert(settings["param3"] == "3.14159265357");

  // Ensemble data
  assert(ensemble->size() == 11);
  for (auto& iter: ensemble) {
    // Fixed parameters
    assert(approx_equal(iter->input["p1"], 1.0));
    assert(approx_equal(iter->input["p2"], 2.0));
    assert(approx_equal(iter->input["p3"], 3.0));

    // Ensemble parameters
    assert(iter->input["tick"] >= 0.0);
    assert(iter->input["tick"] <= 10.0);

    assert(iter->input["tock"] >= 1e1);
    assert(iter->input["tock"] <= 1e11);

    // Look for a parameter that doesn't exist.
    bool caught = false;
    try {
      auto nope = iter->input["invalid_param"];
    }
    catch (Exception&) {
      caught = true;
    }
    assert(caught);

    // Add a "qoi" metric set to 4.
    iter->output.set("qoi", 4.0);
  }

  // Write out a Python module.
  ensemble->write("enumeration_test_cpp.py");

  // Clean up.
  delete ensemble;
}
