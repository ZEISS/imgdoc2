# database structure              {#database_structure}

In database we a dealing with the following tables:

| Name | purpose | description |
| - | - | - |
| GENERAL | contains configuration | Here we gather configuration and global data (affecting the operation). This table holds a simple key-value property-bag. |
| TILESINFO | contains "logical information" about tiles | In this table we have the "logical position" of the tiles, and their coordinates. |
| TILESDATA | contains "physical information" about tiles | Here we list "physical information" about the tiles, and have a link to the actual pixeldata. 
| BLOBS | store binary blobs | This table contains binary blobs. |


## The TILESINFO table

The TILESINFO table has the following columns:

| column name | type | description |
| - | - | - |
| Pk | | primary key |
| TileX | double | The x-coordinate of the top-left point of the tile |
| TileY | double | The y-coordinate of the top-left point of the tile |
| TileW | double | The width of the tile |
| TileH | double | The height of the tile |
| PyramidLevel | integer(1) | The pyramid-level of the tile. |
| TileDataId | |key into TILESDATA table |
| Dim_? | integer(4) | coordinate of the tile for a dimension (where the ? can be any of [a-zA-Z]).|
| Dim_? | integer(4) | ...there can be a plurality of dimension-columns |

The coordinates specified for the tile are given in the (document global) _pixel coordinate system_. 

Note that there can be a plurality of columns of the form "Dim_?", where ? can be a letter a-z and A-Z (this is case-sensitive).

## The TILESDATA table

The TILESDATA table stores physical information about the tile, i.e. the bitmap.

The structure is:

| column name | type | description |
| - | - | - |
| Pk | | primary key |
|PixelWidth | Integer(4) | The width of the tile in pixels. |
|PixelHeight | Integer(4) | The height of the tile in pixels. |
|PixelType | Integer(1) | An enumeration specifying the pixel type of the tile. |
|TileDataType| Integer(1) | An enumeration specifying the encoding to the tile. |
|BinDataStorageType | Integer(1) | An enumeration specifying the storage mode of the tile. |
|BinDataId| | A key in order to identify the binary blob.|

**TileDataType** is an enumeration with the following options:

| name | numeric value | description |
| - | - | - |
| ZERO | 0 | All pixels of this tile have the value 'zero', and there is no actual binary data for this tile. In this case, the values of 'BinDataStorageType" and 'BinDataId' are not relevant. |
| UNCOMPRESSED_BITMAP | 1 | The blob contains an uncompressed bitmap (TODO: specify details of the layout) |
| JPGXRCOMPRESSED_BITMAP | 2 | The blob contains a JPGXR-compressed bimatp (TODO: specifiy details) |
| UNCOMPRESSED_BRICK | 32 | The blob contains an uncompressed brick  (TODO: specify details of the layout) |

**BinDataStorageType** is an enumeration with the following options:

| name | numeric value | description |
| - | - | - |
| BlobInDatabase | 1 | The blob is stored inside the database itself. |

Here the idea is that actual data may be stored in some external storage in future versions.






