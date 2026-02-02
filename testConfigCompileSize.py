# Build Automation Script  
# This script automates building a PlatformIO project with multiple configuration header files.
# 
# It works by:
# 1. Iterating over all .h files in the testConfig directory.
# 2. Copying each config file to the target project’s config.h path.
# 3. Running a PlatformIO build (pio run) for each configuration.
# 4. Parsing the build output to extract Flash and RAM usage (both bytes and percentages).
# 5. Recording whether the build succeeded or failed.
# 6. Writing the results into a CSV report testConfig/0_build_report.csv for easy comparison across configurations. Additionally an easier to read markdown file is generated.
# 7. This makes it easy to test how different configurations affect memory usage and build success.
#
# Prerequisite: The location of you pio.exe must be known on your path. 
# E.g. On Windows: C:\Users\xyz\.platformio\penv\Scripts
# On Linux: nano ~/.bashrc  -   export PATH="$HOME/.platformio/penv/bin:$PATH"

import os
import shutil
import re
import csv
import subprocess
from datetime import datetime

CONFIG_DIR = 'testConfig'
CONFIG_TARGET = 'spacemouse-keys/config.h'
PIO_CMD = "pio"              # Adjust if needed, e.g. full path to platformio.exe
BUILD_ARGS = ["run"]
REPORT_FILE = 'testConfig/0_build_report.csv'
REPORT_MD_FILE = 'testConfig/0_build_report.md'

# Markdown introduction text (supports multiple lines, Markdown formatting allowed)
REPORT_MD_INTRO = """# Build Report

This document summarizes the build results for all tested configurations.  
Each configuration corresponds to a different `config.h` variant given in folder `testConfig`.

The first line of each configuration file is used as its description.

This file is created manually be calling `testConfigCompileSize.py`.
"""

OK_MARK = "[OK]"
FAIL_MARK = "[FAIL]"
BUILD_MARK = "[BUILD]"

def parse_build_output(output):
    flash_used = None
    flash_total = None
    ram_used = None
    ram_total = None

    for line in output.splitlines():
        if 'RAM:' in line:
            match = re.search(r'used\s+(\d+)\s+bytes.*?(\d+)\s+bytes', line)
            if match:
                ram_used = int(match.group(1))
                ram_total = int(match.group(2))
        elif 'Flash:' in line or 'Program:' in line:
            match = re.search(r'used\s+(\d+)\s+bytes.*?(\d+)\s+bytes', line)
            if match:
                flash_used = int(match.group(1))
                flash_total = int(match.group(2))

    flash_pct = round(flash_used / flash_total * 100, 1) if flash_used and flash_total else None
    ram_pct = round(ram_used / ram_total * 100, 1) if ram_used and ram_total else None

    return flash_used, ram_used, flash_pct, ram_pct

def run_platformio_capture_output(args):
    try:
        result = subprocess.run(
            [PIO_CMD] + args,
            capture_output=True,
            text=True,
            check=True
        )
        return result.stdout, True
    except subprocess.CalledProcessError as e:
        return e.stdout + "\n" + e.stderr, False

def get_config_description(config_path):
    """Read first line of config file, strip leading // if present."""
    with open(config_path, "r", encoding="utf-8") as f:
        first_line = f.readline().strip()
    return first_line.lstrip("/ ").strip()

def run_automation():
    # Gather all .h files from the testConfig folder (as full relative paths)
    configs = [os.path.join(CONFIG_DIR, f) for f in os.listdir(CONFIG_DIR) if f.endswith('.h')]

    # Add the two sample configs (hardcoded) to the list of tested files
    extra_samples = ['spacemouse-keys/config_sample.h', 'spacemouse-keys/config_sample_hall_effect.h']
    for s in extra_samples:
        if s not in configs:
            configs.append(s)

    results = []
    all_success = True

    max_flash_pct = 0
    max_ram_pct = 0

    for config_path in configs:
        config_file = os.path.basename(config_path)
        print(f'\n{BUILD_MARK} Building with {config_file}...')

        if not os.path.exists(config_path):
            all_success = False
            print(f"{FAIL_MARK} Config file not found: {config_path}. Skipping.")
            results.append({
                'Config': config_file,
                'Description': 'File not found',
                'Flash (bytes)': 'N/A',
                'Flash (%)': 'N/A',
                'RAM (bytes)': 'N/A',
                'RAM (%)': 'N/A',
                'Build Success': 'No'
            })
            continue

        shutil.copyfile(config_path, CONFIG_TARGET)

        description = get_config_description(config_path)

        output, success = run_platformio_capture_output(BUILD_ARGS)
        flash, ram, flash_pct, ram_pct = parse_build_output(output)

        # track maxima
        if flash_pct is not None and flash_pct > max_flash_pct:
            max_flash_pct = flash_pct
        if ram_pct is not None and ram_pct > max_ram_pct:
            max_ram_pct = ram_pct

        if success:
            print(f"{OK_MARK} Build succeeded: Flash={flash} bytes ({flash_pct}%), RAM={ram} bytes ({ram_pct}%)")
        else:
            all_success = False
            print(f"{FAIL_MARK} Build failed for {config_file}")
            if flash or ram:
                print(f"   Info: Detected size before failure: Flash={flash} ({flash_pct}%), RAM={ram} ({ram_pct}%)")
            print(output)

        results.append({
            'Config': config_file,
            'Description': description,
            'Flash (bytes)': flash if flash is not None else 'N/A',
            'Flash (%)': flash_pct if flash_pct is not None else 'N/A',
            'RAM (bytes)': ram if ram is not None else 'N/A',
            'RAM (%)': ram_pct if ram_pct is not None else 'N/A',
            'Build Success': 'Yes' if success else 'No'
        })

    # Sort alphabetically by Config name
    results.sort(key=lambda x: x['Config'])

    # Add summary row with maxima
    summary_line = f"{OK_MARK} All builds successful." if all_success else f"{FAIL_MARK} Some builds failed."
    results.append({
        'Config': 'Summary',
        'Description': summary_line,
        'Flash (bytes)': '',
        'Flash (%)': f"Max: {max_flash_pct}",
        'RAM (bytes)': '',
        'RAM (%)': f"Max: {max_ram_pct}",
        'Build Success': f"{OK_MARK}" if all_success else f"{FAIL_MARK}"
    })

    # Write CSV
    with open(REPORT_FILE, 'w', newline='', encoding="utf-8") as csvfile:
        fieldnames = ['Config', 'Description', 'Flash (bytes)', 'Flash (%)', 'RAM (bytes)', 'RAM (%)', 'Build Success']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(results)

    # Write Markdown
    with open(REPORT_MD_FILE, 'w', encoding="utf-8") as mdfile:
        mdfile.write(REPORT_MD_INTRO.strip() + "\n\n")
        mdfile.write("| Config | Description | Flash (bytes) | Flash (%) | RAM (bytes) | RAM (%) | Build Success |\n")
        mdfile.write("|--------|-------------|---------------|-----------|-------------|---------|---------------|\n")
        for row in results:
            mdfile.write(
                f"| {row['Config']} | {row['Description']} | {row['Flash (bytes)']} | {row['Flash (%)']} "
                f"| {row['RAM (bytes)']} | {row['RAM (%)']} | {row['Build Success']} |\n"
            )
        mdfile.write("\n")
        mdfile.write(f"**Report generated on:** {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")

    print(f'\nReports generated:\n - {REPORT_FILE}\n - {REPORT_MD_FILE}')
    print(summary_line)


if __name__ == '__main__':
  run_automation()
