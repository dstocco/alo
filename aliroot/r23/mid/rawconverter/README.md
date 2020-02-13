# Conversion of RAW data from run2 to run3 format
This package allows to perform a conversion of the RAW data from the run2 to the run3 format.

It is worth noting that, for testing purposes, one does not only need the RAW data, but also the corresponding ESD reconstructed tracks.
For this purpose, a specific package was created (see details [here](../rawesdconverter/README.md))

## Usage
To run the package:
```
mid-ali-to-alo-raw <space_seprated_list_of_raw_files>
```

For the full list of options, please run:
```
mid-ali-to-alo-raw -h
```
