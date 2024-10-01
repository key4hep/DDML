/*
 * Apparently both ROOT and Torch define a ClassDef Macro, where the former
 * expects (name,id) and the latter a (TreeRef&&), this makes this approach
 * difficult.
 */

#ifdef __CLING__

  #pragma link off all globals;
  #pragma link off all classes;
  #pragma link off all functions;

  #pragma link C++ nestedclasses + ;

  #pragma link C++ namespace ddml;

  #pragma link C++ struct ddml::SpacePoint + ;
  #pragma link C++ class ddml::InferenceInterface + ;
  #pragma link C++ class ddml::Geant4FastHitMakerGlobal + ;
  #pragma link C++ class ddml::GeometryInterface + ;
  #pragma link C++ class ddml::PolyhedraBarrelGeometry + ;
  #pragma link C++ class ddml::ModelInterface + ;
  #pragma link C++ class ddml::EndcapGeometry + ;
  #pragma link C++ class ddml::RegularGridGANModel + ;
  #pragma link C++ class ddml::FastMLShower + ;
  #pragma link C++ class ddml::TorchInference;

  #pragma link C++ class dd4hep::sim::Geant4Action + ;
  #pragma link C++ class G4FastTrack + ;

  #pragma link C++ namespace std;
  #pragma link C++ class std::vector < float> + ;

#endif // __CLING__
