# Fire probability analysis
This package allows to perform an analysis of the probability of firing a strip at a distance _d_ from the impact point.

## Building the fire probability
The fire probability distribution can be built from the simultaneous analysis of (converted) raw data and the corresponding (converted) reconstructed ESD tracks.
These can be generated with the rawesdconverter package (see [here](../../../aliroot/r23/rawesdconverter/README.md) for details).
```
mid-fireprobability mid_merged.dat mid_merged.dat 
```
The fire probability from data is of course only approximate.
The impact point is deduced by extrapolating the tracker track to the MID chambers (without accounting for the uncertainty).
Then for the bending and non-bending plane separately one searches for the maximum distance between the pre-cluster x and y border and the x and y position of the extrapolated track, respectively.
For simplicity, the pre-clusters of the detection elements with more than 1 track are ignored, in order to avoid pileup events.

The output is a file named `MIDFireProbability.root` by defauly.
It contans one histogram per detection element.
The first bin of the x axis contains the distributions in the non-bending plane.
The other bins contain the distributions in each column of the bending plane.

For a list of available options, run:
```
mid-fireprobability -h
```

## Analysing the fire probability
The output obtained in the previous step can be analysed with:
```
mid-fireprobability-analysis --ocdb <ocdb.dat> MIDFireProbability.root
```
The ocdb file contains the high voltage of the RPCs in the run and can be obtained as explained [here](../../../aliroot/r23/ocdbconverter/README.md)
The output file of the executable is by default `MIDFireProbabilityFit.root`.
It contains 1 graph per detection element for the bending and non bending plane.
The distribution for the non-bending plane is obtained by projecting the corresponding bin of the histogram and integrating all entries from 0 to the current bin.
The histogram was indeed filled in a discreate way.
But, if one fills the bin corresponding to distance _d_ between the pre-cluster and the extrapolated impact point of the track, it means that all distances between 0 and _d_ where fired.
The distribution is divided by the integral from 0 to the last bin in order to ensure a value of 1 when the distance is 0.
The distribution for the non-bending plane is built in a similar way, but in this case one integrates over all of the columns.

One can notice that:
- there are events where the full RPC is fired in the bending (non-bending) plane, which correspond to a distance of the order of the RPC width (length), i.e. about 68 cm (240 cm)
- the distribution drops at a distance of about half of the RPC width (length) for the bending (non-bending) plane. This indeed correspond to events where the hit was close to one edge of the RPC and the full width of the RPC was fired, so the distribution is cut in one direction.

The distributions are fitted with the fire probability function that is used in simulations to describe the cluster size, so that the corresponding parameters can be adjusted on data.
The fit is performed with an upper limit to the maximum distance to avoid the drop of the distribution explained above.
