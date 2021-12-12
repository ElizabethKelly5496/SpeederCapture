"""
Elizabeth Kelly
SWENG452 Final Project - Speeder Capture
Python program to upload file to GCP cloud storage bucket
"""

import os
import sys
from google.cloud import storage

# Authentication Credentials
os.environ['GOOGLE_APPLICATION_CREDENTIALS'] = 'sweng452-3eda5fa68164.json'

# Open access to cloud storage
storage_client = storage.Client()

# Access the cloud storage bucket - bucket name passed by 2nd argument
bucket = storage_client.get_bucket(sys.argv[2])

# Set file name of bucket. File name passed as 1st argument
blob = bucket.blob(sys.argv[1])

# Upload file on RPi to bucket
blob.upload_from_filename(sys.argv[1])

# Delete file from RPi
os.remove(sys.argv[1])
