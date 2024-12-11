import matplotlib.pyplot as plt
import re
from collections import defaultdict
from datetime import datetime

# Read the log file
log_file_path = 'express.log'
priority_data = defaultdict(list)
time_stamps_data = defaultdict(list)

with open(log_file_path, 'r') as f:
    for line in f:
        # Extract the timestamp and priority information
        match = re.match(r'^.*?(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d{6}Z).*?Priority: (\d+), Current length: (\d+)', line)
        
        if match:
            timestamp, priority, length = match.groups()
            priority = int(priority)
            length = int(length)
            
            # Parse timestamp to datetime object
            timestamp = datetime.strptime(timestamp, '%Y-%m-%dT%H:%M:%S.%fZ')
            
            time_stamps_data[priority].append(timestamp)
            priority_data[priority].append(length)

# Plot the data for each priority
plt.figure(figsize=(12, 8))

for priority in sorted(priority_data.keys()):
    # Convert timestamps to elapsed time (seconds) relative to the first timestamp
    timestamps = time_stamps_data[priority]
    start_time = timestamps[0]
    elapsed_time = [(ts - start_time).total_seconds() for ts in timestamps]
    
    lengths = priority_data[priority]
    plt.plot(elapsed_time, lengths, label=f'Priority {priority}')

plt.xlabel('Elapsed Time (Seconds)')
plt.ylabel('Queue Length')
plt.title('Queue Lengths Over Time by Priority (Zenoh Server)')
plt.legend()
plt.grid(True)
plt.show()
