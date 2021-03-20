# BTP

How to run:

1) Place btp folder inside src of ns3
2) Place main.cc file inside scratch of ns3
3) ./waf --command-template="gdb %s" --run "scratch/main"
4) r


./waf configure
./waf build

How to run without NS-3 version:

1) cd without_NS-3
2) make