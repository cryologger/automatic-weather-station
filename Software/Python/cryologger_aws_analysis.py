#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun May 15 09:55:23 2022

@author: Adam Garbo

Description:
    Code to analyze the operation of the Cryologger Automatic Weather Station (AWS).
"""

# -----------------------------------------------------------------------------
# Librarires
# -----------------------------------------------------------------------------
 
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as mticker
import matplotlib.dates as mdates
import cartopy.crs as ccrs
import cartopy.feature as cfeature
import pyproj
import seaborn as sns
from cartopy.mpl.ticker import (LongitudeFormatter, 
                                LatitudeFormatter,
                                LongitudeLocator,
                                LatitudeLocator)

# -----------------------------------------------------------------------------
# Plotting attributes
# -----------------------------------------------------------------------------

# Seaborn configuration
sns.set_theme(style="ticks")
sns.set_context("talk") # talk, paper, poster

# Set colour palette
sns.set_palette("colorblind") # turbo

# Graph attributes
lw = 1
interval = 30
date_format = "%Y-%m-%d"

# Figure DPI
dpi = 300

# -----------------------------------------------------------------------------
# Mapping attributes
# -----------------------------------------------------------------------------

# Add Natural Earth coastline
coast = cfeature.NaturalEarthFeature("physical", "land", "10m",
                                     edgecolor="black",
                                     facecolor="lightgray",
                                     lw=0.75)

# Add Natural Earth coastline
coastline = cfeature.NaturalEarthFeature("physical", "coastline", "10m",
                                         edgecolor="black",
                                         facecolor="none",
                                         lw=0.75)

# -----------------------------------------------------------------------------
# Folder paths
# -----------------------------------------------------------------------------

# Data directory
path = "/Users/adam/Documents/GitHub/automatic-weather-station/Software/Python/"

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
# Plot
# -----------------------------------------------------------------------------

# Temperature Internal (°C)
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="temperature_int", data=df, errorbar=None, lw=lw, label="Internal")
sns.lineplot(x="datetime", y="temperature_ext", data=df, errorbar=None, lw=lw, label="External")
ax.set(xlabel=None, ylabel="Temperature (°C)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
#ax.xaxis.set_major_locator(mdates.DayLocator(interval=interval))
#ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Sensor")
plt.savefig(path_figures + "01_temperature.png", dpi=dpi, transparent=False, bbox_inches="tight")

# Temperature External (°C)
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="temperature_ext", data=df, errorbar=None, lw=lw, hue="uid")
ax.set(xlabel=None, ylabel="Temperature External (°C)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
#ax.xaxis.set_major_locator(mdates.DayLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
plt.savefig(path_figures + "01_temperature_ext.png", dpi=dpi, transparent=False, bbox_inches="tight")

# Humidity (%)
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="humidity_int", data=df, errorbar=None, lw=lw, label="Internal")
sns.lineplot(x="datetime", y="humidity_ext", data=df, errorbar=None, lw=lw, label="External")
ax.set(xlabel=None, ylabel="Humidity (%)", ylim=(0,100))
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
#ax.xaxis.set_major_locator(mdates.DayLocator(interval=interval))
#ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
plt.savefig(path_figures + "02_humidity.png", dpi=dpi, transparent=False, bbox_inches="tight")

# Wind Speed
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="wind_speed", data=df, errorbar=None, lw=lw, hue="uid")
ax.set(xlabel=None, ylabel="Wind Speed (km/h)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
#ax.xaxis.set_major_locator(mdates.DayLocator(interval=interval)) 
#ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
plt.savefig(path_figures + "03_wind_speed.png", dpi=dpi, transparent=False, bbox_inches="tight")

# Wind Gust Speed 
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="wind_gust_speed", data=df, errorbar=None, lw=lw, hue="uid")
ax.set(xlabel=None, ylabel="Wind Gust Speed (km/h)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
#ax.xaxis.set_major_locator(mdates.DayLocator(interval=interval)) 
#ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
plt.savefig(path_figures + "04_wind_gust_speed.png", dpi=dpi, transparent=False, bbox_inches="tight")

# Wind Direction (°)
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="wind_direction", data=df, errorbar=None, lw=lw, hue="uid")
ax.set(xlabel=None, ylabel="Wind Direction (°)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
#ax.xaxis.set_major_locator(mdates.DayLocator(interval=interval)) 
#ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
plt.savefig(path_figures + "05_wind_direction.png", dpi=dpi, transparent=False, bbox_inches="tight")

# Pitch (°)
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="pitch", data=df, errorbar=None, lw=lw, hue="uid")
ax.set(xlabel=None, ylabel="Pitch (°)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
#ax.xaxis.set_major_locator(mdates.DayLocator(interval=interval))
#ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
plt.savefig(path_figures + "06_pitch.png", dpi=dpi, transparent=False, bbox_inches="tight")

# Roll (°)
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="roll", data=df, errorbar=None, lw=lw, hue="uid")
ax.set(xlabel=None, ylabel="Roll (°)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
#ax.xaxis.set_major_locator(mdates.DayLocator(interval=interval))
#ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
plt.savefig(path_figures + "07_roll.png", dpi=dpi, transparent=False, bbox_inches="tight")

# Pitch & Roll (°)
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="pitch", data=df, errorbar=None, lw=lw, label="Pitch")
sns.lineplot(x="datetime", y="roll", data=df, errorbar=None, lw=lw, label="Roll")
ax.set(xlabel=None, ylabel="Roll (°)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
#ax.xaxis.set_major_locator(mdates.DayLocator(interval=interval))
#ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Axis")
plt.savefig(path_figures + "pitch_roll.png", dpi=dpi, transparent=False, bbox_inches="tight")


# Solar ()
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="solar", data=df, errorbar=None, lw=lw, hue="uid")
ax.set(xlabel=None, ylabel="Solar (°)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
#ax.xaxis.set_major_locator(mdates.DayLocator(interval=interval))
#ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
plt.savefig(path_figures + "08_solar.png", dpi=dpi, transparent=False, bbox_inches="tight")

# Snow ()
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="snow_depth", data=df, errorbar=None, lw=lw, hue="uid")
ax.set(xlabel=None, ylabel="Snow depth (mm)")
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
ax.xaxis.set_major_locator(mdates.DayLocator(interval=interval))
#ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
plt.savefig(path_figures + "09_snow_fon.png", dpi=dpi, transparent=False, bbox_inches="tight")


# Transmit Duration
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.scatterplot(x="datetime", y="transmit_duration", data=df, hue="uid")
ax.set(xlabel=None, ylabel="Transmit Duration (s)", ylim=(-15,255))
plt.xticks(rotation=45, horizontalalignment="right")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
#ax.xaxis.set_major_locator(mdates.DayLocator(interval=interval))
#ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
plt.savefig(path_figures + "10_transmit_duration.png", dpi=dpi, transparent=False, bbox_inches="tight")

# Voltage
fig, ax = plt.subplots(figsize=(10,5))
ax.grid(ls="dotted")
sns.lineplot(x="datetime", y="voltage", data=df, errorbar=None, lw=lw, hue="uid")
ax.set(xlabel=None, ylabel="Voltage (V)")
plt.xticks(rotation=45, horizontalalignment="center")
ax.xaxis.set_major_formatter(mdates.DateFormatter(date_format))
#ax.xaxis.set_major_locator(mdates.DayLocator(interval=interval))
#ax.xaxis.set_major_locator(mdates.MonthLocator(interval=interval)) 
ax.legend(loc="center left", bbox_to_anchor=(1.0, 0.5), title="Station")
plt.savefig(path_figures + "11_voltage.png", dpi=dpi, transparent=False, bbox_inches="tight")

# -----------------------------------------------------------------------------
# Load data for deployments
# -----------------------------------------------------------------------------

# Load data
df = pd.read_csv(path + "cryologger_aws_metadata.csv", index_col=False)

df = df[::-1]

# Subset by visit
df = df[df["reason_for_visit"].isin(["deployment"])]

# Convert to datetime
df["datetime"] = pd.to_datetime(df["datetime_onsite_start"].astype(str))

# Year
df["year"] = df["datetime"].dt.year
df["year"] = df["year"].astype(str)



# -----------------------------------------------------------------------------
# Map global deployments
# -----------------------------------------------------------------------------

# Set extents
x1 = -110
x2 = -50
y1 = 40
y2 = 83.5

# Plot map
plt.figure(figsize=(10,10))
ax = plt.axes(projection=ccrs.LambertConformal(df["longitude"].median(), df["latitude"].median())) # Centre of extents
#ax = plt.axes(projection=ccrs.Orthographic(-60, 80)) # Centre of extents

#ax.set_global()
ax.set_extent([x1, x2, y1, y2])
ax.add_feature(coastline)
#ax.add_feature(coast)
#ax.set_adjustable("datalim")
ax.add_feature(cfeature.OCEAN, zorder=0)
ax.add_feature(cfeature.LAND, zorder=0, )


gl = ax.gridlines(crs=ccrs.PlateCarree(), draw_labels=True,
                  color="black", alpha=0.25, linestyle="dotted",
                  x_inline=False, y_inline=False)
gl.top_labels = False
gl.right_labels = False
gl.rotate_labels = False
gl.xlocator = mticker.FixedLocator(np.arange(-180,0,30))
gl.ylocator = mticker.FixedLocator(np.arange(40,90,10))
gl.xformatter = LongitudeFormatter()
gl.yformatter = LatitudeFormatter()
gl.xpadding=10
sns.scatterplot(x="longitude", y="latitude", hue="year", style="year",
                markers=True, 
                data=df, zorder=10, 
                s=400, linewidth=1, edgecolor="black", legend="full",
                transform=ccrs.PlateCarree())
ax.legend(loc=3, title="Year", framealpha=1)

# Add scale bar
#scale_bar(ax, 100, (0.8, 0.025), 3)

# Save figure
plt.savefig(path_figures + "cryologger_aws_deployments.png", dpi=dpi, transparent=False, bbox_inches="tight")




