/**
 * Localizing Jockey based on local costmap (costmap position is relative to
 * the sensor but orientation is absolute).
 *
 * The role of this jockey is to get the dissimilarity of the PlaceProfile
 * descriptors of all vertices with the current PlaceProfile.
 * The action is done when the dissimilarities are computed.
 * Implemented actions:
 * - GET_VERTEX_DESCRIPTOR: return the PlaceProfile and the computed Crossing
 * - GET_SIMILARITY: return the dissimilarity based on PlaceProfile
 *
 * Interaction with the map (created by this jockey):
 * - [Getter][/][Setter], message type, interface default name
 * - Getter/Setter: PlaceProfile, jockey_name + "_place_profile"
 * - Setter: Crossing, jockey_name + "_crossing"
 *
 * Interaction with the map (created by other jockeys):
 * - [Getter][/][Setter], message type, interface default name
 *
 * Subscribers (other than map-related):
 * - message type, topic default name, description
 * - nav_msgs/OccupancyGrid, "~/local_costmap", local cost map which orientation is global
 *
 * Publishers (other than map-related):
 * - message type, topic default name, description
 *
 * Services used (other than map-related):
 * - service type, server default name, description
 * - place_matcher_msgs::PolygonDissimilarity, "~/compute_dissimilarity", used to
 *    compare all known places (as polygons) with the current place
 *
 * Parameters:
 * - name, type, default name, description
 * - ~/costmap_interface_name, String, jockey_name + "_place_profile", name of the map interface for place profiles.
 * - ~/crossing_interface_name, String, jockey_name + "_crossing", name of the map interface for crossing.
 * - ~/localize_service, String, "localize_in_vertex", name of the
 *     dissimilarity service for LOCALIZE_IN_VERTEX (only the pose is considered).
 * - ~/dissimilarity_server_name, String, "compute_dissimilarity", name of the
 *     dissimilarity service (only the dissimilarity is considered).
 * - ~/range_cutoff, Float, 0, points farther than this are considered to be
 *     free of obstacle and frontiers may exist. 0 means ignore.
 */

#ifndef LJ_COSTMAP_JOCKEY_H
#define LJ_COSTMAP_JOCKEY_H

#include <ros/ros.h>

#include <lama_common/place_profile_conversions.h>
#include <lama_interfaces/ActOnMap.h>
#include <lama_interfaces/AddInterface.h>
#include <lama_msgs/DescriptorLink.h>
#include <lama_jockeys/localizing_jockey.h>
#include <lama_msgs/GetPlaceProfile.h>
#include <lama_msgs/SetPlaceProfile.h>
#include <lama_msgs/SetCrossing.h>
#include <place_matcher_msgs/PolygonDissimilarity.h>

#include <crossing_detector/costmap_crossing_detector.h>

namespace lj_costmap
{

class Jockey : public lama_jockeys::LocalizingJockey
{
  public:

    Jockey(std::string name, double frontier_width, double max_frontier_angle=0.785);

    virtual void onGetVertexDescriptor();
    virtual void onLocalizeInVertex();
    virtual void onGetDissimilarity();

    void setDissimilarityServerName(std::string name) {dissimilarity_server_name_ = name;}

  private:

    void initMapPlaceProfileInterface();
    void initMapCrossingInterface();
    void getLiveData();
    void handleMap(const nav_msgs::OccupancyGridConstPtr& msg);

    lama_msgs::DescriptorLink placeProfileDescriptorLink(int32_t id);
    lama_msgs::DescriptorLink crossingDescriptorLink(int32_t id);

    // ROS parameters.
    double range_cutoff_;  //!< A range longer that this is considered to be free (m).

    // Internals.
    bool data_received_;
    bool range_cutoff_set_;

    // Reception and storage of OccupancyGrid and PlaceProfile.
    nav_msgs::OccupancyGrid map_;
    lama_msgs::PlaceProfile profile_;

    // Map interface for PlaceProfile and Crossing descriptors.
    std::string place_profile_interface_name_;
    ros::ServiceClient place_profile_getter_;
    ros::ServiceClient place_profile_setter_;
    std::string crossing_interface_name_;
    ros::ServiceClient crossing_setter_;

    // LocalizeInVertex server.
    std::string localize_service_;
    // Dissimilarity server.
    std::string dissimilarity_server_name_;

    crossing_detector::CostmapCrossingDetector crossing_detector_;
};


} // namespace lj_costmap

#endif // LJ_COSTMAP_JOCKEY_H
