TODO
------------------------------------------
- [ ] add tile type (regular,stone,rocks,water,sand)
- [ ] fill registry with prices
- [ ] buy locked area
- [ ] handle area states for every island -> which ones are still locked
- [ ] path finding- but where would be the starting point?
- [ ] do workers just walk around idle?
- [ ] BUG - after loading - regular tasks disappear when finished

10.08.2015
- [x] include error handler which is gathering error message and codes

06.08.2015
- [x] load - load current time and adjust all queues (tick with timeUnits) - WON'T FIX

05.08.2015
- [x] remove building after collected when in manual mode

04.08.2015
- [x] save current time
- [x] serialize state of the registries (basically only task registry)

30.07.2015
- [x] write out rewards when a task has finished
- [x] task system (?)
- [x] Include logging
- [x] add world level system based on experience
- [x] tasks are based on world level

27.07.2015
- [x] start -> if CM_MANUAL check if it is not done (pending)
- [x] check that every ID (sign / first col) in CSV file is unique
- [x] extract registries into individual files
- [x] registry - show line number and proper error message

24.07.2015
- [x] load - check if there are all required regular josb running
- [x] registry - verify foreign keys (mainly sign) - report error
- [x] price registry - extract prices into individual files like HT.txt
- [x] convert all txt files to the new named csv format
- [x] every island is split into areas which are visible or not and user needs to buy areas
- [x] add hidden attribute to tiles -> TS_LOCKED bit
- [x] read area definition from file containig tiles
- [x] world -> load islands.txt and process this file
- [x] build island map (1 Home base and 1 Hut and x tress and y fruits spread randomly)
- [x] island - define supported buildings / resources
- [x] island define size (make Tiles size dynamic)

23.07.2015
- [x] price registry - take all signs from buildings registry and try to read files

21.07.2015
- [x] add describe with details about building
- [x] move commands to separate classes and add to Simulation
- [x] command line - add char* as paramter (like build 6 6 HT)
- [x] include SRESULT struct as general result code for methods - WON'T FIX
- [x] build command -> show resources of costs

20.07.2015
- [x] requirements -> use CSV format
- [x] load / save -> use index in name

17.07.2015
- [x] map command use center_x/center_y parameter and show only center around 16x16
- [x] buildings -> should have different sizes and occupy tiles

16.07.2015
- [x] build bigger world (256 x 256)
- [x] add island (world -> 1:n -> island)
- [x] world support multiple islands
- [x] support local and global resources
- [x] add switch command to switch to other worlds
- [x] remove id from resource definitions
- [x] remove id from building definitions
- [x] add remove command (work type delete)
- [x] add load_world with index command to load text file as world
- [x] load world as text file for initial version
- [x] extract queue which returns list of events
- [x] add removable "buildings" like a tree or a stone henge (once started will be removed from the world)

15.07.2015
- [x] start work -> select level -> check if level is available
- [x] world add method -> check if we need to create regular work

14.07.2015
- [x] add step - timeunit multiplier
- [x] when build/upgrade check if permanent prices are available
- [x] CM_MANUAL -> certain resources needs to be returned immediately (worker)
- [x] remove max_level from buildings -> already defined if no registry entry
- [x] make auto collect a config value
- [x] add collect command -> no auto collection 

13.07.2015
- [x] build command - check if there is already a building in the queue to be build at that position
- [x] implement move command 
- [x] add permanent resource handling
- [x] add "buildings" command which will list building definitions
- [x] add resource db
- [x] add 1:n relation to costs
- [x] regular -> houses produce x gold every x timeunits
- [x] reschedule work items for permanent jobs

12.07.2015
- [x] registry -> add dependencies/requirements (if you want to build a battery you need a Home with level 2) etc.
- [x] read builings from config file
- [x] parser : check number of arguments
- [x] add building only after successfull build
- [x] calculate max resources based on buildings
- [x] load and save max resources as well (not necessary since we calculate max resources after load)
- [x] building new buildings might increase max_resources -> build registry for this

10.07.2015
- [x] building should take prices / costs from registry
- [x] world -> save
- [x] world -> load

09.07.215:
- [x] Parser -> extract CommandLine
- [x] start command
- [x] world -> status translate resource types
- [x] build database for upgrade costs
- [x] database must support start costs
- [x] check resources
