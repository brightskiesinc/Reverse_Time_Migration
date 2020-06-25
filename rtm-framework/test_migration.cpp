//
// Created by amrnasr on 15/10/2019.
//
#include <dummy_components/dummy_components.h>
#include <skeleton/base/engine_configuration.h>
#include <skeleton/engine/rtm_engine.h>
#include <skeleton/helpers/callbacks/example/dout_callback.h>
#include <skeleton/helpers/timer/timer.hpp>

EngineConfiguration create_dummy_configuration();
ComputationParameters create_dummy_computation_parameters();

int main(int argc, char *argv[]) {
  EngineConfiguration config = create_dummy_configuration();
  ComputationParameters parameters = create_dummy_computation_parameters();
  CallbackCollection cbs;
  DoutCallback dcb;
  cbs.RegisterCallback(&dcb);
  RTMEngine engine(&config, &parameters, &cbs);
  vector<uint> possible_shots = engine.GetValidShots();
  engine.Migrate(possible_shots);
  Timer *timer = Timer::getInstance();
  timer->export_to_file("time.txt" , 1);
  Timer::Timer_Cleanup();
  return 0;
}

EngineConfiguration create_dummy_configuration() {
  EngineConfiguration config;
  config.boundary_manager = new DummyBoundaryManager();
  config.computation_kernel = new DummyComputationKernel();
  config.correlation_kernel = new DummyCorrelationKernel();
  config.forward_collector = new DummyForwardCollector();
  config.source_injector = new DummySourceInjector();
  config.trace_manager = new DummyTraceManager();
  config.model_handler = new DummyModelHandler();
  config.sort_min = 0;
  config.sort_max = 2;
  config.sort_key = "CSR";
  return config;
}

ComputationParameters create_dummy_computation_parameters() {
  ComputationParameters p(O_8);
  p.boundary_length = 20;
  p.source_frequency = 200;
  p.half_length = O_8;
  return p;
}