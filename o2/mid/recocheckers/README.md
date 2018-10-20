# Reconstruction checker for MID
This package allows to perform some studies of the reconstruction performance in MC events.

# Usage:
To run the package:
```
mid-reco-check <space_seprated_list_of_track_refs>
```

The track refs files can be obtained by converting the run2 simulation to the run3 data format as explained [here](../../../aliroot/r23/trackrefsconverter/README.md)

The output file is by default `MIDCheckReco.root` and contains:
- the number of generated and reconstructed clusters per RPC
- the residuals between the reconstructed x and y coordinates of the cluster and the corresponding impact point in the chamber
- the number of generated and reconstructed tracks as a function of the total momentum
- the residual of the x, y position and x_slope and y_slope between the reconstructed track and the corresponding generated track extrapolated to the first MID chamber as a function of the total momentum.
- the equivalent plot as above but with the residual divided by the uncertainty (pull)
- the residual of the x, y position and x_slope and y_slope between the reconstructed track and the track parameters that can be obtained from the hit of the corresponding track ref


For the full list of options, please run:
```
mid-convert-rawesd -h
```
