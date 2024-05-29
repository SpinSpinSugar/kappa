/*
 * \file dumpspectrumTest.cpp
 * \brief Dump vibrational spectrum
 */

#include <fstream>
#include <iostream>

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#include "kappa.hpp"

std::string GetCurrentWorkingDir(void) {
  char buff[FILENAME_MAX];
  GetCurrentDir(buff, FILENAME_MAX);
  std::string current_working_dir(buff);
  return current_working_dir;
}

using namespace std;
using namespace kappa;

int main(int argc, char **argv) {
  std::cout << "Start Test for dump_spectrum" << endl;

  // Retrieve the kappa data path from environment variable defined in ~/.bashrc
  std::string m_source = std::getenv("KAPPA_DATA_DIRECTORY");
  std::string particle_source = m_source + "particles.yaml";
  std::string interaction_source = m_source + "interaction.yaml";
  std::string output_dir = GetCurrentWorkingDir();

  std::cout << "KAPPA_DATA_DIRECTORY is: " << m_source << '\n';
  std::cout << "Current directory is: " << output_dir << std::endl;
  std::cout << "Particle directory is: " << particle_source << std::endl;
  std::cout << "Interaction directory is: " << interaction_source << std::endl;

  // create molecules
  Molecule N2("N2", true, false, particle_source);
  Molecule NO("NO", true, true, particle_source);
  Molecule O2("O2", true, true, particle_source);

  // select approximation level
  kappa::Approximation appr = kappa::Approximation{};

  int i;  // vibrational levels

  ofstream f;  // output file

  f.open(output_dir + "/N2_spectrum.csv");
  std::cout << N2.vibr_energy[0][1] << std::endl;
  for (i = 0; i < N2.num_vibr_levels[0]; i++) {
    f << N2.vibr_energy[0][i] / K_CONST_EV;
    std::cout << i << " " << N2.vibr_energy[0][i] - N2.vibr_energy[0][0] << " "
              << appr.avg_rot_energy(300, N2, N2.vibr_energy[0][i], 0)
              << std::endl;
    f << ",";
  }
  f.close();

  f.open(output_dir + "/NO_spectrum.csv");
  for (i = 0; i < NO.num_vibr_levels[0]; i++) {
    f << NO.vibr_energy[0][i] / K_CONST_EV;
    f << ",";
  }
  f.close();

  f.open(output_dir + "/O2_spectrum.csv");
  for (i = 0; i < O2.num_vibr_levels[0]; i++) {
    f << O2.vibr_energy[0][i] / K_CONST_EV;
    f << ",";
  }
  f.close();

  return 0;
}
