#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun May 15 09:55:23 2022

@author: adam
"""

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as mticker
import matplotlib.dates as mdates
import seaborn as sns
import numpy as np

# -----------------------------------------------------------------------------
# Library Configuration
# -----------------------------------------------------------------------------

# Configure Seaborn styles
sns.set_theme(style="ticks")
sns.set_style({"xtick.direction": "in", "ytick.direction": "in"})

sns.set_context("talk")  # Options: talk, paper, poster

# Set figure DPI
dpi = 300

# -----------------------------------------------------------------------------
# Paths
# -----------------------------------------------------------------------------

# Path to figures
path_figures = (
    "/Users/adam/Documents/GitHub/Cryologger_Automatic_Weather_Station/Software/Python/"
)

# Path to data
path_data = (
    "/Users/adam/Documents/GitHub/Cryologger_Automatic_Weather_Station/Software/Python/"
)


# -----------------------------------------------------------------------------
# Load and prepare data
# -----------------------------------------------------------------------------

df = pd.read_csv(
    path_data + "2022_aws_arctic_bay.csv",
    index_col=False,
)

# Convert to datetime
df["Datetime"] = pd.to_datetime(df["Datetime"].astype(str), format="%Y-%m-%d %H:%M:%S")

# -----------------------------------------------------------------------------
# Plot temperature, wind speed, and wind direction
# -----------------------------------------------------------------------------

fig, axs = plt.subplots(3, 1, figsize=(12, 12), sharex=True, constrained_layout=True)
sns.lineplot(
    x="Datetime",
    y="Temperature (°C)",
    data=df,
    lw=1,
    ci=None,
    color="red",
    ax=axs[0],
)
sns.lineplot(
    x="Datetime",
    y="Wind Speed (km/h)",
    data=df,
    lw=1,
    ci=None,
    color="blue",
    ax=axs[1],
)
sns.lineplot(
    x="Datetime",
    y="Wind Direction (°)",
    data=df,
    lw=1,
    ci=None,
    color="green",
    ax=axs[2],
)

axs[2].xaxis.set_major_locator(mdates.DayLocator(interval=7))
axs[2].xaxis.set_major_formatter(mdates.DateFormatter("%Y-%m-%d"))
axs[2].set(xlabel=" ")
sns.despine()
fig.autofmt_xdate(rotation=45)
fig.align_ylabels()

# Save figure
fig.savefig(path_figures + "temp_wind4.png", dpi=dpi, transparent=False, bbox_inches="tight")


# -----------------------------------------------------------------------------
# Plot battery voltage
# -----------------------------------------------------------------------------

fig, axs = plt.subplots(1, 1, figsize=(12, 6), constrained_layout=True)
sns.lineplot(
    x="Datetime",
    y="Voltage (V)",
    data=df,
    lw=1,
    ci=None,
    ax=axs,
)

axs.grid(ls="dotted")
axs.xaxis.set_major_locator(mdates.DayLocator(interval=7))
axs.xaxis.set_major_formatter(mdates.DateFormatter("%Y-%m-%d"))
axs.set(xlabel=" ")
#sns.despine()
fig.autofmt_xdate(rotation=45)
fig.align_ylabels()

# Save figure
fig.savefig(
    path_figures + "voltage.png", dpi=dpi, transparent=False, bbox_inches="tight"
)


# -----------------------------------------------------------------------------
# Transmit Duration
# -----------------------------------------------------------------------------

fig, axs = plt.subplots(1, 1, figsize=(12, 6), constrained_layout=True)
sns.lineplot(
    x="Datetime",
    y="Transmit Duration (s)",
    data=df,
    lw=1,
    ci=None,
    ax=axs,
)
axs.grid(ls="dotted")
axs.xaxis.set_major_locator(mdates.DayLocator(interval=7))
axs.xaxis.set_major_formatter(mdates.DateFormatter("%Y-%m-%d"))
axs.set(xlabel=' ')
#sns.despine()
fig.autofmt_xdate(rotation=45)
fig.align_ylabels()

# Save figure
fig.savefig(
    path_figures + "transmit.png", dpi=dpi, transparent=False, bbox_inches="tight"
)
