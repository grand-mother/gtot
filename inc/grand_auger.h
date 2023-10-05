//
// Created by lewhoo on 24/03/23.
//

#ifndef GTOT_GRANDAUGER_H
#define GTOT_GRANDAUGER_H

#include <vector>
#include <string>

using namespace std;

namespace grand_auger
{
	//! Origin of the coordinate system used for the array
	// Position of the 6th antenna counting from 1
	const float origin_geoid[3] = {40.98456, 93.95225, 1200};
	//! Detector unit (antenna) ID
	const vector<int> du_id = {49, 59, 60, 83, 70, 84, 58, 144, 151};
	//! Detector unit (antenna) (lat,lon,alt) position
	const vector<vector<float>> du_geoid = {{-69.5308828, -35.1104783, 1205.9284},
											{-69.52946829999999, -35.1123499, 1200.0},
											{-69.52675889999999, -35.112391099999996, 1191.5459},
											{-69.5294851, -35.1162916, 1200.0},
											{-69.5281835, -35.114307, 1200.0},
											{-69.5267858, -35.1163491, 1200.0},
											{-69.532292, -35.112398, 1200.0},
											{-69.5274144, -35.110479999999995, 1212.4777},
											{-69.5253518, -35.1134566, 1200.0}};

	//! Detector unit (antenna) (x,y,z) position in site's referential
	const vector<vector<float>> du_xyz = {{675.4017605581184, 430.9601688997285, 5.9028607812831755},
										  {402.21608273980263, -24.062547091056164, -0.004079215845324248},
										  {589.1504963382326, -525.5824323059244, -8.500960900647325},
										  {404.3093633028301, -869.0888468676005, -0.08163399246852812},
										  {-85.430936965152, 871.5963325536272, -0.0430201055102979},
										  {0.0, 0.0, 0.0},
										  {-155.6634338399852, -839.9943892245229, -0.07896424487489591},
										  {-340.855121913717, 413.26624417342043, 12.456397396290665},
										  {-357.8054796014014, -410.0573792198407, -0.040322189478145046},
										  {-861.9627914601336, 446.0219903740909, -0.08396481938601646},
										  {-1052.9785322441526, -4.470543647518846, 18.48740477148975},
										  {-849.5187799823594, -479.0686998058622, -0.10450728073232532},
										  {415.6087874604296, 853.8657144320816, 7.6306648463446285}};
	//! Detector unit type
	const vector<string> du_type = {"GP13 antenna v1", "GP13 antenna v1", "GP13 antenna v1", "GP13 antenna v1", "GP13 antenna v1", "GP13 antenna v1", "GP13 antenna v1", "GP13 antenna v1", "GP13 antenna v1", "GP13 antenna v1", "GP13 antenna v1", "GP13 antenna v1", "GP13 antenna v1"};
	//! Detector unit (antenna) angular tilt
	const vector<vector<float>> du_tilt = {{0, 0},
									 {0, 0},
									 {0, 0},
									 {0, 0},
									 {0, 0},
									 {0, 0},
									 {0, 0},
									 {0, 0},
									 {0, 0},
									 {0, 0},
									 {0, 0},
									 {0, 0},
									 {0, 0}};
	//! Angular tilt of the ground at the antenna
	const vector<vector<float>> du_ground_tilt = {{0, 0},
											{0, 0},
											{0, 0},
											{0, 0},
											{0, 0},
											{0, 0},
											{0, 0},
											{0, 0},
											{0, 0},
											{0, 0},
											{0, 0},
											{0, 0},
											{0, 0}};;
	//! Detector unit (antenna) nut ID
	const vector<int> du_nut = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	//! Detector unit (antenna) FrontEnd Board ID
	const vector<int> du_feb = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}

#endif //GTOT_GRANDAUGER_H
