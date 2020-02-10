# Convert alo raw/digits into o2 Raw data
The run2 data can be converted in a suitable alo format following the instructions [here](../../../aliroot/r23/rawconverter/README.md) or [here](../../../aliroot/r23/rawesdconverter/README.md) (in case the corresponding ESDs are needed as well).
This intermediate format is needed since the alo-aliroot and alo-o2 are separate packages, with only the alo format in common.

Once the file in the alo format is produced, one can convert it into proper o2 raw data with:
```
mid-alo-to-o2-raw <list_of_alo_digits_files>
```
