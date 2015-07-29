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

# Registry

There are several registries for each type. The types are described
below. The acutal data is defined in txt files. The format is a kind
of named csv file or a shortened form of JSON. Although this makes
the files itself much more verbose it just helps to edit the data.
All files are located in the data subdirectory.

## Resources

First we need to define the resources that are available within our world.
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
sign : MO , global : Y , name : "Money"
sign : IR , global : N , name : "Iron"
sign : FO , global : N , name : "Food"
sign : WD , global : N , name : "Wood"
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
sign : HB , size_x : 2 , size_y : 2 , permanent : N , regular : Y , max_amount :  1 , destructable : N , name : "Home Base"
sign : HT , size_x : 1 , size_y : 1 , permanent : N , regular : Y , max_amount :  3 , destructable : Y , name : "Hut"
sign : HN , size_x : 1 , size_y : 1 , permanent : N , regular : Y , max_amount :  3 , destructable : Y , name : "Nice Hut"
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
|amount   | the maximum amount of the resource |

```
building : WH , level : 1 , resource : FO , amount : 250 
building : WH , level : 1 , resource : WD , amount : 250
```

For example Warehouse (WH) with level 1 will contribute 250 food and 250 wood to the maximum storable resources
on this island.

## Requirements

In order to upgrade or build certain buildings there might be requirements which must be fullfilled.
For example it might be necessary to have 3 Huts of level 4 before the player can upgrade the home base
to level 2.
The requirements are defined in requirements.txt

| field            | description                            |
|------------------|----------------------------------------|
|building          | the symbol of the building             |
|level             | the level of the building              |
|required_building | the required building                  |
|required_level    | the required level of these buildings  |
|required_amount   | the required amount of these buildings |

```
building : HB , level : 2 , required_building : HT , required_level : 3 , required_amount : 3
```

## Price registry

There is no single defining the costs of any work. For every building there should be a corresponding
file using the sign as name. For example for the building HT (Hut) there should be a HT.txt file.

| field   | description                         |
|---------|-------------------------------------|
|building | the building type                   |
|level    | the level of the building           |  
|work     | the type of work                    |
|stage    | stage of the work                   |
|duration | the duration of the work in seconds |
|resource | the resource that it will cost      |
|amount   | the amount of the resource          |

### Work types

| Sign  | Name  | description                     |
|-------|-------|---------------------------------|
| W     | Work      | start working at a building |
| U     | Upgrade   | start upgrading a building  |
| D     | Delete    | deletes a building          |
| B     | Build     | build a new building        |
| P     | Permanent | permanent work              |
| R     | Regular   | regular work                |

### Stages

There are three different stages of work. First there is start (S). All resources and there amount
will be used when you start a work. After the duration the work will be finished (F). If the
world is running in manual collection mode then there is also collectable (C). Otherwise
the collectable resources will be picked up immediately.

Example:

```
building : HT , level : 1 , work : B , stage : S , duration:  60 , resource : WO , amount : 1
building : HT , level : 1 , work : B , stage : S , duration:  60 , resource : FO , amount : 20
building : HT , level : 1 , work : B , stage : S , duration:  60 , resource : WD , amount : 20
building : HT , level : 1 , work : B , stage : S , duration:  60 , resource : MO , amount : 10
building : HT , level : 1 , work : B , stage : F , duration:   0 , resource : WO , amount : 2
building : HT , level : 1 , work : U , stage : F , duration:   0 , resource : XP , amount : 10
```

## Islands

## Tasks

| field   | description                                      |
|---------|--------------------------------------------------|
|id       | the id of the task                               |
|island   | the type of resource                             | 
|type     | the amount of the resource that will be rewarded |
|resource | the resource type - can be empty (--)            |
|building | the building type - can be empty (--)            |
|level    | required level of the building                   |
|amount   | the actual amount                                |
|previous | the previous task that must be fullfilled        |
|text     | Some text as description                         |

Example:

```
id : 1 , island :  0 , type : D , resource : FO , building : BB , level : 1 , amount :   1 , previous : -1 , text : "Pick Berries by removing one berry brush"
id : 2 , island :  0 , type : D , resource : WD , building : TR , level : 1 , amount :   2 , previous :  1 , text : "Chop down 2 trees"
id : 3 , island : -1 , type : C , resource : MO , building : -- , level : 1 , amount : 300 , previous : -1 , text : "Collect 300 money"
```

## Rewards

There should be some rewards for task. The rewards are defined in the rewards.txt

| field    | description                                      |
|----------|--------------------------------------------------|
|task_id   | the id of the task                               |
|resource  | the type of resource                             | 
|amount    | the amount of the resource that will be rewarded |

Example:

```
task_id : 1 , resource : XP , amount : 10
task_id : 1 , resource : MO , amount : 100
```

# GUI

There is none. Right now it is an ascii based UI with a command line.

# Commands