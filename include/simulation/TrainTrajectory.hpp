#include "simulation/EdgeTrajectory.hpp"

#include <optional>
#include <vector>

namespace cda_rail {

class TrainTrajectory {
  /**
   * Path of a single train over entire timespan
   * Repairs solution speeds
   */
  std::vector<EdgeTrajectory>   edge_trajectories;
  std::vector<InitialEdgeState> initial_edge_states;
  RoutingSolution               solution;

public:
  TrainTrajectory(const SimulationInstance& instance, const Train& train,
                  RoutingSolution solution);

  void match_velocity(const SimulationInstance& instance, const Train& train,
                      SpeedTargets v_targets);
};

InitialEdgeState read_initial_train_state(const SimulationInstance& instance,
                                          const Train&              train);

// Match a velocity before reaching next edge
SpeedTargets match_velocity()

}; // namespace cda_rail
