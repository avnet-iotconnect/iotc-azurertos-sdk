## Contributing To This Project

When contributing to this project, please follow the contributing guidelines 
for [iotc-c-lib](https://github.com/avnet-iotconnect/iotc-c-lib/blob/master/CONTRIBUTING.md)
where applicable and possible.

As opposed to downloading project packages, you clone the repository. Once cloned, ensure to run:

```commandline
./scripts/setup-project.sh name_of_the_project
```
where name_of_the_project is the directory name under samples. setup-project.sh 
script is currently supported on Linux, MacOS, MSYS2 (Windows) and WSL (Windows) 

Additionally, if updating baselines for git modules, update this list of dependencies:
* iotc-c-lib v2.0.4
* cJSON v1.7.13
* libTO hash 2217696249de310b15b17b1a262422de9b3a7d04
