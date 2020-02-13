# Conversion of the track refs from run2
This package allows to perform a conversion of the TrackRefs from the run2 to the run3 format.

## Usage
To run the package:
```
mid-convert-trackrefs <space_seprated_list_of_path_to_simulations>
```
The paths are the simulation directories containing the `galice.root`, the `kinematics.root` and the `TrackRefs.root`.

For the full list of options, please run:
```
mid-convert-trackrefs -h
```
