# Comparison of Wireless Propagation Loss Models

## Description
This project aims to compare different wireless propagation loss models using the
ns-3 network simulator

## Usage
```bash
# configure directories for this repository and the ns3 allinone
export PROJECT_DIR=/tmp/wifi-propagation-experiment
export WORKING_DIR=/tmp

# clone this repo
git clone https://github.com/0x6e66/wifi-propagation-experiment.git $PROJECT_DIR

# download, unpack, configure and build ns3
wget -O $WORKING_DIR/ns-allinone-3.43.tar.bz2 https://www.nsnam.org/releases/ns-allinone-3.43.tar.bz2
tar -xjf $WORKING_DIR/ns-allinone-3.43.tar.bz2 -C $WORKING_DIR
cd $WORKING_DIR/ns-allinone-3.43/ns-3.43/
./ns3 configure
./ns3 build

# run simulation (could take a while)
cp $PROJECT_DIR/{propagation.cc,helper.h} $WORKING_DIR/ns-allinone-3.43/ns-3.43/scratch/
for m in "friis" "fixed_rss" "three_log" "two_ray" "nakagami"; do ./ns3 run propagation.cc -- --model=$m; done

# set up python environment
python -m venv .venv
source .venv/bin/activate # for bash

# plot results
cp $PROJECT_DIR/{main.py,requirements.txt} $WORKING_DIR/ns-allinone-3.43/ns-3.43/
pip install -r requirements.txt
python main.py
```
