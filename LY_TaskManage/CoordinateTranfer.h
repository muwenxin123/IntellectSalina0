#ifndef CoordinateTranfer_h__
#define CoordinateTranfer_h__

#ifndef PI
	#define PI 3.14159265358979323846
#endif

class CoordinateTranfer
{
public:

	static void LonLat2XYZ(double dLon,double dLat,double dAlt,double& x,double& y,double& z);

	static void CreateTransferMatrix( double dLon,double dLat,double matrix[3][3]);

	static void World2Local(double dLon,double dLat,double dAlt,double dLon1,double dLat1,double dAlt1,double& xN,double& yE,double& zUp);
	static void World2Local(double dLon,double dLat,double dAlt,int nPtNum,double pLonOri[],double pLatOri[],double pAltOri[]
		,double px[],double py[],double pz[]);

	static void XYZ2LonLat(double x,double y,double z,double& dLon,double& dLat,double& dAlt);

	static void CreateReverseMatrix( double dLon,double dLat,double matrix[3][3]);

	static void Local2World(double dLon,double dLat,double dAlt,double xN,double yE,double zUp,double& dLon1,double& dLat1,double& dAlt1);
	static void Local2World(double dLon,double dLat,double dAlt,int nPtNum,double xN[],double yE[],double zUp[]
		,double lon[],double lat[],double alt[]);

public:

	static void WorldXYZ2Local(double dLon,double dLat,double dAlt,double x,double y,double z,double& xN,double& yE,double& zUp);
	static void WorldXYZ2Local(double dLon,double dLat,double dAlt,int nPtNum,double x[],double y[],double z[],
		double xN[],double yE[],double zUp[]);

	static void Local2WorldXYZ(double dLon,double dLat,double dAlt,double xN,double yE,double zUp,double& x,double& y,double& z);

	static void	LonLat2Polar(double dLon,double dLat,double dAlt,double dLon1,double dLat1,double dAlt1,double& b,double& d,double& h);

	static void Local2Local(double dLon,double dLat,double dAlt,double dLon1,double dLat1,double dAlt1,double x1,double y1,double z1,double& x,double& y,double& z);

	static void LocalXYZ2Polar(double xN,double yE,double zUp,double& b,double& d,double& h);
	static void Polar2LocalXYZ(double b,double d,double h,double& xN,double& yE,double& zUp);

	static void	Vector_World2Local(double dLon,double dLat,double x,double y,double z,double& xN,double& yE,double& zUp);
	static void	Vector_Local2World(double dLon,double dLat,double xN,double yE,double zUp,double& x,double& y,double& z);
	static void	Vector_Local2Local(double dLon,double dLat,double dLon1,double dLat1,double x1,double y1,double z1,double& x,double& y,double& z);

public:

	static double Local_Alt2Z(double altStation,double x,double y,double alt);
	static double Local_Z2Alt(double altStation,double x,double y,double z);

	static void j_z(double dLon,double dLat,double dAlt,double b,double d,double alt,double& x,double& y,double& z);
	static void z_j(double dLon,double dLat,double dAlt,double x,double y,double z,double& b,double& d,double& alt);

	static void	LocalXYAlt2LonLat(double dLon,double dLat,double dAlt,double xN,double yE,double alt_,double& lon_,double& lat_);
};

namespace DFM
{

	double Distance(double dLon,double dLat,double dAlt,double dLon1,double dLat1,double dAlt1);
	double HorizonDistance(double dLon,double dLat,double dAlt,double dLon1,double dLat1,double dAlt1);

	double Direction(double dLon,double dLat,double dAlt,double dLon1,double dLat1,double dAlt1);
	double Direction(double xN,double yE);
}

double get_distance_from_lat_lon(double lat1, double lon1, double lat2, double lon2);

#endif

