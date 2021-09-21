#include "clustering_demo.h"

#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerGeom.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>
#include <phool/phool.h>

#include <TFile.h>
#include <TH1F.h>
#include <TH3F.h>
#include <TROOT.h>

//____________________________________________________________________________..
clustering_demo::clustering_demo(const std::string &name):
 SubsysReco(name)
{
}

//____________________________________________________________________________..
int clustering_demo::Init(PHCompositeNode *topNode)
{
  tower_energy = new TH1F("tower_energy", "", 100, 0, 100);
  cluster_energy = new TH1F("cluster_energy", "", 100, 0, 100);
  cluster_xyz = new TH3F("cluster_xyz", "", 200, -200, 200, 200, -200, 200, 300, -500, 500);
  energy_fraction = new TH1F("energy_fraction", "", 20, 0, 1);
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int clustering_demo::process_event(PHCompositeNode *topNode)
{
  // Grab the calorimeter's tower node
  std::string tower_node_name = "TOWER_CALIB_" + detector;
  RawTowerContainer *towers = findNode::getClass<RawTowerContainer>(topNode, tower_node_name);
  if (towers == nullptr) {
    std::cerr << PHWHERE << "Could not find tower node " << tower_node_name<< std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // Grab the calorimeter's gemoetry tower node
  std::string tower_geom_node_name = "TOWERGEOM_" + detector;
  RawTowerGeomContainer *tower_geom = findNode::getClass<RawTowerGeomContainer>(topNode, tower_geom_node_name);
  if (tower_geom == nullptr) {
    std::cerr << PHWHERE << "Could not find tower geometry node " << tower_geom_node_name<< std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // Add tower energies to the histogram
  RawTowerContainer::ConstRange tower_begin_end = towers->getTowers();
  for (RawTowerContainer::ConstIterator itr = tower_begin_end.first; itr != tower_begin_end.second; ++itr) {
    RawTower *tower = itr->second;
    if (tower->get_energy() > 1) {
      tower_energy->Fill(tower->get_energy());
    }
  }

  // Grab the calorimeter's cluster node
  std::string cluster_node_name = "CLUSTER_" + detector;
  RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topNode, cluster_node_name);
  if (clusters == nullptr) {
    std::cerr << PHWHERE << "Could not find cluster node " << cluster_node_name << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // Add the cluster energies to the histogram
  RawClusterContainer::ConstRange cluster_begin_end = clusters->getClusters();
  for (RawClusterContainer::ConstIterator itr = cluster_begin_end.first; itr != cluster_begin_end.second; ++itr) {
    RawCluster *cluster = itr->second;
    cluster_energy->Fill(cluster->get_energy());
    cluster_xyz->Fill(cluster->get_x(), cluster->get_y(), cluster->get_z());

    // Let's play with the towermap...
    RawCluster::TowerMap tower_map = cluster->get_towermap();
    RawCluster::TowerIterator twr_itr; 
    float max_e = -1;
    for (twr_itr = tower_map.begin(); twr_itr != tower_map.end(); ++twr_itr) {
      max_e = twr_itr->second > max_e ? twr_itr->second : max_e;  // If we just want the energy, use the second element
      
      RawTower *cluster_tower = towers->getTower(twr_itr->first); // Or we can get the entire tower for calculating things like shape
      RawTowerGeom *cluster_tower_geom = tower_geom->get_tower_geometry(twr_itr->first); 
      float weighted_x = cluster_tower->get_energy() * cluster_tower_geom->get_center_x(); // for example
      
      weighted_x++; // to avoid unused-variable warning at compilation, meaningless
    }
    energy_fraction->Fill(max_e / cluster->get_energy());
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int clustering_demo::End(PHCompositeNode *topNode)
{
  TFile *f = new TFile(outfile.c_str(), "RECREATE");
  tower_energy->Write();
  cluster_energy->Write();
  cluster_xyz->Write();
  energy_fraction->Write();
  f->Close();
  delete f;
  delete tower_energy;
  delete cluster_energy;
  return Fun4AllReturnCodes::EVENT_OK;
}
