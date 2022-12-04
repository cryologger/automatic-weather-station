#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Nov 13 11:20:40 2022

@author: adam
"""

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as mticker
import matplotlib.dates as mdates

import seaborn as sns

import requests



# -----------------------------------------------------------------------------
# Plotting attributes
# -----------------------------------------------------------------------------

# Seaborn configuration
sns.set_theme(style="ticks")
sns.set_context("talk") # talk, paper, poster

lw = 2
interval = 30
date_format = "%Y-%m-%d"

# Figure DPI
dpi = 300

# Set colour palette
#sns.set_palette("turbo", 2)

# -----------------------------------------------------------------------------
# Load and prepare data
# -----------------------------------------------------------------------------

# Qakuqtaqtujut
response = requests.get("https://api.cryologger.org/aws?imei=300434063398110&records=5000")
df1 = pd.read_json(response.text)

# Pullataujaq
response = requests.get("https://api.cryologger.org/aws?imei=300434063396360&records=5000")
df2 = pd.read_json(response.text)

# Convert unixtime to datetime
df1["datetime"] = pd.to_datetime(df1["unixtime"], unit="s")
df2["datetime"] = pd.to_datetime(df2["unixtime"], unit="s")

# -----------------------------------------------------------------------------
# Plot
# -----------------------------------------------------------------------------

# Voltage
fig, ax = plt.subplots(figsize=(10,5))
ax.set(xlabel='Date', ylabel='Voltage (V)')
ax.grid(ls='dotted')
sns.lineplot(x='datetime', y='voltage', data=df1, errorbar=None, lw=lw, label="Qakuqtaqtujut")
sns.lineplot(x='datetime', y='voltage', data=df2, errorbar=None, lw=lw, label="Pullataujaq")

plt.xticks(rotation=45, horizontalalignment='center')
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
ax.xaxis.set_major_locator(mdates.DayLocator(interval=3)) 
ax.legend(loc='center left', bbox_to_anchor=(1.0, 0.5), title="Station")
# Save figure
plt.savefig("/Users/adam/Desktop/battery.png", dpi=dpi, transparent=False, bbox_inches="tight")
