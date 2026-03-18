These are the files for the source code behind _Texas Red_ (Outlaw)

**main_arduino** - runs on the leader arduino
- state machine, driving, line-sensing, distance sensing, flywheel control, and interfacing with followers arduinos 1 (stepper motor) and 2 (IR sensing).


**manual_stepper_motor_with_distance_sensors** - follower board 1
- contrary to advertising, this has nothing to do with distance sensors
- this runs the sin drive for scheduling a 2-phase stepper motor and interfacing with the leader

**follower_test** - for communication testing (high vs low)


**follower_test_edge_detection** - for communication testing (handles rising edge interrupt)


**leader_test** - sends high/low signal


**leader_test_edge_detection** - sends pulse

