find_package(FairLogger REQUIRED)

add_executable(mid_performance_mc_workflow MIDPerformanceMCWorkflow.cxx PerformanceMC.cxx PerformanceMC.h)
target_link_libraries(mid_performance_mc_workflow PUBLIC alireaders AliRoot::MUONsim o2::MIDBase o2::MIDSimulation o2::MIDClustering o2::MIDTracking FairLogger::FairLogger)
