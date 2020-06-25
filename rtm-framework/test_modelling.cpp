//
// Created by amr on 12/11/2019.
//
#include <dummy_components/dummy_components.h>
#include <skeleton/base/engine_configuration.h>
#include <skeleton/engine/modelling_engine.h>
#include <skeleton/helpers/callbacks/example/dout_callback.h>

ModellingEngineConfiguration create_dummy_configuration();
ComputationParameters create_dummy_computation_parameters();

int main(int argc, char *argv[]) {
  ModellingEngineConfiguration config = create_dummy_configuration();
  ComputationParameters parameters = create_dummy_computation_parameters();
  CallbackCollection cbs;
  DoutCallback dcb;
  cbs.RegisterCallback(&dcb);
  ModellingEngine engine(&config, &parameters, &cbs);
  engine.Model();
  return 0;
}

ModellingEngineConfiguration create_dummy_configuration() {
  ModellingEngineConfiguration config;
  config.boundary_manager = new DummyBoundaryManager();
  config.computation_kernel = new DummyComputationKernel();
  config.source_injector = new DummySourceInjector();
  config.model_handler = new DummyModelHandler();
  config.modelling_configuration_parser =
      new DummyModellingConfigurationParser();
  config.trace_writer = new DummyTraceWriter();
  return config;
}

ComputationParameters create_dummy_computation_parameters() {
  ComputationParameters p(O_8);
  p.boundary_length = 20;
  p.source_frequency = 200;
  p.half_length = O_8;
  return p;
}