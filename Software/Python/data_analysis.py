#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun May 15 09:55:23 2022

@author: Adam Garbo

Description:
    Code to analyze the operation of the Cryologger AWS.
"""

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as mticker
import matplotlib.dates as mdates
import numpy as np
import seaborn as sns


# -----------------------------------------------------------------------------
# Plotting attributes
# -----------------------------------------------------------------------------

# Seaborn configuration
sns.set_theme(style="ticks")
sns.set_context("talk") # talk, paper, poster

# Set colour palette
sns.set_palette("colorblind")

# Graph attributes
lw = 1
interval = 30
date_format = "%Y-%m-%d"

# Figure DPI
dpi = 300


# Test
# Set ticks to point inward
sns.set_style({"xtick.direction": "in", "ytick.direction": "in"})

# -----------------------------------------------------------------------------
# Folder paths
# -----------------------------------------------------------------------------

# Python code directory
path = "/Users/adam/Documents/GitHub/automatic-weather-station/Software/Python/"

# Data directory
path_data = "/Users/adam/Documents/GitHub/automatic-weather-station/Software/Python"

# Figure directory
path_figures = "/Users/adam/Documents/GitHub/automatic-weather-station/Software/Python/Figures/"

# -----------------------------------------------------------------------------
# Download and clean data exported from MariaDB
# -----------------------------------------------------------------------------

df1 = pd.read_csv("/Users/adam/Downloads/cryologger_aws.csv", index_col=False)

df1 = pd.read_csv(path_data + "2022_aws_arctic_bay.csv", index_col=False)
df2 = pd.read_csv(path_data + "2022_aws_milne.csv", index_col=False)

# Convert unixtime to datetime
df1["datetime"] = pd.to_datetime(df1["unixtime"], unit="s")
df2["datetime"] = pd.to_datetime(df2["unixtime"], unit="s")

# Convert datetime
df1["datetime"] = pd.to_datetime(df1["datetime"].astype(str), format="%Y-%m-%d %H:%M:%S")


# Convert IMEI to string
df1["imei"] = df1["imei"].astype(str)
df2["imei"] = df2["imei"].astype(str)

# Subset data based on dates
df1 = df1[(df1["datetime"] > "2022-11-01 00:00")]
df2 = df2[(df2["datetime"] > "2022-12-01 00:00")]

# Set zero transmission duration values to null
df1.loc[df1["transmit_duration"] == 0, "transmit_duration"] = np.nan
df2.loc[df2["transmit_duration"] == 0, "transmit_duration"] = np.nan


# -----------------------------------------------------------------------------
# Download and clean data exported from WordPress
# -----------------------------------------------------------------------------

# Load most recent output file downloaded from WordPress
df = pd.read_csv("/Users/adam/Downloads/2021_itb_amundsen (3).csv", index_col=False)

# Convert to datetime
df["Datetime"] = pd.to_datetime(df["Datetime"].astype(str), format="%Y-%m-%d %H:%M:%S")

# Convert IMEI to string
df["IMEI"] = df["IMEI"].astype(str)

# Set zero transmission duration values to null
df.loc[df["Transmit Duration (s)"] == 0, "Transmit Duration (s)"] = np.nan

# -----------------------------------------------------------------------------
# Download and clean data from microSD card
# -----------------------------------------------------------------------------

# Load most recent output file downloaded from WordPress
df = pd.read_csv(path_data + "/AWS_LVY_20231119_133401.csv", index_col=False)

# Convert to datetime
df["datetime"] = pd.to_datetime(df["datetime"].astype(str), format="%Y-%m-%d %H:%M:%S")


# -----------------------------------------------------------------------------
# Plot temperature, wind speed, and wind direction
# -----------------------------------------------------------------------------

df = df2

fig, axs = plt.subplots(3, 1, figsize=(12, 12), sharex=True)
sns.lineplot(
    x="datetime",
    y="temperature_int",
    data=df,
    lw=1,
    errorbar=None,
    hue="imei",
    ax=axs[0],
)
sns.lineplot(
    x="datetime",
    y="wind_speed",
    data=df,
    lw=1,
    errorbar=None,
    hue="imei",
    ax=axs[1],
)
sns.lineplot(
    x="datetime",
    y="wind_direction",
    data=df,
    lw=1,
    errorbar=None,
    hue="imei",
    ax=axs[2],
)

axs[0].get_legend().remove()
axs[1].get_legend().remove()
axs[2].get_legend().remove()
axs[2].xaxis.set_major_locator(mdates.MonthLocator(interval=interval))
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

fig, axs = plt.subplots(1, 1, figsize=(12, 6))
sns.lineplot(
    x="datetime",
    y="voltage",
    data=df,
    lw=1,
    errorbar=None,
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
# Plot battery voltage & temperature
# -----------------------------------------------------------------------------


# Twin plot
fig, ax1 = plt.subplots(figsize=(10,5))
sns.lineplot(x="datetime", y="voltage", data=df1, errorbar=None, lw=lw, color=colours[0], label="Voltage (V)") #hue="imei"
ax1.set(xlabel=None, ylabel="Voltage (V)", ylim=(12,15.5))
ax1.xaxis.set_major_locator(mdates.DayLocator(interval=7))
ax1.xaxis.set_major_formatter(mdates.DateFormatter("%Y-%m-%d"))
ax1.get_legend().remove()
ax2 = plt.twinx()
sns.lineplot(x="datetime", y="temperature_int", data=df1, errorbar=None, lw=lw, color=colours[1], label="Temperature (°C)")
ax2.set(xlabel=None, ylabel="Temperature (°C)", ylim=(-50,0))
ax2.xaxis.set_major_locator(mdates.DayLocator(interval=7))
ax2.xaxis.set_major_formatter(mdates.DateFormatter("%Y-%m-%d"))
ax2.get_legend().remove()
fig.autofmt_xdate(rotation=45)
#fig.legend(bbox_to_anchor=(1.25, 0.4))
fig.legend(loc="lower center", ncol=2, bbox_to_anchor=(0.5, -0.15))
# Save figure
plt.savefig(path_figures + "temp_volt.png", dpi=dpi, transparent=False, bbox_inches='tight')
 
# -----------------------------------------------------------------------------
# Transmit Duration
# -----------------------------------------------------------------------------

df1["transmit_duration"].describe()
df2["transmit_duration"].describe()


df1.columns
df2.columns

df3 = pd.concat([df1, df2]) 
df3["imei"] = df3["imei"].astype(str)

# Scatterplot
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.scatterplot(x="datetime", y="transmit_duration", data=df2, hue="imei")
ax.set(xlabel=None, ylabel="Transmit Duration (s)", ylim=(-5,225))
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
ax.xaxis.set_major_locator(mdates.MonthLocator(interval=1)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Milne Fiord")
plt.savefig(path_figures + "transmit_duration_mf.png", dpi=dpi, transparent=False, bbox_inches="tight")


# Boxplot
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.boxplot(data=df3, x="imei", y="transmit_duration", hue="imei", dodge=False)
ax.set(xlabel=None, ylabel="Transmit Duration (s)")
ax.set_yscale("log")
plt.xticks(rotation=45, horizontalalignment="right")
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="IMEI")
ax.axhline(30, ls="dashed")
plt.savefig(path_figures + "transmit_duration_box.png", dpi=dpi, transparent=False, bbox_inches="tight")


sns.boxplot(data=df2, x="imei", y="transmit_duration")

# -----------------------------------------------------------------------------
# Temperature (°C)
# -----------------------------------------------------------------------------

date_format = "%H:%M"
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="temperature_int", data=df1, errorbar=None, lw=lw, label="Interal")
sns.lineplot(x="datetime", y="temperature_ext", data=df1, errorbar=None, lw=lw, label="External")
ax.set(xlabel=None, ylabel="Temperature (°C)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
#ax.xaxis.set_major_locator(mdates.MonthLocator(interval=1)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Sensor")
plt.savefig(path_figures + "temperature_int.png", dpi=dpi, transparent=False, bbox_inches="tight")

# -----------------------------------------------------------------------------
# Humidity (%)
# -----------------------------------------------------------------------------

date_format = "%H:%M"
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="humidity_int", data=df1, errorbar=None, lw=lw, label="Interal")
sns.lineplot(x="datetime", y="humidity_ext", data=df1, errorbar=None, lw=lw, label="External")
ax.set(xlabel=None, ylabel="Humiidty (%)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
#ax.xaxis.set_major_locator(mdates.MonthLocator(interval=1)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Sensor")
plt.savefig(path_figures + "humidity_int.png", dpi=dpi, transparent=False, bbox_inches="tight")
