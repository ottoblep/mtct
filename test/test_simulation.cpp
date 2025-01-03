#include "datastructure/RailwayNetwork.hpp"
#include "datastructure/Timetable.hpp"
#include "simulation/TrainTrajectorySet.hpp"

using namespace cda_rail;

#include "gtest/gtest.h"
#include <plog/Log.h>
#include <plog/Logger.h>
#include <vector>

TEST(Simulation, SimulationInstance) {
  Network network =
      Network::import_network("./example-networks/SimpleStation/network/");
  Timetable timetable = Timetable::import_timetable(
      "./example-networks/SimpleStation/timetable/", network);

  sim::SimulationInstance instance(network, timetable, 20);

  ASSERT_EQ(instance.get_max_train_speed(), 83.33);
  ASSERT_EQ(instance.get_shortest_track(), 5);
}

TEST(Simulation, EdgeTrajectory) {
  const ulong seed =
      std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::ranlux24_base rng_engine(seed);
  Network            network =
      Network::import_network("./example-networks/SimpleNetwork/network/");
  Timetable timetable = Timetable::import_timetable(
      "./example-networks/SimpleNetwork/timetable/", network);

  sim::SimulationInstance              instance(network, timetable, 20);
  std::uniform_int_distribution<ulong> random_train_index(
      0, timetable.get_train_list().size() - 1);

  for (int i = 0; i < 1000; i++) {
    Train train =
        timetable.get_train_list().get_train(random_train_index(rng_engine));
    sim::RoutingSolution solution(10, 10, instance.n_timesteps, train,
                                  rng_engine);

    Schedule train_schedule = instance.timetable.get_schedule(train.name);
    sim::TrainState init_state{
        .timestep = (ulong)train_schedule.get_t_0(),
        .edge =
            instance.network.get_successors(train_schedule.get_entry()).front(),
        .position    = 0,
        .orientation = true,
        .speed       = train_schedule.get_v_0()};

    sim::EdgeTrajectory edge_traj(instance, train, solution.v_targets,
                                  init_state);
    sim::EdgeEntry      transition = edge_traj.enter_next_edge(0.3);

    edge_traj.check_speed_limits();
  }
}

TEST(Simulation, TrainTrajectory) {
  const ulong seed =
      std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::ranlux24_base rng_engine(seed);
  Network            network =
      Network::import_network("./example-networks/SimpleNetwork/network/");
  Timetable timetable = Timetable::import_timetable(
      "./example-networks/SimpleNetwork/timetable/", network);

  sim::SimulationInstance               instance(network, timetable, 20);
  std::uniform_int_distribution<size_t> random_train_index(
      0, timetable.get_train_list().size() - 1);
  std::uniform_int_distribution<size_t> random_target_amount(1, 100);

  for (int i = 0; i < 100; i++) {
    size_t train_idx = random_train_index(rng_engine);

    Train train = timetable.get_train_list().get_train(train_idx);

    sim::RoutingSolution solution(random_target_amount(rng_engine),
                                  instance.n_switch_vars, instance.n_timesteps,
                                  train, rng_engine);

    sim::TrainTrajectory traj(instance, train, solution);

    traj.check_speed_limits();
  }
}

TEST(Simulation, TrainTrajectorySet) {
  const ulong seed =
      std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::ranlux24_base rng_engine(seed);
  Network            network =
      Network::import_network("./example-networks/SimpleNetwork/network/");
  Timetable timetable = Timetable::import_timetable(
      "./example-networks/SimpleNetwork/timetable/", network);

  sim::SimulationInstance               instance(network, timetable, 20);
  std::uniform_int_distribution<size_t> random_train_index(
      0, timetable.get_train_list().size() - 1);
  std::uniform_int_distribution<size_t> random_target_amount(1, 100);

  for (int i = 0; i < 100; i++) {
    sim::RoutingSolutionSet solution_set{instance, rng_engine};
    sim::TrainTrajectorySet traj{instance, solution_set};

    ASSERT_EQ(solution_set.solutions.size(), 4);
    ASSERT_EQ(traj.size(), 4);
    traj.check_speed_limits();
  }

  sim::RoutingSolutionSet solution_set{instance, rng_engine};
  sim::TrainTrajectorySet traj{instance, solution_set};
  cda_rail::is_directory_and_create("tmp");
  std::filesystem::path p = "tmp/trajectory.csv";
  traj.export_csv(p);
  std::filesystem::remove_all("tmp");
}

// TODO: test for invariance of solution after being repaired and used again
