# mission statement           {#mission_statement}

* Provide a component for managing multi-dimensional image data at large scale (> 1 million bitmaps).
* The image data under consideration is of huge size, and organized in multiple dimensions. A particular use case is mosaics with pyramid.
* Three-dimensional data organized as bricks is to be supported.
* flexibility for maintaining non-image-data as well as proxy-data.
* accessing the data is fast, queries make use of indices.
* SQLite is prime target data-base, but it should be prepared for other databases.
* Component is usable from .NET.