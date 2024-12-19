#include "datastructure/RailwayNetwork.hpp"
#include "datastructure/Timetable.hpp"
#include "simulation/RoutingSolution.hpp"

using namespace cda_rail;

#include "gtest/gtest.h"
#include <plog/Log.h>
#include <plog/Logger.h>
#include <vector>

TEST(Simulation, RandomSolution) {
  Network network =
      Network::import_network("./example-networks/SimpleStation/network/");
  Timetable timetable = Timetable::import_timetable(
      "./example-networks/SimpleStation/timetable/", network);
  ulong seed =
      std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::ranlux24_base rng_engine(seed);

  for (int i = 0; i <= 1000; i++) {
    const RoutingSolution sol = RoutingSolution(
        10, 10, 100, *(timetable.get_train_list().begin()), rng_engine);
    for (auto target : sol.v_targets.targets) {
      ASSERT_GE(target.first, 1);
      ASSERT_LE(target.first, 100);
      ASSERT_GE(target.second, -50);
      ASSERT_LE(target.second, 50);
    }
    ASSERT_EQ(sol.v_targets.targets.size(), 10);
    ASSERT_EQ(sol.switch_directions.size(), 10);
  }
}

TEST(Simulation, SpeedTargets) {
  std::vector<ulong>  timesteps = {3, 20, 50, 75, 87};
  std::vector<double> speeds    = {0.4, 0.6, 0.5, -0.2, -0.5};

  SpeedTargets v_targets(timesteps, speeds);

  ASSERT_EQ(v_targets.find_target_speed(2), 0.4);
  ASSERT_EQ(v_targets.find_target_speed(11), 0.4);
  ASSERT_EQ(v_targets.find_target_speed(21), 0.6);
  ASSERT_EQ(v_targets.find_target_speed(50), 0.5);
  ASSERT_EQ(v_targets.find_target_speed(74), 0.5);
  ASSERT_EQ(v_targets.find_target_speed(84), -0.2);
  ASSERT_EQ(v_targets.find_target_speed(89), -0.5);
}