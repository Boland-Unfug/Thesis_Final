import pandas as pd
import numpy as np
import argparse as arg

parser = arg.ArgumentParser(description="Data Analysis settings")

parser.add_argument('--dir', type=str, default='..\\Data\\Iteration_0\\', help='Type the path to the files to analyze.')
parser.add_argument('--verbose', action='store_true')
args = parser.parse_args()
if (args.verbose == True):
    print(args)

datadir = args.dir
datadir = datadir.replace("\\", "/")
datadir = datadir.replace("..",".")

def decode_key(history, settings):
    decoded = pd.DataFrame()
    decoded['round'] = np.right_shift(history['key'], 32)
    decoded['round'] = decoded['round'] - (settings.loc[settings['setting'] == 'num_agents', 'value'].values[0] + 1)
    decoded['agent_1_id'] = np.bitwise_and(np.right_shift(history['key'], 16), 0xFFFF)
    decoded['agent_2_id'] = np.bitwise_and(history['key'], 0xFFFF)
    decoded = decoded.astype(int)
    return decoded

def decode_value(history):
    # in order: both cooperate, cooperate-defect, defect-cooperate, both defect
    choice_matrix = np.array([[0,0],[0,1],[1,0],[1,1]]) # can hardcode this bc its always the same
    cleaned = pd.DataFrame(
    choice_matrix[history['value']],  # value = choice matrix index, very fast
    columns=['agent_1_choice', 'agent_2_choice']
    )
    return cleaned

def decode(history, settings):
    cleaned_key = decode_key(history, settings)
    cleaned_value = decode_value(history)
    cleaned = pd.concat([cleaned_key, cleaned_value], axis=1)
    # in the future sort this by round, then agent 1, then agent 2 TODO
    return cleaned

def get_scores(decoded_history, reward_matrix, settings):
    # Constants for the number of agents and maximum number of rounds
    NUM_AGENTS = int(settings.loc[settings['setting'] == 'num_agents', 'value'].values[0])
    MAX_ROUNDS = int(settings.loc[settings['setting'] == 'max_rounds', 'value'].values[0]-1)  # Replace with the maximum round number

    # Generate scores for both agents
    score = pd.DataFrame(
        reward_matrix[
            decoded_history['agent_1_choice'].to_numpy(),
            decoded_history['agent_2_choice'].to_numpy()
        ],
        columns=['agent_1_reward', 'agent_2_reward']
    )

    # Combine scores into a single DataFrame (makes whole process much easier)
    scores = pd.DataFrame({
        'round': pd.concat([decoded_history['round'], decoded_history['round']], axis=0).reset_index(drop=True),
        'agent_id': pd.concat([decoded_history['agent_1_id'], decoded_history['agent_2_id']], axis=0).reset_index(drop=True),
        'reward': pd.concat([score['agent_1_reward'], score['agent_2_reward']], axis=0).reset_index(drop=True)
    })

    # Aggregate rewards by agent_id and round
    score_history = scores.groupby(['agent_id', 'round'], as_index=False)['reward'].sum()

    # Create a complete index of all combinations of agent_id and round
    full_index = pd.MultiIndex.from_product(
        [range(MAX_ROUNDS + 1), range(NUM_AGENTS)], names=['round','agent_id']
    )
    
    full_score_history = score_history.set_index(['round','agent_id']).reindex(full_index, fill_value=0).reset_index()

    # Calculate cumulative rewards
    full_score_history['cumulative_reward'] = full_score_history.groupby('agent_id')['reward'].cumsum()

    return score, full_score_history 

def get_strategies(agents, decoded_history, score_history):
    agent1 = agents.loc[decoded_history["agent_1_id"]]
    agent2 = agents.loc[decoded_history["agent_2_id"]]

    history_strategies = pd.DataFrame({
        'agent_1_maneuver': agent1["maneuvers"].values,
        'agent_1_tactic' : agent1["tactics"].values,
        "agent_2_maneuver" : agent2["maneuvers"].values,
        "agent_2_tactic" : agent2["tactics"].values
    })

    agent = agents.loc[score_history["agent_id"]]

    score_strategies = pd.DataFrame({
        'maneuver': agent["maneuvers"].values,
        'tactic' : agent["tactics"].values,
    })
    return history_strategies, score_strategies

def get_colors(agents, decoded_history, score_history): #adds colors to the history

    agent_1 = agents.loc[decoded_history["agent_1_id"]]
    agent_2 = agents.loc[decoded_history["agent_2_id"]]

    # Directly create the colors DataFrame with the fill and outline values
    history_colors = pd.DataFrame({
        "agent_1_tactic_colors": agent_1["tactic_colors"].values,
        "agent_1_tactic_colors_binary": agent_1["tactic_colors_binary"].values,
        "agent_1_maneuver_colors": agent_1["maneuver_colors"].values,
        "agent_1_maneuver_colors_binary": agent_1["maneuver_colors_binary"].values,
        "agent_2_tactic_colors": agent_2["tactic_colors"].values,
        "agent_2_tactic_colors_binary": agent_2["tactic_colors_binary"].values,
        "agent_2_maneuver_colors": agent_2["maneuver_colors"].values,
        "agent_2_maneuver_colors_binary": agent_2["maneuver_colors_binary"].values
    })

    agent = agents.loc[score_history["agent_id"]]

    tactic_colors = agent['tactic_colors_binary'].values

    normalized_tactic_colors = np.stack([
        (tactic_colors >> 24) & 0xFF,
        (tactic_colors >> 16) & 0xFF,
        (tactic_colors >> 8) & 0xFF,
        tactic_colors & 0xFF
    ], axis=-1) / 255.0

    # Convert np.float64 elements to float (standard Python float)
    agent['normalized_tactic_colors'] = normalized_tactic_colors.astype(float).tolist()

    maneuver_colors = agent['maneuver_colors_binary'].values

    normalized_maneuver_colors = np.stack([
        (maneuver_colors >> 24) & 0xFF,
        (maneuver_colors >> 16) & 0xFF,
        (maneuver_colors >> 8) & 0xFF,
        maneuver_colors & 0xFF
    ], axis=-1) / 255.0

    # Convert np.float64 elements to float (standard Python float)
    agent['normalized_maneuver_colors'] = normalized_maneuver_colors.astype(float).tolist()
 

    score_colors = pd.DataFrame({
        "tactic_colors": agent["tactic_colors"].values,
        "tactic_colors_binary": agent["tactic_colors_binary"].values,
        "tactic_colors_normalized": agent["normalized_tactic_colors"].values,
        "maneuver_colors": agent["maneuver_colors"].values,
        "maneuver_colors_binary": agent["maneuver_colors_binary"].values,
        "maneuver_colors_normalized": agent["normalized_maneuver_colors"].values

    })

    return history_colors, score_colors

def clean_data(agents, history, settings, reward_matrix):

    decoded_history = decode(history, settings) #pre-cleaning

    score, score_history = get_scores(decoded_history, reward_matrix, settings)

    history_strategies, score_strategies = get_strategies(agents, decoded_history, score_history)

    history_colors, score_colors = get_colors(agents, decoded_history, score_history)

    cleaned_history = pd.concat([decoded_history, score, history_strategies, history_colors], axis=1)
    cleaned_history = cleaned_history.sort_values(by=['round', 'agent_1_id', 'agent_2_id'], ascending=[True, True, True]) 

    score_history = pd.concat([score_history,score_strategies, score_colors], axis=1)
    score_history = score_history.sort_values(by=['round', 'agent_id'], ascending=[True, True]) 

    if (args.verbose == True):
        
        print("Finished decoding")
        print(decoded_history)
        print("----------------------------------------------------------------")

        print("Calculated scores")
        print(score)
        print(score_history)
        print("----------------------------------------------------------------")

        print("Got strategies")
        print(history_strategies, score_strategies)
        print("----------------------------------------------------------------")

        print("Got colors")
        print(history_colors, score_strategies)
        print("----------------------------------------------------------------")
        
        print("Concatined cleaned history together")
        print("Sorted cleaned history")
        print(cleaned_history)
        print("----------------------------------------------------------------")
        
        print("Concatinated score history together")
        print("Sorted score history")
        print(score_history)
        print("----------------------------------------------------------------")

    return cleaned_history, score_history



agents = pd.read_csv(datadir + "Agents.csv", sep=';')

history = pd.read_csv(datadir + "Raw_History.csv", sep=';')

settings = pd.read_csv(datadir + "settings.csv", sep=';')

reward_matrix=np.array([[[3,3],[0,5]],[[5,0],[1,1]]])
clean_history, score_history = clean_data(agents, history, settings, reward_matrix)
clean_history.to_csv(datadir + "Clean_History.csv", index=False)
score_history.to_csv(datadir + "Score_History.csv", index=False)