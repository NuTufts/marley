#pragma once
#include <functional>
#include <regex>
#include <string>
#include <vector>
#include "TMarleyDecayScheme.hh"
#include "TMarleyLevel.hh"

class TMarleyReaction {
  public:
    TMarleyReaction(std::string filename, TMarleyDecayScheme* scheme = nullptr);
    double fermi_function(int Z, int A, double E, bool electron);
    double fermi_approx(int Z, double E, bool electron);
    double ejectile_energy(double E_level, double Ea, double cos_theta_c);
    double max_level_energy(double Ea);
    double get_threshold_energy();
    double differential_xs(double E_level, double Ea, double matrix_element, double cos_theta_c);
    double total_xs(double E_level, double Ea, double matrix_element);
    double sample_ejectile_scattering_cosine(double E_level, double Ea, double matrix_element);
    void set_decay_scheme(TMarleyDecayScheme* scheme);
    void create_event(double Ea); // TODO: change this to return an event object
    std::string get_next_line(std::ifstream &file_in,
      std::regex &rx, bool match) const;

  private:
    double ma; // projectile mass
    double mb; // target mass
    double mc; // ejectile mass
    double md_gs; // residue mass
    double GF = 1; //1.16637e-11; // Fermi coupling constant (MeV^(-2)) 
    double Vud = 0.97427; // abs(V_ud) (from CKM matrix)
    int Zi, Ai, Zf, Af; // Initial and final values of the atomic and mass numbers
    int pid_a, pid_b, pid_c, pid_d; // Particle IDs for all 4 particles
    // Lab-frame total energy of the projectile at threshold
    // for this reaction (all final-state particles at rest
    // in the CM frame)
    double Ea_threshold;
    TMarleyDecayScheme* ds;
    std::vector<double> residue_level_energies; // Energy values from reaction dataset
    std::vector<double> residue_level_strengths; // B(F) + B(GT) values from reaction dataset
    std::vector<TMarleyLevel*> residue_level_pointers; // Pointers to the corresponding ENSDF decay scheme levels
};
