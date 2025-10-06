#!/bin/bash

#Generated using AI because I don't know bash !

# Prompt for commit message
read -p "Enter commit message: " commit_msg

# If empty message, exit
if [ -z "$commit_msg" ]; then
  echo "Commit message cannot be empty. Aborting."
  exit 1
fi

# Add all changes including deletions
git add -A

# Commit with your message
git commit -m "$commit_msg"

# Push to remote main branch
git push origin main --force
