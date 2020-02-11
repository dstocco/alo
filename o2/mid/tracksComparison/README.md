# Compare run2 tracks with run3 reconstructed tracks
This package allows to compare run2 tracks with fully reconstructed run3 tracks.
The input can be obtained by converting the run2 raw data and the corresponding ESD files to a suitable alo format.
This can be done as explained [here](../../../aliroot/r23/rawesdconverter/README.md).
The alo raw/digits data can be further converted into o2 raw data following [these instructions](../raw/README.md).
The o2 raw data can be reconstructed using the standard commands in O2, that produces the reconstructed tracks.

It is then possible to 
```
mid-compare-tracks --alo-esd-event-filename <alo_esd_file> --o2-tracks-filename <o2_track_file> --max-chi2-matching 36
```

The output is a file called by default `MIDCompareTracks.root` containing a series of histograms showing:
-  the matched tracker/trigger tracks in Run2 that match an o2 track (correct match)
-  the tracker-only tracks in Run2 that match an o2 track (improved match or fake matching)
-  the matched tracker/trigger tracks in Run2 that do not match an o2 track (lost track)
-  the tracker-only tracks in Run2 that do not match an o2 track (correct tracker only)