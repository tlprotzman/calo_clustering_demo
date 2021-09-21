#include "clustering_demo.h"

#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerDefs.h>

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
    // std::cout << "x: " << cluster->get_x();
    // std::cout << "\ty: " << cluster->get_y();
    // std::cout << "\tz: " << cluster->get_z() << std::endl;
    cluster_xyz->Fill(cluster->get_x(), cluster->get_y(), cluster->get_z());
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
  f->Close();
  delete f;
  delete tower_energy;
  delete cluster_energy;
  return Fun4AllReturnCodes::EVENT_OK;
}
