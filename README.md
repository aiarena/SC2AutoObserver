# Sc2AutoObserver

* Auto observer that can be used within the sc2api.
* There are two modes:
	* within a _game_ to simulate the camera movement of one sc2::Agent.
	* within a _replay_ to simulate the camera movement of one sc2::ReplayObserver.
* Inspired by the SSCAIT-ObserverModule found at https://github.com/Plankton555/SSCAIT-ObserverModule

### How to use with sc2::Agent:

1. Initialize the camera module with a sc2::Agent
```c++
CameraModuleAgent cameraModuleAgent(sc2::Agent * const bot);
```

2. Call the onStart and onFrame functions with the onStart and onStep functions of the agent.
```c++
void sc2::Agent::OnGameStart() 
{
	...
	cameraModuleAgent.onStart();
	...
}

void sc2::Agent::OnStep() 
{
	...
	cameraModuleAgent.onFrame();
	...
}
```

3. Call the moveCameraUnitCreated function with the onUnitCreate function of the agent.
```c++
void sc2::Agent::OnUnitCreated(const sc2::Unit * unit)
{
	...
	cameraModuleAgent.moveCameraUnitCreated(const sc2::Unit * unit)
	...
}
```

Please be aware that the camera movement is realized via the debug interface. So the debug commands have to be processed and "Debug()->SendDebug();" will be called every time the camera moves.
An example of an bot that uses the Sc2AutoObserver can be found here: https://github.com/Archiatrus/5minBot

### How to use with sc2::ReplayObserver

1. Initialize the camera module with a sc2::ReplayObserver
```c++
CameraModuleObserver cameraModuleObserver(sc2::Agent * const bot);
```

2. Call the onStart and onFrame functions with the onStart and onStep functions of the Observer.
```c++
void sc2::Observer::OnGameStart() 
{
	...
	cameraModuleObserver.onStart();
	...
}

void sc2::Observer::OnStep() 
{
	...
	cameraModuleObserver.onFrame();
	...
}
```

3. Call the moveCameraUnitCreated function with the onUnitCreate function of the Observer.
```c++
void sc2::Observer::OnUnitCreated(const sc2::Unit * unit)
{
	...
	cameraModuleObserver.moveCameraUnitCreated(const sc2::Unit * unit)
	...
}
```

An example of an command line program that uses the Sc2AutoObserver and plays a replay can be found in the examples directory.