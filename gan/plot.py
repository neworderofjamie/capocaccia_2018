import csv
import matplotlib.pyplot as plt
import numpy as np

times = [10.0, 40.0, 45.0, 75.0]

def plot_times(axis, times):
    for t in times:
        axis.axvline(t, linestyle="--")
        
with open("spikes.csv", "rb") as spike_csv_file, open("voltages.csv", "rb") as voltage_csv_file:
    spike_csv_reader = csv.reader(spike_csv_file, delimiter = ",")
    voltage_csv_reader = csv.reader(voltage_csv_file, delimiter = ",")

    # Skip headers
    spike_csv_reader.next()
    voltage_csv_reader.next()
    
    # Read data and zip into columns
    spike_data_columns = zip(*spike_csv_reader)
    voltage_data_columns = zip(*voltage_csv_reader)

    # Convert to numpy
    spike_times = np.asarray(spike_data_columns[0], dtype=float)
    spike_id = np.asarray(spike_data_columns[1], dtype=int)
    voltage_time = np.asarray(voltage_data_columns[0], dtype=float)
    voltage = np.asarray(voltage_data_columns[2], dtype=float)
    
    # Reshape voltage so it's one column per neuron
    voltage_time = voltage_time.reshape((-1, 4))
    voltage = voltage.reshape((-1, 4))
    
    # Create plot
    figure, axes = plt.subplots(2, sharex=True)

    # Plot voltages
    for i in range(voltage.shape[1]):
        axes[0].plot(voltage_time[:,i], voltage[:,i], label="%u" % i)
    plot_times(axes[0], times)
    
    # Plot spikes
    axes[1].scatter(spike_times, spike_id, s=2)
    plot_times(axes[1], times)
    
    axes[1].set_ylim((0, 4))
    axes[0].set_ylabel("Membrane voltage [mV]")
    axes[1].set_xlabel("Time [ms]")
    axes[1].set_ylabel("Neuron ID")
    
    axes[0].legend()
    
    # Show plot
    plt.show()
