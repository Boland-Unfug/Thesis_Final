import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import argparse as arg
import ast

parser = arg.ArgumentParser(description="Data Analysis Settings")

parser.add_argument('--dir', type=str, default='..\\Data\\Iteration_0\\', help='Type the path to the files to analyze.')
parser.add_argument('--no_draw', action='store_false', dest='draw', help="Disable plot drawing (default: drawing enabled)")
parser.add_argument('--verbose', action='store_true', help="maximal terminal output")

args = parser.parse_args()
if (args.verbose == True):
    print(args)
datadir = args.dir
draw = args.draw


datadir = datadir.replace("\\", "/")
datadir = datadir.replace("..",".")


agents = pd.read_csv(datadir + "Agents.csv")

history = pd.read_csv(datadir + "Clean_History.csv")

score_history = pd.read_csv(datadir + "Score_History.csv")

score_history["fill"] = score_history["maneuver_colors_normalized"].apply(ast.literal_eval)
score_history['fill'] = score_history['fill'].apply(tuple)
score_history["outline"] = score_history["tactic_colors_normalized"].apply(ast.literal_eval)
score_history['outline'] = score_history['outline'].apply(tuple)

settings = pd.read_csv(datadir + "Settings.csv")

def plot_maneuver_score_history(score_history, draw):
    for agent_id, group in score_history.groupby("agent_id"):
        if (agent_id != 0):
            plt.plot(
                group["round"], 
                group["cumulative_reward"], 
                color=group["fill"].iloc[0],  # Get the agent's color
                linewidth=2.0,
                alpha=0.3 # used bc lots of overlap
            )

    unique_maneuvers = score_history[['maneuver', 'fill']].drop_duplicates()

    # Create legend handles dynamically
    legend_handles = [
        plt.Line2D([0], [0], color=color, lw=2, label=name)
        for name, color in zip(unique_maneuvers['maneuver'], unique_maneuvers['fill'])
    ]

    # Add the legend to the plot
    plt.legend(handles=legend_handles, title="Maneuvers")
    
    # Customize plot
    plt.title("Cumulative Rewards by Agent")
    plt.xlabel("Round")
    plt.ylabel("Cumulative Reward")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(datadir + "Maneuver Score History.png")
    if (draw == True): plt.show()
    
def plot_tactic_score_history(score_history, draw):
    for agent_id, group in score_history.groupby("agent_id"):
        if (agent_id != 0):
            plt.plot(
                group["round"], 
                group["cumulative_reward"], 
                color=group["outline"].iloc[0],  # Get the agent's color
                linewidth=2.0,
                alpha=0.3 # used bc lots of overlap
            )

    unique_maneuvers = score_history[['tactic', 'outline']].drop_duplicates()

    # Create legend handles dynamically
    legend_handles = [
        plt.Line2D([0], [0], color=color, lw=2, label=name)
        for name, color in zip(unique_maneuvers['tactic'], unique_maneuvers['outline'])
    ]

    # Add the legend to the plot
    plt.legend(handles=legend_handles, title="Tactics")
    
    # Customize plot
    plt.title("Cumulative Rewards by Agent")
    plt.xlabel("Round")
    plt.ylabel("Cumulative Reward")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(datadir + "Tactic Score History.png")
    if (draw == True): plt.show()
    
# for the above: get the top 3 and bottom 3 agents and add them to the legend

# next step is get the average
plot_maneuver_score_history(score_history, draw)
plot_tactic_score_history(score_history, draw)