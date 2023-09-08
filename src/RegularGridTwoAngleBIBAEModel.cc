#include "DDML/RegularGridTwoAngleBIBAEModel.h"

#include <G4FastTrack.hh>                // for G4FastTrack

//#include <torch/script.h>

#define DEBUGPRINT 1

namespace ddml {

    void RegularGridTwoAngleBIBAEModel::prepareInput(G4FastTrack const& aFastTrack,
			      G4ThreeVector const& localDir,
			      InputVecs& inputs, TensorDimVecs& tensDims,
			      std::vector<float>& output ) {

      tensDims = _tensDims ;

    G4double energy = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();

    G4ThreeVector position  = aFastTrack.GetPrimaryTrack()->GetPosition();
    G4ThreeVector direction = aFastTrack.GetPrimaryTrack()->GetMomentumDirection();

    // compute local incident angle
    double theta = acos( localDir.z() ) ;

    if( DEBUGPRINT ) 
      std::cout << "  RegularGridTwoAngleBIBAEModel::prepareInput   pos0 = " << position
		<< " - dir = " << direction << " - E = " << energy / CLHEP::GeV
		<< " theta = " << theta
		<< std::endl ;


    // the input for the BIB-AE is one energy and two angles (plus cond tensor)
    inputs.resize(_latentSize);

    inputs[0].resize(1);   // Energy
    inputs[1].resize(1);   // Theta
    inputs[2].resize(1);   // Phi
    inputs[3].resize(3);   // cond (above three combined)

    // For now, assume batch size one, and just assign values
    inputs[0][0] = energy / CLHEP::GeV ;//E_vec[0]/100.;
    inputs[1][0] = M_PI/2. - theta ; // 89.*(M_PI/180.) ; //Theta_vec[0]/(90.*(M_PI/180.));
    inputs[2][0] = phi; // Need to apply some reflection onto training qudrant here!   
    inputs[3][0] = ( inputs[0][0] ) / 126. ;
    inputs[3][1] = ( inputs[1][0] ) / (95.*(M_PI/180.)) ;
    inputs[3][2] = ( inputs[2][0] ) / (95.*(M_PI/180.)) ;
    

    if(DEBUGPRINT) std::cout << " Input_energy_tensor : " <<   inputs[0][0]*100. << std::endl ;

    // ----  resize output vector

    int outputSize = _nCellsX * _nCellsY * _nCellsZ ;

    output.assign(outputSize, 0);
  }

  
void RegularGridTwoAngleBIBAEModel::convertOutput(G4FastTrack const& /*aFastTrack*/,
					  G4ThreeVector const& localDir,
					  const std::vector<float>& output,
					  std::vector<SpacePointVec>& spacepoints ){

    // compute local incident angles

    double phi = atan2( localDir.y() , localDir.x() ) ;

    int nLayer = _nCellsZ ; // number of layers is z dimension

    std::vector<double>  Incident_position = getIncidentCell (double theta, double phi)
    
    double centerCellX = Incident_position[0];
    double centerCellY = Incident_position[1];

    spacepoints.resize( nLayer ) ; 

    int iHit = 0 ;

    for(int l=0 ; l < nLayer ; ++l){

      spacepoints[l].reserve( _nCellsX * _nCellsY ) ;
      
      for(int i=0; i<_nCellsX; ++i){
	for(int j=0; j<_nCellsY; ++j){

	  if( output[ iHit ] > 0. ){

	    // in the current BIB-AE x and y are switched, i.e. the angle is changed along y
	    // does this return the center of the given cell (i.e + 0.5?)?
      // double y = ( i - int(_centerCellX) + 0.5 ) * _cellSizeX ;
	    // double x = ( j - int(_centerCellY) + 0.5 ) * _cellSizeY ;

      // Allow for non-integer incident position in grid
      double y = ( i - centerCellX ) * _cellSizeX ;
	    double x = ( j - centerCellY ) * _cellSizeY ;

	    // rotate the individual layers corresponding to the local azimuth angle phi
	    ddml::SpacePoint sp(
	      x * cos(phi) - y * sin(phi) ,
	      x * sin(phi) + y * cos(phi) ,
	      0.,
	      output[ iHit ] ,
	      0.
	      ) ;
	    
	    spacepoints[l].emplace_back( sp ) ;
	  }
	
	  ++iHit ;
	}
      }
    }
    
  }

  // Get incident cell given theta and phi angles
  // Here input theta and phi in LOCAL COORDINATES in the one-sided box
  std::vector<double> RegularGridTwoAngleBIBAEModel::getIncidentCell (double theta, double phi) {
      // Central point in ILD coordinates
      double center_x = 0.00;
      double center_y = 1889.;
      double center_z = -100.;

      // Point on plane of calorimeter face
      double p_plane_x = 0.;
      double p_plane_y = 1810.0;
      double p_plane_z = -100.;

      // unit vector of particle flight direction defined by directional consine (angles from each axis)
      double u_L_x = cos(phi) * sin(theta);
      double u_L_y = sin(phi) * sin(theta);
      double u_L_z = cos(theta);

      // Get second point on the line form the unit vector
      double p_1_x = center_x + (1 * u_L_x);
      double p_1_y = center_y + (1 * u_L_y);
      double p_1_z = center_z + (1 * u_L_z);

      // Normal vector to the plane of the calorimeter face- points along y axis in ILD coords
      double n_0_x = 0.;
      double n_0_y = 1.;
      double n_0_z = 0.;

      // Calcualte incident position
      // Returns vector in form of [x, y, z]
      std::vector<double> = isect_line_plane_3Vec(center_x, center_y, center_z, p_1_x, p_1_y, p_1_z, p_plane_x, p_plane_y, p_plane_z,
                                                  n_0_x, n_0_y, n_0_z);

      // Apply binning in ILD coordinates (index + frac is incident position)
      // Relabel Z-> Y to avoid confusion
      double pos_X = isect_line_plane_3Vec[0];
      double pos_Y = isect_line_plane_3Vec[2];

      // equivalent of np.arange(-77, 78, 5.088333)
      std::vector<double> binX;
      for (double i = -77; i < 78; i += 5.088333) {
          binX.push_back(i);
      }

      // equivalent of np.arange(-237, 12.5, 5.088333)
      std::vector<double> binY;
      for (double i = -237; i < 12.5; i += 5.088333) {
          binY.push_back(i);
      }

      // find bin index of incident cell and fraction in X/Y
      // X
      for (int i=0;  binX.size(); ++i){        
          
          if ( i+1 > binX.size()){ 
            std::cout << "RegularGridTwoAngleBIBAEModel::getIncidentCell incident position not found in grid X!" << std::endl;
            double gridX = 0;
          }
          
          else if ( binX[i] <= pos_X && pos_X < binX[i+1] ){
              double frac = ( pos_X - binX[i] ) / 5.088333;

              if (frac < 0) {std::cout << "RegularGridTwoAngleBIBAEModel::getIncidentCell incident cell fraction wrong X!"} << std::endl; // has to be positive by construction!

              double gridX = static_cast<double>(i) + frac;

              break;
          }
      }

      // Y
      for (int i=0;  binY.size(); ++i){        
          
          if ( i+1 > binY.size()){ 
            std::cout << "RegularGridTwoAngleBIBAEModel::getIncidentCell incident position not found in grid Y!" << std::endl;
            double gridY = 0;
          }
          
          else if ( binY[i] <= pos_Y && pos_Y < binY[i+1] ){
              double frac = ( pos_Y - binY[i] ) / 5.088333;

              if (frac < 0) {std::cout << "RegularGridTwoAngleBIBAEModel::getIncidentCell incident cell fraction wrong Y!"} << std::endl; // has to be positive by construction!

              double gridY = static_cast<double>(i) + frac;

              break;
          }
      }


      std::vector<double> grid_incident_point;

      grid_incident_point[0] = gridX;
      grid_incident_point[1] = gridY;

      return grid_incident_point;

  }


  // Function to intersect line and plane
  std::vector<double> RegularGridTwoAngleBIBAEModel::isect_line_plane_3Vec (double p0_x, double p0_y, double p0_z, double p1_x, double p1_y, double p1_z, 
                                                                          double p_co_x, double p_co_y, double p_co_z, double p_no_x, double p_no_y, double p_no_z,
                                                                          double epsilon=1e-6):

    /*
    p0, p1: Points on the line
    p_co, p_no: Defiine plane
              p_co: Coords of a point on the plane
              p_no: Normal vector to plane (not necessarily normalised) 
    */

    double u_x = p1_x - p0_x;
    double u_y = p1_y - p0_y;
    double u_z = p1_z - p0_z;

    double dot = (p_no_x * u_x) + (p_no_y * u_y) + (p_no_z * u_z);

    if abs(dot) > epsilon {
      // Ratio of point between p0 -> p1 (0 - 1)
      // if 'ratio' is between (0 - 1), the point intersects with the segment.
      // Else: < 0 - behind p0 ; > 1 in front of p1
      double w_x = p0_x - p_co_x; 
      double w_y = p0_y - p_co_y;
      double w_z = p0_z - p_co_z;
      double ratio = -( (p_no_x * w_x) + (p_no_y * w_y) + (p_no_z * w_z) ) / dot;
      double u_ratio_x = u_x * ratio;
      double u_ratio_y = u_y * ratio;
      double u_ratio_z = u_z * ratio;

      std::vector<float> isect_point;

      isect_point[0] = p0_x + u_ratio_x;
      isect_point[1] = p0_y + u ratio_y;
      isect_point[2] = p0_z + u_ratio_z;

      return isect_point;
    }
    
    else {
      /// If this happens, something is broken :(
      std::cout << "Line intersection in RegularGridTwoAngleBIBAEModel faiiled. Ratio: " << ratio << std::endl;
      return nullptr;
    } 

}
