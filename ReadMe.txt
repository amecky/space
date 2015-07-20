TODO
------------------------------------------
- fill registry with prices
- task system (?)
- add describe with details about building
- start -> if CM_MANUAL check if it is not done (pending)
- include SRESULT struct as general result code for methods
- add world level system based on experience
- tasks are based on world level
- build island map (1 Home base and 1 Hut and x tress and y fruits spread randomly)
- command line - add char* as paramter (like build 6 6 HT)
- island - define supported buildings / resources
- island define size (make Tiles size dynamic)
- check that every ID (sign / first col) in CSV file is unique
- save current time
- load - load current time and adjust all queues (tick with timeUnits)
- load - check if there are all required regular josb running

20.07.2015
- requirements -> use CSV format
- load / save -> use index in name

17.07.2015
- map command use center_x/center_y parameter and show only center around 16x16
- buildings -> should have different sizes and occupy tiles

16.07.2015
- build bigger world (256 x 256)
- add island (world -> 1:n -> island)
- world support multiple islands
- support local and global resources
- add switch command to switch to other worlds
- remove id from resource definitions
- remove id from building definitions
- add remove command (work type delete)
- add load_world with index command to load text file as world
- load world as text file for initial version
- extract queue which returns list of events
- add removable "buildings" like a tree or a stone henge (once started will be removed from the world)

15.07.2015
- start work -> select level -> check if level is available
- world add method -> check if we need to create regular work

14.07.2015
- add step - timeunit multiplier
- when build/upgrade check if permanent prices are available
- CM_MANUAL -> certain resources needs to be returned immediately (worker)
- remove max_level from buildings -> already defined if no registry entry
- make auto collect a config value
- add collect command -> no auto collection 

13.07.2015
- build command - check if there is already a building in the queue to be build at that position
- implement move command 
- add permanent resource handling
- add "buildings" command which will list building definitions
- add resource db
- add 1:n relation to costs
- regular -> houses produce x gold every x timeunits
- reschedule work items for permanent jobs

12.07.2015
- registry -> add dependencies/requirements (if you want to build a battery you need a Home with level 2) etc.
- read builings from config file
- parser : check number of arguments
- add building only after successfull build
- calculate max resources based on buildings
- load and save max resources as well (not necessary since we calculate max resources after load)
- building new buildings might increase max_resources -> build registry for this

10.07.2015
- building should take prices / costs from registry
- world -> save
- world -> load

09.07.215:
- Parser -> extract CommandLine
- start command
- world -> status translate resource types
- build database for upgrade costs
- database must support start costs
- check resources
