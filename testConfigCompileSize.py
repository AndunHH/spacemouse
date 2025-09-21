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
Each configuration corresponds to a different `config.h` variant.  

The first line of each configuration file is used as its description.
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
    configs = [f for f in os.listdir(CONFIG_DIR) if f.endswith('.h')]
    results = []
    all_success = True

    for config_file in configs:
        print(f'\n{BUILD_MARK} Building with {config_file}...')

        config_path = os.path.join(CONFIG_DIR, config_file)
        shutil.copyfile(config_path, CONFIG_TARGET)

        description = get_config_description(config_path)

        output, success = run_platformio_capture_output(BUILD_ARGS)
        flash, ram, flash_pct, ram_pct = parse_build_output(output)

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

    # Add summary row (always at bottom)
    summary_line = f"{OK_MARK} All builds successful" if all_success else f"{FAIL_MARK} Some builds failed"
    results.append({
        'Config': summary_line,
        'Description': '',
        'Flash (bytes)': '',
        'Flash (%)': '',
        'RAM (bytes)': '',
        'RAM (%)': '',
        'Build Success': ''
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
