import os
import shutil
import tkinter as tk
from tkinter import filedialog, simpledialog, messagebox
import sys
import re

sys.stdout.reconfigure(encoding='utf-8')

src_dir_toMove= r""
src_dir_name = r""
file_ext = ''
dest_dir = r""

def move_files_by_extension(src_dir,dest_dir):
    # Move files
    moved_count = 0
    for filename in os.listdir(src_dir):
        if filename.endswith(file_ext):
            src_file = os.path.join(src_dir, filename)
            dest_file = os.path.join(dest_dir, filename)
            try:
                shutil.move(src_file, dest_file)
                moved_count += 1
            except Exception as e:
                print(f"Failed to move {filename}: {e}")

    messagebox.showinfo("Done", f"Moved {moved_count} file(s) with extension {file_ext}.")

def change_files_name(src_dir):
    char_counter = 0
    if len(os.listdir(src_dir)) == 0:
        print("No files in the directory.")
        return
    
    for filename in os.listdir(src_dir):
        chars_to_remove = []

        for char in range(len(filename) - 1, -1, -1):
            char_counter += 1
            if filename[char] == '[':
                chars_to_remove.append(filename[char])
                new_name = filename[:-len(chars_to_remove)] + ".mp3"
                os.rename(os.path.join(src_dir, filename), os.path.join(src_dir, new_name))
                print(f"Renamed '{filename}' to '{new_name}'")
                char_counter = 0
                break

            if char_counter > 18:
                print(f"Filename '{filename}' is too long. Skipping.")
                char_counter = 0
                break
            chars_to_remove.append(filename[char])
def ChangeFileNameListIncreasement(folder_path,startingIndex):
    files = [
        f for f in os.listdir(folder_path)
        if os.path.isfile(os.path.join(folder_path, f))
    ]

    # Sort files for consistent numbering
    files.sort()

    # Save original filenames
    txt_path = os.path.join(folder_path, "original_filenames.txt")

    with open(txt_path, "w", encoding="utf-8") as txt_file:
        for filename in files:
            if filename == "original_filenames.txt":
                continue
            name, ext = os.path.splitext(filename)
            txt_file.write(f'"{name}",\n')

    # Rename files
    for index, filename in enumerate(files, start=startingIndex):
        # Skip the log file itself
        if filename == "original_filenames.txt":
            continue
        if re.match(r"^\d{4}_", filename):
            print(f"Skipping already indexed file: {filename}")
            continue

            
        old_path = os.path.join(folder_path, filename)

        name, ext = os.path.splitext(filename)

        # Format: 0001_[songname].mp3
        new_filename = f"{index:04d}_{name}{ext}"

        new_path = os.path.join(folder_path, new_filename)

        os.rename(old_path, new_path)

        print(f"{filename} -> {new_filename}")
    print(f"TXT file saved at: {txt_path}")

# Run the function
'''move_files_by_extension(src_dir_toMove, dest_dir)
change_files_name(src_dir_name)
ChangeFileNameListIncreasement(src_dir_name, 1)'''
