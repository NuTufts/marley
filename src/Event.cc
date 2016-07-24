#include <fstream>
#include <iostream>
#include <iomanip>

#include "marley/Error.hh"
#include "marley/Event.hh"

// Local constants used only within this file
namespace {

  // Indices of the projectile and target in the vector of initial particles
  constexpr size_t PROJECTILE_INDEX = 0;
  constexpr size_t TARGET_INDEX = 1;

  // Indices of the ejectile and residue in the vector of final particles
  constexpr size_t EJECTILE_INDEX = 0;
  constexpr size_t RESIDUE_INDEX = 1;
}

// Creates an empty 2->2 scattering event with dummy initial and final
// particles and residue (particle d) excitation energy Ex.
marley::Event::Event(double Ex)
  : initial_particles_{new marley::Particle(), new marley::Particle()},
  final_particles_{new marley::Particle(), new marley::Particle()},
  Ex_(Ex) {}

// Creates an 2->2 scattering event with given initial (a & b) and final
// (c & d) particles. The residue (particle d) has excitation energy Ex
// immediately following the 2->2 reaction.
marley::Event::Event(const marley::Particle& a, const marley::Particle& b,
  const marley::Particle& c, const marley::Particle& d, double Ex)
  : initial_particles_{new marley::Particle(a), new marley::Particle(b)},
  final_particles_{new marley::Particle(c), new marley::Particle(d)},
  Ex_(Ex) {}

// Destructor
marley::Event::~Event() {
  for (auto& p : initial_particles_) delete p;
  for (auto& p : final_particles_) delete p;
  initial_particles_.clear();
  final_particles_.clear();
}

// Copy constructor
marley::Event::Event(const marley::Event& other_event)
  : initial_particles_(other_event.initial_particles_.size()),
  final_particles_(other_event.final_particles_.size()),
  Ex_(other_event.Ex_)
{
  for (size_t i = 0; i < other_event.initial_particles_.size(); ++i) {
    initial_particles_[i] = new marley::Particle(
      *other_event.initial_particles_[i]);
  }
  for (size_t i = 0; i < other_event.final_particles_.size(); ++i) {
    final_particles_[i] = new marley::Particle(
      *other_event.final_particles_[i]);
  }
}


// Move constructor
marley::Event::Event(marley::Event&& other_event)
  : initial_particles_(other_event.initial_particles_.size()),
  final_particles_(other_event.final_particles_.size()),
  Ex_(other_event.Ex_)
{
  other_event.Ex_ = 0.;
  for (size_t i = 0; i < other_event.initial_particles_.size(); ++i) {
    initial_particles_[i] = other_event.initial_particles_[i];
  }
  for (size_t i = 0; i < other_event.final_particles_.size(); ++i) {
    final_particles_[i] = other_event.final_particles_[i];
  }
  other_event.initial_particles_.clear();
  other_event.final_particles_.clear();
}

// Copy assignment operator
marley::Event& marley::Event::operator=(const marley::Event& other_event) {
  Ex_ = other_event.Ex_;

  // Delete the old particle objects owned by this event
  for (auto& p : initial_particles_) delete p;
  for (auto& p : final_particles_) delete p;

  initial_particles_.resize(other_event.initial_particles_.size());
  final_particles_.resize(other_event.final_particles_.size());

  for (size_t i = 0; i < other_event.initial_particles_.size(); ++i) {
    initial_particles_[i] = new marley::Particle(
      *other_event.initial_particles_[i]);
  }
  for (size_t i = 0; i < other_event.final_particles_.size(); ++i) {
    final_particles_[i] = new marley::Particle(
      *other_event.final_particles_[i]);
  }

  return *this;
}

// Move assignment operator
marley::Event& marley::Event::operator=(marley::Event&& other_event) {

  Ex_ = other_event.Ex_;
  other_event.Ex_ = 0.;

  // Delete the old particle objects owned by this event
  for (auto& p : initial_particles_) delete p;
  for (auto& p : final_particles_) delete p;

  initial_particles_.resize(other_event.initial_particles_.size());
  final_particles_.resize(other_event.final_particles_.size());

  for (size_t i = 0; i < other_event.initial_particles_.size(); ++i) {
    initial_particles_[i] = other_event.initial_particles_[i];
  }
  for (size_t i = 0; i < other_event.final_particles_.size(); ++i) {
    final_particles_[i] = other_event.final_particles_[i];
  }

  other_event.initial_particles_.clear();
  other_event.final_particles_.clear();

  return *this;
}

marley::Particle& marley::Event::projectile() {
  return *initial_particles_.at(PROJECTILE_INDEX);
}

marley::Particle& marley::Event::target() {
  return *initial_particles_.at(TARGET_INDEX);
}

marley::Particle& marley::Event::ejectile() {
  return *final_particles_.at(EJECTILE_INDEX);
}

marley::Particle& marley::Event::residue() {
  return *final_particles_.at(RESIDUE_INDEX);
}

const marley::Particle& marley::Event::projectile() const {
  return *initial_particles_.at(PROJECTILE_INDEX);
}

const marley::Particle& marley::Event::target() const {
  return *initial_particles_.at(TARGET_INDEX);
}

const marley::Particle& marley::Event::ejectile() const {
  return *final_particles_.at(EJECTILE_INDEX);
}

const marley::Particle& marley::Event::residue() const {
  return *final_particles_.at(RESIDUE_INDEX);
}

void marley::Event::add_initial_particle(const marley::Particle& p)
{
  initial_particles_.push_back(new marley::Particle(p));
}

void marley::Event::add_final_particle(const marley::Particle& p)
{
  final_particles_.push_back(new marley::Particle(p));
}

void marley::Event::print(std::ostream& out) const {
  if (initial_particles_.empty()) return;
  out << *initial_particles_.at(PROJECTILE_INDEX) << '\n';
  for (const auto p : final_particles_) out << *p << '\n';
}

// Function that dumps a marley::Particle to an output stream in HEPEvt format.
// This is a private helper function for the publicly-accessible write_hepevt.
void marley::Event::dump_hepevt_particle(const marley::Particle& p,
  std::ostream& os, bool track)
{
  if (track) os << "1 ";
  else os << "0 ";

  // TODO: improve this entry to give the user more control over the vertex
  // location and to reflect the parent-daughter relationships between
  // particles.
  // Factors of 1000. are used to convert MeV to GeV for the HEPEvt format
  os << p.pdg_code() << " 0 0 0 0 " << p.px() / 1000.
    << ' ' << p.py() / 1000. << ' ' << p.pz() / 1000.
    << ' ' << p.total_energy() / 1000. << ' ' << p.mass() / 1000.
    // Spacetime origin is currently used as the initial position 4-vector for
    // all particles
    << " 0. 0. 0. 0." << '\n';
}

void marley::Event::write_hepevt(size_t event_num, std::ostream& out) {
  out << std::setprecision(16) << std::scientific;
  out << event_num  << ' ' << final_particles_.size() + 1 << '\n';
  dump_hepevt_particle(*initial_particles_.at(PROJECTILE_INDEX), out, false);
  for (const auto fp : final_particles_) dump_hepevt_particle(*fp, out, true);
}
