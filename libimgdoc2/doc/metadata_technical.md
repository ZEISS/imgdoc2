# Metatadata: technical implementation           {#pixel_data_encoding}

## data base table layout

The realization is providing a hierarchical key-value store. The mapping of hierarchy
to table is by following the "Adjacency List" pattern (c.f. [here](https://www.databasestar.com/hierarchical-data-sql/)).
