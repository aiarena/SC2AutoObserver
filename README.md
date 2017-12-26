Sc2AutoObserver
============
Auto oberserver that can be used within the sc2api. Early alpha. It is basically a translation of the SSCAIT-ObserverModule found at https://github.com/Plankton555/SSCAIT-ObserverModule

How to use:
--------
1. Activate feature layers in the coordinator (coordinator.SetFeatureLayers(sc2::FeatureLayerSettings());)
2. Initialize the camera module with an sc2::Agent (CameraModule::CameraModule(sc2::Agent & bot))
3. Call the onStart and onFrame functions with the onStart and onStep functions of the agent.
4. Call the CameraModule::moveCameraUnitCreated(const sc2::Unit * unit) with the onUnitCreate function of the agent.