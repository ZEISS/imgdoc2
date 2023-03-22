# encoding of pixel data           {#pixel_data_encoding}

## binary layout of the blob

For the blob which ends up in blob-storage, the enum "DataTypes" is determing the binary layout.
There following types are defined:

| DataType enumeration                 | type                                     | description                                                                                |
|--------------------------------------|------------------------------------------|--------------------------------------------------------------------------------------------|
| ZERO=0                               | special case: no blob                    | this data type is indicating: there is no blob, this tile is to be regarded as "all black" |
| UNCOMPRESSED_BITMAP=1                | uncompressed 2D-bitmap                   |                                                                                            |
| JPGXRCOMPRESSED_BITMAP=2             | JPG-XR-compressed 2D-bitmap              |                                                                                            |
| ZSTD0COMPRESSED_BITMAP               | 2D-bitmap compressed with 'zstd0' method | (to be done)                                                                               |
| ZSTD1COMPRESSED_BITMAP               | 2D-bitmap compressed with 'zstd1' method | (to be done)                                                                               |
| UNCOMPRESSED_BRICK=32                | uncompressed 3D-bitmap                   | (to be done)                                                                               |

The bitmap is sufficiently described with the structure 'TileBlobInfo'. This information must be sufficient to reconstruct the bitmap from the blob.

### Uncompressed Bitmap

An uncompressed bitmap is stored with the following conventions:

* The lines are stored from top to bottom.
* The stride is exactly equal to pixelWidth times size-of-a-pixel in bytes. I.e. between consecutive lines, there is not unused data.
* The correct size of the blob is therefore size-of-line times pixelHeight. If the blob is larger, then the surplus data is to be ignored. 
  If it is shorter, then the missing data is to treated as zeroes.

### JPGXR compressed Bitmap

The blob contaings a valid Jpeg-XR aka HDP-file (ISO/IEC 29199-2). The blob is suitable for being decompressed using the Windows WIC-WmpImageCodec.

### Uncompressed Brick

to be done