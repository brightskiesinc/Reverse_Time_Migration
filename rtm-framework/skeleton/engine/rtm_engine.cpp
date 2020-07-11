//
// Created by amrnasr on 20/10/2019.
//
#include <cstring>
#include <iostream>
#include <skeleton/engine/rtm_engine.h>
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

using namespace std;

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress(double percentage, const char* str) {
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%s\t%3d%% [%.*s%*s]", str, val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

RTMEngine::RTMEngine(EngineConfiguration *configuration,
                     ComputationParameters *parameters) {
  this->configuration = configuration;
  this->parameters = parameters;
  this->callbacks = new CallbackCollection();
  this->timer = Timer::getInstance();
}

RTMEngine::RTMEngine(EngineConfiguration *configuration,
                     ComputationParameters *parameters,
                     CallbackCollection *cbs) {
  this->configuration = configuration;
  this->parameters = parameters;
  this->callbacks = cbs;
  this->timer = Timer::getInstance();
}

vector<uint> RTMEngine::GetValidShots() {
    return this->configuration->trace_manager->GetWorkingShots(this->configuration->trace_files,
            this->configuration->sort_min, this->configuration->sort_max, this->configuration->sort_key);
}

MigrationData *RTMEngine::Migrate(vector<uint> shot_ids) {
  this->timer->start_timer("Engine::Migration");
#ifndef NDEBUG
  this->callbacks->BeforeInitialization(parameters);
#endif
  GridBox *grid_box = this->Initialize();

#ifndef NDEBUG
  this->callbacks->AfterInitialization(grid_box);
#endif
  this->configuration->computation_kernel->SetBoundaryManager(
      this->configuration->boundary_manager);
  cout << "Gridbox->dt : " << grid_box->dt << endl;
  cout << "Gridbox->nx : " << grid_box->grid_size.nx << endl;
  cout << "Gridbox->nz : " << grid_box->grid_size.nz << endl;
  cout << "Gridbox->wnx : " << grid_box->window_size.window_nx << endl;
  cout << "Gridbox->wnz : " << grid_box->window_size.window_nz << endl;
  uint shot_num = 0;
  for (uint shot_id : shot_ids) {
    shot_num++;
	  printf("Shot %d/%d\n",
			  shot_num,
			  shot_ids.size());
	  this->configuration->correlation_kernel->ResetShotCorrelation();
      this->timer->start_timer("TraceManager::ReadShot");
	  this->configuration->trace_manager->ReadShot(
        this->configuration->trace_files, shot_id, this->configuration->sort_key);
      this->timer->stop_timer("TraceManager::ReadShot");
#ifndef NDEBUG
    this->callbacks->BeforeShotPreprocessing(
        this->configuration->trace_manager->GetTraces());
#endif
    this->timer->start_timer("TraceManager::PreprocessShot");
    this->configuration->trace_manager->PreprocessShot(
        this->configuration->source_injector->GetCutOffTimestep());
    this->timer->stop_timer("TraceManager::PreprocessShot");
#ifndef NDEBUG
    this->callbacks->AfterShotPreprocessing(
        this->configuration->trace_manager->GetTraces());
#endif
    this->configuration->source_injector->SetSourcePoint(
        this->configuration->trace_manager->GetSourcePoint());
    this->timer->start_timer("ModelHandler::SetupWindow");
    this->configuration->model_handler->SetupWindow();
    this->timer->stop_timer("ModelHandler::SetupWindow");
    this->timer->start_timer("BoundaryManager::ReExtendModel");
    this->configuration->boundary_manager->ReExtendModel();
    this->timer->stop_timer("BoundaryManager::ReExtendModel");
    this->timer->start_timer("ForwardCollector::ResetGrid(Forward)");
    this->configuration->forward_collector->ResetGrid(true);
    this->timer->stop_timer("ForwardCollector::ResetGrid(Forward)");
#ifndef NDEBUG
    this->callbacks->BeforeForwardPropagation(grid_box);
#endif
    this->Forward(grid_box);
    this->timer->start_timer("ForwardCollector::ResetGrid(Backward)");
    this->configuration->forward_collector->ResetGrid(false);
    this->timer->stop_timer("ForwardCollector::ResetGrid(Backward)");
    this->timer->start_timer("BoundaryManager::AdjustModelForBackward");
    this->configuration->boundary_manager->AdjustModelForBackward();
    this->timer->stop_timer("BoundaryManager::AdjustModelForBackward");
#ifndef NDEBUG
    this->callbacks->BeforeBackwardPropagation(grid_box);
#endif
    this->Backward(grid_box);
#ifndef NDEBUG
    this->callbacks->BeforeShotStacking(
        this->configuration->correlation_kernel->GetShotCorrelation(),
        grid_box);
#endif
    this->timer->start_timer("CorrelationKernel::Stack");
    this->configuration->correlation_kernel->Stack();
    this->timer->stop_timer("CorrelationKernel::Stack");
#ifndef NDEBUG
    this->callbacks->AfterShotStacking(
        this->configuration->correlation_kernel->GetStackedShotCorrelation(),
        grid_box);
#endif
  }
#ifndef NDEBUG
  this->callbacks->AfterMigration(
      this->configuration->correlation_kernel->GetStackedShotCorrelation(),
      grid_box);
#endif


  this->timer->start_timer("CorrelationKernel::GetMigrationData");
  MigrationData *migration =
      this->configuration->correlation_kernel->GetMigrationData();
  this->timer->stop_timer("CorrelationKernel::GetMigrationData");
  mem_free((void *)grid_box);
  this->timer->stop_timer("Engine::Migration");
  return migration;
}

GridBox *RTMEngine::Initialize() {
  this->timer->start_timer("Engine::Initialization");
  this->configuration->trace_manager->SetComputationParameters(parameters);
  this->configuration->boundary_manager->SetComputationParameters(parameters);
  this->configuration->computation_kernel->SetComputationParameters(parameters);
  this->configuration->correlation_kernel->SetComputationParameters(parameters);
  this->configuration->forward_collector->SetComputationParameters(parameters);
  this->configuration->model_handler->SetComputationParameters(parameters);
  this->configuration->source_injector->SetComputationParameters(parameters);
  this->timer->start_timer("ModelHandler::ReadModel");
  GridBox *grid = this->configuration->model_handler->ReadModel(
      this->configuration->model_files,
      this->configuration->computation_kernel);
  this->timer->stop_timer("ModelHandler::ReadModel");
  this->configuration->trace_manager->SetGridBox(grid);
  this->configuration->boundary_manager->SetGridBox(grid);
  this->configuration->computation_kernel->SetGridBox(grid);
  this->configuration->correlation_kernel->SetGridBox(grid);
  this->configuration->forward_collector->SetGridBox(grid);
  this->configuration->model_handler->SetGridBox(grid);
  this->configuration->source_injector->SetGridBox(grid);
  this->timer->start_timer("ModelHandler::PreprocessModel");
  this->configuration->model_handler->PreprocessModel(
      this->configuration->computation_kernel);
  this->timer->stop_timer("ModelHandler::PreprocessModel");
  this->timer->start_timer("BoundaryManager::ExtendModel");
  this->configuration->boundary_manager->ExtendModel();
  this->timer->stop_timer("BoundaryManager::ExtendModel");
  this->timer->stop_timer("Engine::Initialization");

  return grid;
}

void RTMEngine::Forward(GridBox *grid_box) {
  this->timer->start_timer("Engine::Forward");
  int onePercent = grid_box->nt / 100 + 1;
  for (uint t = 1; t < grid_box->nt; t++) {
    this->timer->start_timer("ForwardCollector::SaveForward");
    this->configuration->forward_collector->SaveForward();
    this->timer->stop_timer("ForwardCollector::SaveForward");
    this->timer->start_timer("SourceInjector::ApplySource");
    this->configuration->source_injector->ApplySource(t);
    this->timer->stop_timer("SourceInjector::ApplySource");
    this->timer->start_timer("Forward::ComputationKernel::Step");
    this->configuration->computation_kernel->Step();
    this->timer->stop_timer("Forward::ComputationKernel::Step");
#ifndef NDEBUG
    this->callbacks->AfterForwardStep(grid_box, t);
#endif
    if((t % onePercent) == 0)
    {
    	printProgress(((float)t) / grid_box->nt, "Forward Propagation");
    }
  }
  printProgress(1, "Forward Propagation");
  cout << " ... Done" << endl;
  this->timer->stop_timer("Engine::Forward");
}

void RTMEngine::Backward(GridBox *grid_box) {
  this->timer->start_timer("Engine::Backward");
  int onePercent = grid_box->nt / 100 + 1;
  for (uint t = grid_box->nt - 1; t > 0; t--) {
    this->timer->start_timer("TraceManager::ApplyTraces");
    this->configuration->trace_manager->ApplyTraces(t);
    this->timer->stop_timer("TraceManager::ApplyTraces");
    this->timer->start_timer("Backward::ComputationKernel::Step");
    this->configuration->computation_kernel->Step();
    this->timer->stop_timer("Backward::ComputationKernel::Step");
    this->timer->start_timer("ForwardCollector::FetchForward");
    this->configuration->forward_collector->FetchForward();
    this->timer->stop_timer("ForwardCollector::FetchForward");
#ifndef NDEBUG
    this->callbacks->AfterFetchStep(
        this->configuration->forward_collector->GetForwardGrid(), t);
    this->callbacks->AfterBackwardStep(grid_box, t);
#endif
    this->timer->start_timer("CorrelationKernel::Correlate");
    this->configuration->correlation_kernel->Correlate(
    		this->configuration->forward_collector->GetForwardGrid());
    this->timer->stop_timer("CorrelationKernel::Correlate");
    if((t % onePercent) == 0)
    {
    	printProgress(((float)(grid_box->nt - t)) / grid_box->nt, "Backward Propagation");
    }
  }
  printProgress(1, "Backward Propagation");

  cout << " ... Done" << endl;
  this->timer->stop_timer("Engine::Backward");
}
