# Sc2AutoObserver

* Auto observer that can be used within the sc2api.
* Early alpha. At the moment it moves the camera for one player.
* Inspired by the SSCAIT-ObserverModule found at https://github.com/Plankton555/SSCAIT-ObserverModule

### How to use:
--------
1. Initialize the camera module with an sc2::Agent
```c++
CameraModule cameraModule(sc2::Agent & bot);
```
---
2. Call the onStart and onFrame functions with the onStart and onStep functions of the agent.
```c++
void sc2::Agent::OnGameStart() 
{
	...
	cameraModule.onStart();
	...
}

void sc2::Agent::OnStep() 
{
	...
	cameraModule.onFrame();
	...
}
```
---
3. Call the moveCameraUnitCreated function with the onUnitCreate function of the agent.
```c++
void sc2::Agent::OnUnitCreated(const sc2::Unit * unit)
{
	...
	cameraModule.moveCameraUnitCreated(const sc2::Unit * unit)
	...
}
```
---
4. The camera movement is realized via the debug interface. So the debug commands have to be processed.
```c++
void sc2::Agent::OnStep() 
{
	...
	Debug()->SendDebug();
	...
}
```