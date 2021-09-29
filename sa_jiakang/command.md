setting virtual env python

```
source ../../.sa_jiakang/bin/activate
```

setting Log

```
export NS_LOG="scenario=level_info|prefix_func|prefix_time:nodehelper=level_info|prefix_func|prefix_time"
```

run Test

```
./waf --run "scratch/sa_jiakang/sa_jiakang --numUEs=2 --topo_type=test" >&scratch/sa_jiakang/test/log.txt

./waf --run "scratch/sa_jiakang/sa_jiakang --numUEs=20 --topo_type=test" >&scratch/sa_jiakang/test/log.txt
```

run static_full_energy

```
./waf --run "scratch/sa_jiakang/sa_jiakang --time_step=4 --numUEs=20 --topo_type=static_full_energy" >&scratch/sa_jiakang/static_full/log.txt

./waf --run "scratch/sa_jiakang/sa_jiakang --time_step=0.2 --numUEs=20 --topo_type=static_full_energy" >&scratch/sa_jiakang/static/log_full_energy.txt
```

Open Animation

```
../netanim/NetAnim
```

Build NS3

```
export PYTHONPATH=../../build/lib${PYTHONPATH:+:$PYTHONPATH}

./waf configure --enable-tests --enable-example
```
