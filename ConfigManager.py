import pandas as pd
import argparse as arg
import random as rand

import argparse
import random as rand
import os

def positiveFloat(value):
    """ Validate and return a non-negative float value, raise error if negative or not a float. """
    try:
        fvalue = float(value)
    except ValueError:
        raise argparse.ArgumentTypeError(f"{value} is an invalid float. It must be a non-negative float.")
    if fvalue <= 0:
        raise argparse.ArgumentTypeError(f"{value} is a negative value. It must be a non-negative float.")
    return fvalue

def positiveFraction(value):
    """ Validate and return a fraction between 0 and 1, inclusive, raise error if out of bounds. """
    try:
        fvalue = float(value)
    except ValueError:
        raise argparse.ArgumentTypeError(f"{value} is an invalid float. It must be a fraction between 0 and 1.")
    if not (0 < fvalue <= 1):
        raise argparse.ArgumentTypeError(f"{value} must be between 0 and 1, inclusive.")
    return fvalue

def positiveInt(value):
    """ Validate and return a positive integer value, ensuring it's non-negative. """
    try:
        ivalue = int(value)
    except ValueError:
        raise argparse.ArgumentTypeError(f"{value} is not a valid integer")
    if ivalue < 0:
        raise argparse.ArgumentTypeError(f"{value} must be a non-negative integer")
    return ivalue


# Create the parser
parser = argparse.ArgumentParser(description="Simulation Settings")

# args that only effect configuration
parser.add_argument('--manual', action='store_true', help='Manually input settings?')
parser.add_argument('--reset_constants', action="store_true", help='Reset constants to default values.')
parser.add_argument('--tactics', nargs='+', default=None, help='Choose what tactics to use for this simulation. Check constants file for options.')
parser.add_argument('--maneuvers', nargs='+', default=None, help='Choose what maneuvers to use for this simulation. Check constants file for options.')
# parser.add_argument('--clear_data', action='store_true', help="delete all data in data file") #TODO later

#args to save to the settings file
parser.add_argument('--no_draw', action='store_true', help='Render the simulation?')
parser.add_argument('--no_text', action='store_true', help='Number the agents?')
parser.add_argument('--no_play', action='store_true', help='Have agents play games?')
parser.add_argument('--no_cache', action='store_true', help='Save the data to files?')
parser.add_argument('--verbose', action='store_true', help="maximum terminal output")
parser.add_argument('--pause_on_start', action='store_true', help='Have the simulation start paused')
parser.add_argument('--seed', type=int, default=0, help='Set the seed.')
parser.add_argument('--num_agents', type=positiveInt, default=250, help='Number of agents (positive integer, 1000 max)')
parser.add_argument('--agent_radius', type=positiveFraction, default=0.5, help='Agent radius (fraction between 0 and 1, ideal range 0.1 to 0.5 recommended)')
parser.add_argument('--max_rounds', type=positiveInt, default=10000, help='Max rounds (positive integer, 1 billion max)')
parser.add_argument('--gravity_x', type=positiveFloat, default=0.0, help='Gravity x (positive float recommended)')
parser.add_argument('--gravity_y', type=positiveFloat, default=0.0, help='Gravity y (positive float recommended)')
parser.add_argument('--time_step', type=positiveFloat, default=1.0, help='Time steps per frame (positive float recommended)')
parser.add_argument('--velocity_iterations', type=positiveInt, default=6, help='Velocity iterations (positive integer recommended)')
parser.add_argument('--position_iterations', type=positiveInt, default=2, help='Position iterations (positive integer recommended)')
parser.add_argument('--density', type=positiveFloat, default=1.0, help='Density (positive float recommended)')
parser.add_argument('--friction', type=positiveFraction, default=0.3, help='Friction (fraction between 0 and 1 recommended)')
parser.add_argument('--restitution', type=positiveFraction, default=0.5, help='Restitution (fraction between 0 and 1 recommended)')
parser.add_argument('--start_position_x', type=positiveFloat, default=20.0, help='Start position x (positive float recommended)')
parser.add_argument('--start_position_y', type=positiveFloat, default=15.0, help='Start position y (positive float recommended)')
parser.add_argument('--start_force_x', type=positiveFloat, default=1.0, help='Start force x (positive float recommended)')
parser.add_argument('--start_force_y', type=positiveFloat, default=1.0, help='Start force y (positive float recommended)')
parser.add_argument('--speed_cap', type=positiveFloat, default=5.0, help='Speed cap (positive float recommended)')
parser.add_argument('--acceleration_cap', type=positiveFloat, default=0.05, help='Acceleration cap (positive float recommended)')
parser.add_argument('--window_x', type=positiveInt, default=40, help='Window x size (positive integer recommended)')
parser.add_argument('--window_y', type=positiveInt, default=30, help='Window y size (positive integer recommended)')
parser.add_argument('--framerate', type=positiveInt, default=120, help='Frame rate (positive integer recommended)')



# Parse the arguments
args = parser.parse_args()
args.no_draw = int(args.no_draw)
args.no_text = int(args.no_text)
args.no_play = int(args.no_play)
args.no_cache = int(args.no_cache)
args.verbose = int(args.verbose)
args.pause_on_start = int(args.pause_on_start)

args_dict = vars(args)
args_dataframe = pd.DataFrame.from_dict(args_dict, orient='index')
if (args.verbose == True):
    print(args_dataframe)

# Apply seed for random operations
rand.seed(args.seed)

os.makedirs("Configs", exist_ok=True)
os.makedirs("CONSTANTS", exist_ok=True)
os.makedirs("Data", exist_ok=True)

def list_to_single_int(color_list):
    # Combine the list elements into a single integer
    result = 0
    for num in color_list:
        result = (result << 8) | num  # Left shift by 8 bits and add the value
    return result

if ((args.reset_constants == True) or (os.path.exists('CONSTANTS/MANEUVERS.csv') == False)):
    MANEUVER_CONSTANTS = pd.DataFrame({
    "maneuvers": ['Up', 'Down', 'Left', 'Right', 'RandomMove','Still', 'Chase', 'Flee', 'WinChase', 'LossFlee'],
    "maneuver_colors": [[50, 50, 50,255], [100, 100, 100,255], [150, 150, 150,255], [200,200,200,255], [250, 250, 250,255], [0, 0, 0,255], [255, 0, 255,255], [0, 255, 255,255], [128, 0, 128,255], [0, 128, 128, 255]]
    })
    MANEUVER_CONSTANTS['maneuver_colors_binary'] = MANEUVER_CONSTANTS['maneuver_colors'].apply(
    lambda x: int.from_bytes(bytes(x), byteorder='big')
    )

    MANEUVER_CONSTANTS.to_csv('CONSTANTS/MANEUVERS.csv', index=False)

if ((args.reset_constants == True) or (os.path.exists('CONSTANTS/TACTICS.csv') == False)):
    TACTIC_CONSTANTS = pd.DataFrame({
    "tactics": ['Cooperate', 'Defect', 'TitForTat', 'NaiveTitForTat', 'RandomChoice'],
    "tactic_colors": [[0, 255, 0,255], [255, 0, 0,255], [0, 0, 255,255], [128, 0, 0,255], [0, 128, 0, 255]]
    })
    TACTIC_CONSTANTS['tactic_colors_binary'] = TACTIC_CONSTANTS['tactic_colors'].apply(
    lambda x: int.from_bytes(bytes(x), byteorder='big')
    )
    TACTIC_CONSTANTS.to_csv('CONSTANTS/TACTICS.csv', index=False)
    

MANEUVER_CONSTANTS = pd.read_csv('CONSTANTS/MANEUVERS.csv')
TACTIC_CONSTANTS = pd.read_csv('CONSTANTS/TACTICS.csv')

# quite simple, check if the arg matches any dataframe values, and if it does drop the dataframe values that don't match
def get_strategies(args, MANEUVER_CONSTANTS, TACTIC_CONSTANTS):
    tactics = TACTIC_CONSTANTS
    maneuvers = MANEUVER_CONSTANTS
    if args.tactics:
        args.tactics = [m for m in args.tactics if m in set(TACTIC_CONSTANTS['tactics'])]

        if args.tactics:
            tactics = TACTIC_CONSTANTS[TACTIC_CONSTANTS['tactics'].isin(args.tactics)]

    if args.maneuvers:
        args.maneuvers = [m for m in args.maneuvers if m in set(MANEUVER_CONSTANTS['maneuvers'])]

        if args.maneuvers:
            maneuvers = MANEUVER_CONSTANTS[MANEUVER_CONSTANTS['maneuvers'].isin(args.maneuvers)]
    return tactics, maneuvers

tactics, maneuvers = get_strategies(args, MANEUVER_CONSTANTS, TACTIC_CONSTANTS)

agent_maneuvers = maneuvers.sample(args.num_agents, replace=True, random_state=args.seed, ignore_index=True)
agent_tactics = tactics.sample(args.num_agents, replace=True, random_state=args.seed + 1, ignore_index=True)

agent = pd.concat([agent_maneuvers, agent_tactics], axis=1)
agent.index.name = 'agent_id'
agent.to_csv("Configs/Agents.csv", sep=';')


args_dataframe = args_dataframe.drop(['reset_constants', 'manual', 'tactics', 'maneuvers'], axis=0)
args_dataframe.index.name = 'setting'
args_dataframe = args_dataframe.rename(columns={0: 'value'})
args_dataframe.to_csv("Configs/Settings.csv", sep=';')

import subprocess
os.chdir("./build")
subprocess.run(["cmake", "--build",  "."])
subprocess.run(["Thesis.exe"])
print("finished")