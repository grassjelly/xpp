 /******************************************************************************
Copyright (c) 2017, Alexander W. Winkler. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#include <xpp_hyq/hyqleg_inverse_kinematics.h>

#include <cmath>
#include <map>

#include <xpp_states/cartesian_declarations.h>
#include <iostream>


namespace xpp {

HyqlegInverseKinematics::Vector3d
HyqlegInverseKinematics::GetJointAngles (const Vector3d& ee_pos_B, KneeBend bend) const
{

}

HyqlegInverseKinematics::Vector3d
HyqlegInverseKinematics::GetJointAngles (int leg_id, const Vector3d& ee_pos_B, KneeBend bend) const
{
  double q_HAA_bf, q_HAA_br, q_HFE_br; // rear bend of knees
  double q_HFE_bf, q_KFE_br, q_KFE_bf; // forward bend of knees
  double HAA_to_HFE;

  Eigen::Vector3d xr;
  Eigen::Matrix3d R;

  HAA_to_HFE = hfe_to_haa_z[Z];
  xr = ee_pos_B;


  if(leg_id == 1 || leg_id ==3)
    HAA_to_HFE = -HAA_to_HFE;

  q_HAA_bf = q_HAA_br = -(atan(xr[Z] / xr[X]) - (1.5708 - acos(HAA_to_HFE / sqrt(pow(xr[Z], 2) + pow(xr[X], 2)))));;
  R <<  cos(q_HAA_bf), 0, sin(q_HAA_bf), 0, 1, 0, -sin(q_HAA_bf), 0, cos(q_HAA_bf);
  xr = (R * xr).eval();
  
  q_KFE_bf = q_KFE_br = -acos((pow(xr[X], 2) + pow(xr[Y], 2) - pow(length_thigh ,2) - pow(length_shank ,2)) / (2 * length_thigh * length_shank));
  q_HFE_bf = q_HFE_br = (atan(xr[Y] / xr[X]) - atan( (length_shank * sin(q_KFE_bf)) / (length_thigh + (length_shank * cos(q_KFE_bf)))));


//   // forward knee bend
//   EnforceLimits(q_HAA_bf, HAA);
//   EnforceLimits(q_HFE_bf, HFE);
//   EnforceLimits(q_KFE_bf, KFE);

//   // backward knee bend
//   EnforceLimits(q_HAA_br, HAA);
//   EnforceLimits(q_HFE_br, HFE);
//   EnforceLimits(q_KFE_br, KFE);

//   if (bend==Forward)
//     return Vector3d(q_HAA_bf, -q_HFE_bf, -q_KFE_bf);
//   else // backward
    return Vector3d(q_HAA_br, q_HFE_br, q_KFE_br);
}

void
HyqlegInverseKinematics::EnforceLimits (double& val, HyqJointID joint) const
{
  // totally exaggerated joint angle limits
  const static double haa_min = -180;
  const static double haa_max =  90;

  const static double hfe_min = -90;
  const static double hfe_max =  90;

  const static double kfe_min = -180;
  const static double kfe_max =  0;

  // reduced joint angles for optimization
  static const std::map<HyqJointID, double> max_range {
    {HAA, haa_max/180.0*M_PI},
    {HFE, hfe_max/180.0*M_PI},
    {KFE, kfe_max/180.0*M_PI}
  };

  // reduced joint angles for optimization
  static const std::map<HyqJointID, double> min_range {
    {HAA, haa_min/180.0*M_PI},
    {HFE, hfe_min/180.0*M_PI},
    {KFE, kfe_min/180.0*M_PI}
  };

  double max = max_range.at(joint);
  val = val>max? max : val;

  double min = min_range.at(joint);
  val = val<min? min : val;
}

} /* namespace xpp */
