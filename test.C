#include <algorithm>
#include <numeric>
#include <chrono>
#include <random>
#include <string>
#include <tuple>
#include <map>
#include <vector>
struct msg;
template <typename T>
class is_PUPbytes;/*
template <>
struct is_PUPbytes<msg> {
  static const bool value = false;
};*/
#include "test.decl.h"
CProxy_main mainProxy; //readonly
struct msg {
  std::vector<int> data;
  //int data;
  msg(std::vector<int> s) : data(s) {}
  //msg(int s) : data(s) {}
  friend void operator|(PUP::er& er, msg& m) {
    er|m.data;
  }
  msg() : data() {}
};

class main : public CBase_main {
  CProxy_engine engines;
  int N;
  int msgcount;
  int lambda;
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
public:
  main(CkArgMsg* args) {
    if (args->argc == 4) {
      mainProxy=thisProxy;
      N=std::atoi(args->argv[1]);
      msgcount=std::atoi(args->argv[2]);
      lambda=std::atoi(args->argv[3]);
      engines=CProxy_engine::ckNew(N,msgcount,lambda,N);
      start=std::chrono::high_resolution_clock::now();
      engines.simulate();
      //done(0);
      //CkStartQD(CkCallback(CkIndex_main::done(),thisProxy));
    }
    else {
      CkPrintf("Usage: %s N Msgcount lambda [given: %d]\n",args->argv[0], args->argc);
      CkExit();
    }
    delete args;
  }
  void done(int) {
    auto end=std::chrono::high_resolution_clock::now();
    CkPrintf("Time: %ld\n",std::chrono::duration_cast<std::chrono::microseconds>(end-start).count());
    CkExit();
  }
};
class engine : public CBase_engine {
  int N;
  int msgcount;
  int lambda;
  std::mt19937 mt;
  int times;
  int iter;
  int i;
public:
  engine() {}
  engine(int _N, int _msgcount, int _lambda) : N(_N), msgcount(_msgcount), lambda(_lambda), mt(thisIndex), iter(0) {}
  void simulate() {
    for (int j=0;j!=N-1;++j) {
      for (int k=0;k!=100;++k) {
        thisProxy[(thisIndex+j+1)%N].ping(rand());
      }
    }
  } //after sending each message to the other PE
  void ping(msg val) {
    ++iter;
    if (iter == (N-1)*msgcount) {
      //mainProxy.done();
      int stuff=0;
      contribute(sizeof(int), &stuff, CkReduction::sum_int, CkCallback(CkReductionTarget(main,done), mainProxy));
    }
    else if (iter % 100*(N-1) == 0) {
      thisProxy[(thisIndex+1)%N].simulate();
      //int stuff=0;
      //contribute(sizeof(int), &stuff, CkReduction::sum_int, CkCallback(CkReductionTarget(main,done), mainProxy));
    }
  }
  msg rand() {
    int length=lambda/4;
    std::vector<int> gener;
    gener.reserve(length);
    for (int j=0;j!=length;++j) {
      gener.push_back(j);
    }
    return msg(gener);
    //return msg(0);
  }
};
#include "test.def.h"

