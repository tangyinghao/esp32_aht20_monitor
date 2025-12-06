#!/usr/bin/env python3
"""
Plot temperature and humidity from readings.csv
Usage: python plot_readings.py [readings.csv]
"""

import sys
import pandas as pd
import matplotlib.pyplot as plt
from datetime import datetime

def plot_readings(csv_file='readings.csv'):
    """Read CSV and plot temperature & humidity over time."""
    try:
        # Read CSV without header (PlatformIO monitor adds startup text), skip lines until data starts
        df = pd.read_csv(csv_file, header=None, skiprows=lambda x: x < 5 or '---' in str(x), on_bad_lines='skip')
        
        # Rename columns to expected names
        df.columns = ['timestamp', 'temp_C', 'humidity_RH']
        
        # Parse timestamp column as datetime
        df['timestamp'] = pd.to_datetime(df['timestamp'], errors='coerce')
        
        # Remove rows with invalid timestamps
        df = df.dropna(subset=['timestamp'])
        
        if df.empty:
            print(f"Error: {csv_file} is empty or has no valid rows.")
            return
        
        print(f"Loaded {len(df)} readings from {csv_file}")
        print(f"Time range: {df['timestamp'].min()} to {df['timestamp'].max()}")
        
        # Create figure with two subplots (temperature and humidity)
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8), sharex=True)
        fig.suptitle('Room Temperature & Humidity Monitor', fontsize=14, fontweight='bold')
        
        # Plot temperature
        ax1.plot(df['timestamp'], df['temp_C'], color='red', linewidth=2, label='Temperature')
        ax1.fill_between(df['timestamp'], df['temp_C'], alpha=0.3, color='red')
        ax1.set_ylabel('Temperature (°C)', fontsize=11)
        ax1.set_ylim(15, 30)  # Restrict y-axis to 15-30°C to show small changes
        ax1.grid(True, alpha=0.3)
        ax1.legend(loc='upper left')
        
        # Plot humidity
        ax2.plot(df['timestamp'], df['humidity_RH'], color='blue', linewidth=2, label='Humidity')
        ax2.fill_between(df['timestamp'], df['humidity_RH'], alpha=0.3, color='blue')
        ax2.set_ylabel('Humidity (% RH)', fontsize=11)
        ax2.set_xlabel('Time', fontsize=11)
        ax2.grid(True, alpha=0.3)
        ax2.legend(loc='upper left')
        
        # Rotate x-axis labels for readability
        fig.autofmt_xdate(rotation=45, ha='right')
        
        plt.tight_layout()
        
        # Save figure
        output_file = csv_file.replace('.csv', '_plot.png')
        plt.savefig(output_file, dpi=100, bbox_inches='tight')
        print(f"✅ Plot saved to {output_file}")
        
        # Show plot
        plt.show()
        
    except FileNotFoundError:
        print(f"Error: {csv_file} not found.")
        sys.exit(1)
    except pd.errors.EmptyDataError:
        print(f"Error: {csv_file} is empty or invalid format.")
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == '__main__':
    csv_file = sys.argv[1] if len(sys.argv) > 1 else 'readings.csv'
    plot_readings(csv_file)
