# Introduction

This is my attempt at building a simulation engine/game. The main concept
is based on games like "The Tribez" and similiar. 

# Key concepts

The simulation is based on one world. The world itself can have many islands.

The simulation can run in two different modes:
Immediate - This means that every resource will be collected immediately after a work unit has finished
Collect - The user has to collect the resources which are marked as collectable after a work unit has finished

# Defining the world

The following will describe the basic parts of the simulation. Everything
is defined in CSV files so nothing is actually hardcoded. The references
are handled by signs. These are basically two letter words used as
unqiue identifier.

# Work

The simulation supports six different kind of work items.
There are three different stages when a work is executed.
First it will be started (S). This may costs certain resources.
The prices are defined in the price registry and described below.
After the duration the work will be finished (F). Depending on the
simulation mode the resources will be collected (C) immediately or
the user has to do it.

## WORK

Starts the work on building.

## UPGRADE

Will upgrade a building

## DELETE

If the building is destructable it will remove the building

## BUILD

## PERMANENT

## REGULAR

## Resources

First we need to define the resources that are available within our world.

The resources are defined as a CSV file located in the data subdirectory. 
The file is called resource_definitions.txt. Every definition has a sign
which is a two character symbol which must be unique within this list.
There is also a flag defining the resource is global or local. The resource
Money for example is global so there is only one for the entire world.
All local resources are only available on each island.

| field | description                                                   |
|-------|---------------------------------------------------------------|
|Sign   | two character symbol for this resource                        |
|Global | a flag that defines of the resource is global or local        |
|Name   | full name of the resource                                     |

Example:
```
# sign / global (G=global L = local) / name 
MO,Y,Money
IR,N,Iron
FO,N,Food
WO,N,Worker
```

## Buildings

The available buildings are defined in the buildings.txt file. The CSV
file consists of

| field       | description                                                   |
|-------------|---------------------------------------------------------------|
|Sign         | two character symbol                                          |
|size_x       | how many tiles in x direction it needs                        |
|size_y       | how many tiles in y direction it needs                        |
|permanent    | a flag defining if this building produces permanent resources |
|regular      | a flag defining if this building produces regular resources   |
|max_level    | the maximum level                                             |
|destructable | a flag defining if this building can be removed               |

Example:
```
HB,2,2,N,Y,1,N,HomeBase
HT,1,1,N,Y,3,Y,Hut
HN,1,1,N,Y,3,Y,Nice Hut
```

The Hut with the symbol HT will occupy one tile in each direction. It produces no permanent resources.
But it produces regular resources as for example tax (money). The maximum level is 3 and it is
destructable.

## Max resources

The maximum amount of resources for every building and level can be defined in the max_resources.txt.
The total amount of maximum resources on any island will be the sum of all buildings and levels taken
from this list.

| field   | description                        |
|---------|------------------------------------|
|Sign     | the symbol of the building         |
|level    | the level of the building          |
|resource | the symbol of the resource         |
|max      | the maximum amount of the resource |

```
HB,1,WD,500 
HB,1,FO,500
# hut
HT,1,WO,1
HT,2,WO,2
HT,3,WO,3
HT,4,WO,4
```

For example the first line means that the HB (home base) with level 1 will add 500 WD (wood) to the maximum resources.

## Requirements

# home base level 2 requires 3 houses level 3
```
HB,2,HT,3,3
```

## Price registry

# GUI

There is none. Right now it is an ascii based UI with a command line.