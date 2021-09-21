// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CLUSTERING_DEMO_H
#define CLUSTERING_DEMO_H

#include <fun4all/SubsysReco.h>

#include <TH1F.h>
#include <TH3F.h>

#include <string>

class PHCompositeNode;

class clustering_demo : public SubsysReco
{
 public:

  clustering_demo(const std::string &name = "clustering_demo");

  virtual ~clustering_demo() {};

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;


  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  void SetDetector(std::string d) { detector = d; };
  void SetOutfile(std::string f) { outfile = f; };

 private:
    std::string detector;
    std::string outfile;

    TH1F *cluster_energy;
    TH1F *tower_energy;

    TH3F *cluster_xyz;

};

#endif // CLUSTERING_DEMO_H
