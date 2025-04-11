#!/bin/bash

# Rescan for available Wi-Fi networks
echo "Rescanning for available networks..."
sudo nmcli device wifi rescan

# List available Wi-Fi networks
echo "Available Wi-Fi networks:"
nmcli device wifi list

# Prompt for SSID (with spaces in quotes)
echo -n "Enter SSID (Network Name): "
read SSID

# Prompt for password
echo -n "Enter password for '$SSID': "
read -s PASSWORD
echo

# Connect to the selected network
echo "Connecting to '$SSID'..."
sudo nmcli device wifi connect "$SSID" password "$PASSWORD"

# Check if the connection was successful
if [ $? -eq 0 ]; then
    echo "Successfully connected to '$SSID'."
else
    echo "Failed to connect to '$SSID'."
fi
