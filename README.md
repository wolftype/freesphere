#### FREESPHERE ####

AlloSystem + OmniRender tests

  git clone https://github.com/wolftype/freesphere.git
  cd freesphere
  git submodule update
  git submodule init

get an invite to the arg calibration data folder!!!  
download proj9.bin and proj10.bin from box
make a calibration-current folder in freesphere repo
add the .bin files to calibration-current folder

  ./run.sh src/main.cpp


For reference, this repo was constructed the following way:

0. AlloSystem was downloaded (better way would be to wget project_generator.py)
1. Initialize new project by project_generator.py in AlloSystem/tools/
2. Add OmniRender as submodule
