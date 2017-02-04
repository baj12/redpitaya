# redpitaya
examples on how to program the redpitaya


# data format

The output format is binary. The first int, ie. 4 bytes hold the version number

## version 1

After the version (4bytes) the decimation (4bytes) and nCount (4bytes) or number of measurement blocks are stored.

Following the header (first 12 bytes) start the data blocks. Since we read half of the available memory the block size is 8192. Channel 1 is written before channel2.
The R script "readBinDatav1.Rmd" implements reading the data using R.

This data format is supposed to be fast for I/O, but can be greatly compressed. I currently do this using gzip, but one could imagine other options...
