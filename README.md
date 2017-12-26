# Sc2AutoObserver
============
*Auto observer that can be used within the sc2api.
*Early alpha. At the moment it moves the camera for one player.
*Inspired by the SSCAIT-ObserverModule found at https://github.com/Plankton555/SSCAIT-ObserverModule

### How to use:
--------
1. Activate feature layers in the coordinator
---
```c++
sc2::Coordinator coordinator;
coordinator.SetFeatureLayers(sc2::FeatureLayerSettings());
```
The minimap resolution can be adjusted for smoother screen movement. For example:
```c++
sc2::Coordinator coordinator;
auto settings = sc2::FeatureLayerSettings();
settings.minimap_x = 128;
settings.minimap_y = 128;
coordinator.SetFeatureLayers(settings);
```
---
2. Initialize the camera module with an sc2::Agent
```c++
CameraModule cameraModule(sc2::Agent & bot);
```
---
3. Call the onStart and onFrame functions with the onStart and onStep functions of the agent.
```c++
void sc2::Agent::OnGameStart() 
{
	cameraModule.onStart();
}

void sc2::Agent::OnStep() 
{
	cameraModule.onFrame();
}
```
4. Call the moveCameraUnitCreated function with the onUnitCreate function of the agent.
```c++
void sc2::Agent::OnUnitCreated(const sc2::Unit * unit)
{
	cameraModule.moveCameraUnitCreated(const sc2::Unit * unit)
}
```