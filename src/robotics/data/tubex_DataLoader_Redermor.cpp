/* ============================================================================
 *  tubex-lib - DataLoader_Redermor class
 * ============================================================================
 *  Copyright : Copyright 2017 Simon Rohou
 *  License   : This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 *
 *  Author(s) : Simon Rohou
 *  Bug fixes : -
 *  Created   : 2018
 * ---------------------------------------------------------------------------- */

#include <time.h>
#include "tubex_DataLoader_Redermor.h"
#include "tubex_Function.h"
#include "tubex_Exception.h"

using namespace std;
using namespace ibex;

namespace tubex
{
  DataLoader_Redermor::DataLoader_Redermor(const string& file_path)
    : DataLoader(file_path)
  {

  }

  void DataLoader_Redermor::load_data(TubeVector *&x, TrajectoryVector *&truth, const Interval& domain)
  {
    clock_t t_start = clock();
    cout << "Loading data..." << flush;

    if(serialized_data_available())
      deserialize_data(x, truth);

    else // loading data from file
    {
      // todo: truncate domain
      if(!m_datafile->is_open())
        throw Exception("DataLoader_Redermor::load_data", "data file not already opened");

      int i = 0;
      string line;
      TrajectoryVector traj_data_x, traj_data_dx, traj_depth, traj_ddepth;
      truth = new TrajectoryVector();

      while(getline(*m_datafile, line))
      {
        i ++;
        if(i < 46) continue; // accessing data
        if(i == 60000) break; // end of data

        double t;
        Vector y(10), dy(10);

        istringstream iss(line);
        if(!(iss >> t
                 >> y[0] >> dy[0]   // phi
                 >> y[1] >> dy[1]   // theta
                 >> y[2] >> dy[2]   // psi
                 >> y[3] >> dy[3]   // vx
                 >> y[4] >> dy[4]   // vy
                 >> y[5] >> dy[5]   // vz
                 >> y[6] >> dy[6]   // depth
                 >> y[7] >> dy[7]   // alt
                 >> y[8] >> dy[8]   // x
                 >> y[9] >> dy[9])) // y
          throw Exception("DataLoader_Redermor::load_data", "fail loading data");

        // Trajectory used for velocities evaluations:
        traj_data_x.set(t, y.subvector(0,5));
        traj_data_dx.set(t, dy.subvector(0,5));

        // Trajectory used for depth enclosure:
        traj_depth.set(t, y.subvector(6,6));
        traj_ddepth.set(t, 0.01 * y.subvector(6,6));

        // Trajectory used as ground truth:
        Vector truth_vector(6);
        truth_vector.put(0, y.subvector(8,9));
        truth_vector[2] = y[6];
        truth_vector.put(3, Vector(3, 0.));
        truth->set(t, truth_vector);
      }

      // Data from sensors with uncertainties:
      TubeVector data_x(traj_data_x, 1.);
      data_x.inflate(traj_data_dx);

      // Computing robot's velocities:
      tubex::Function f("phi", "theta", "psi", "vxr", "vyr", "vzr", 
                        "(vxr * cos(theta) * cos(psi) \
                          - vyr * (cos(phi) * sin(psi) - sin(theta) * cos(psi) * sin(phi)) \
                          + vzr * (sin(phi) * sin(psi) + sin(theta) * cos(psi) * cos(phi)) \
                          ; \
                          vxr * cos(theta) * sin(psi) \
                          + vyr * (cos(psi) * cos(phi) + sin(theta) * sin(psi) * sin(phi)) \
                          - vzr * (cos(psi) * sin(phi) - sin(theta) * cos(phi) * sin(psi)) ; \
                          - vxr * sin(theta) + vyr * cos(theta)*sin(phi) + vzr * cos(theta) * cos(phi))");
      TubeVector velocities = f.eval(data_x);

      // State vector:
      x = new TubeVector(data_x);
      x->resize(6);
      x->put(0, velocities.primitive());
      x->put(3, velocities);

      // Case of the depth, directly sensed:
      TubeVector depth(*x);
      depth.resize(1);
      depth.set_empty();
      depth |= traj_depth;
      depth.inflate(traj_ddepth);
      x->put(2, depth);

      serialize_data(*x, *truth);
    }

    printf(" %.2fs\n", (double)(clock() - t_start)/CLOCKS_PER_SEC);
  }

  vector<Beacon> DataLoader_Redermor::get_beacons() const
  {
    vector<Beacon> v_beacons;
    v_beacons.push_back(Beacon(594.0533723, 374.72930350, 19.));
    v_beacons.push_back(Beacon(599.6093723, 484.55286430, 19.));
    v_beacons.push_back(Beacon(601.4613723, 557.35730860, 19.));
    v_beacons.push_back(Beacon(94.01337232, -2.868189501, 19.));
    v_beacons.push_back(Beacon(119.9413723, 84.751556720, 20.));
    v_beacons.push_back(Beacon(127.3493723, 156.32803660, 20.));
    return v_beacons;
  }
}